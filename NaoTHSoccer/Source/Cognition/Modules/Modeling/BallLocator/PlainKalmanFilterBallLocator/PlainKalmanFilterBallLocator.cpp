#include "PlainKalmanFilterBallLocator.h"

#include <Eigen/Eigenvalues>

PlainKalmanFilterBallLocator::PlainKalmanFilterBallLocator():
     epsilon(10e-6)
{
    // Debug Drawings
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_real_ball_percept",    "draw the real incomming ball percept",                              true);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_ball_on_field",        "draw the modelled ball on the field after prediction and update",  false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_ball_on_field_before", "draw the modelled ball on the field before prediction and update",  true);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_assignment",           "draws the assignment of the ball percept to the filter",            true);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_final_ball",           "draws the final i.e. best model",                                  false);

    // Plotting Related Debug Requests
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:plot_prediction_error",     "plots the prediction errors in x (horizontal angle) and y (vertical angle)", false);

    // Update Association Function Debug Requests
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:UpdateAssociationFunction:useEuclid",            "minimize Euclidian distance in measurement space",                                false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:UpdateAssociationFunction:useMahalanobis",       "minimize Mahalanobis distance in measurement space (no common covarince matrix)", false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:UpdateAssociationFunction:useMaximumLikelihood", "maximize likelihood of measurement in measurement space ",                        false);

    // Parameter Related Debug Requests
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:reloadParameters",          "reloads the kalman filter parameters from the kfParameter object",  true);

    h.ball_height = 32.5;

    updateAssociationFunction = &euclid;

    getDebugParameterList().add(&kfParameters);

    reloadParameters();
}

PlainKalmanFilterBallLocator::~PlainKalmanFilterBallLocator()
{
    getDebugParameterList().remove(&kfParameters);
}

/*--- !!! sometimes nan filters !!! ---*/
void PlainKalmanFilterBallLocator::execute()
{
    // apply odometry on the filter state, to keep it in the robot's local coordinate system
    for(std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin(); iter != filter.end(); ++iter) {
        applyOdometryOnFilterState(*iter);
    }

    doDebugRequestBeforPredictionAndUpdate();

    // prediction
    double dt = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();

    for(std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin(); iter != filter.end(); iter++){
      predict(*iter, dt);
    }

    // measurement
    if(getBallPercept().ballWasSeen || getBallPerceptTop().ballWasSeen) {

        Eigen::Vector2d z;
        Vector2d p;

        // allways use the bottom percept if available
        if(getBallPercept().ballWasSeen)
        {
            Vector2d angles = CameraGeometry::pixelToAngles(getCameraInfo(),getBallPercept().centerInImage.x,getBallPercept().centerInImage.y);

            z << angles.x, angles.y;

            PLOT("PlainKalmanFilterBallLocator:Measurement:Bottom:horizontal", z(0));
            PLOT("PlainKalmanFilterBallLocator:Measurement:Bottom:vertical",   z(1));

            // set camera bottom in functional
            h.camMat  = getCameraMatrix();
            h.camInfo = getCameraInfo();

            // needed if a new filter has to be created
            p = getBallPercept().bearingBasedOffsetOnField;
        }
        else 
        {
            Vector2d angles = CameraGeometry::pixelToAngles(getCameraInfoTop(),getBallPerceptTop().centerInImage.x,getBallPerceptTop().centerInImage.y);

            z << angles.x, angles.y;

            PLOT("PlainKalmanFilterBallLocator:Measurement:Top:horizontal", z(0));
            PLOT("PlainKalmanFilterBallLocator:Measurement:Top:vertical",   z(1));

            // set camera top in functional
            h.camMat  = getCameraMatrixTop();
            h.camInfo = getCameraInfoTop();

            // needed if a new filter has to be created
            p = getBallPerceptTop().bearingBasedOffsetOnField;
        }

        // find best matching filter
        updateAssociationFunction->determineBestPredictor(filter, z, h);

        std::vector<ExtendedKalmanFilter4d>::iterator bestPredictor = updateAssociationFunction->getBestPredictor();

        // if no suitable filter found create a new one
        if(!updateAssociationFunction->inRange() || bestPredictor == filter.end())
        {
            Eigen::Vector4d newState;
            newState << p.x, 0, p.y, 0;
            filter.push_back(ExtendedKalmanFilter4d(newState, processNoiseStdSingleDimension, measurementNoiseCovariances, initialStateStdSingleDimension));
        }
        else
        {
            DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_assignment",
                FIELD_DRAWING_CONTEXT;
                PEN("FF0000", 10);
                const Eigen::Vector4d state = (*bestPredictor).getState();
                LINE(p.x,p.y,state(0),state(2));
                TEXT_DRAWING((p.x+state(0))/2,(p.y+state(2))/2,updateAssociationFunction->getScore());
            );

            // debug stuff -> should be in a DEBUG_REQUEST
            DEBUG_REQUEST("PlainKalmanFilterBallLocator:plot_prediction_error",
                Eigen::Vector2d prediction_error;
                prediction_error = z - (*bestPredictor).getStateInMeasurementSpace(h);

                PLOT("PlainKalmanFilterBallLocator:Innovation:x", prediction_error(0));
                PLOT("PlainKalmanFilterBallLocator:Innovation:y", prediction_error(1));
            );

            (*bestPredictor).update(z,h);
        }
    }// end if


    // delete some filter if they are to bad
    if(filter.size() > 1) {
        std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin();
        while(iter != filter.end()){
            if((*iter).getEllipseLocation().major * (*iter).getEllipseLocation().minor * M_PI > area95Threshold){
                iter = filter.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    // find the best model for the ball
    if(!filter.empty())
    {
        // find best model
        bestModel = filter.begin();
        double evalue = (*bestModel).getEllipseLocation().major * (*bestModel).getEllipseLocation().minor *M_PI;

        for(std::vector<ExtendedKalmanFilter4d>::const_iterator iter = ++filter.begin(); iter != filter.end(); ++iter){
            double temp = (*iter).getEllipseLocation().major * (*iter).getEllipseLocation().minor * M_PI;
            if(temp < evalue) {
                evalue = temp;
                bestModel = iter;
            }
        }

        const Eigen::Vector4d& x = (*bestModel).getState();

        // set ball model representation
        getBallModel().position.x = x(0);
        getBallModel().position.y = x(2);
        getBallModel().speed.x = x(1);
        getBallModel().speed.y = x(3);

        // set preview ball model representation
        const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
        const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;

        Pose2D lFootPose(lFoot.rotation.getZAngle(), lFoot.translation.x, lFoot.translation.y);
        Pose2D rFootPose(rFoot.rotation.getZAngle(), rFoot.translation.x, rFoot.translation.y);

        Vector2d ballLeftFoot  = lFootPose/getBallModel().position;
        Vector2d ballRightFoot = rFootPose/getBallModel().position;

        getBallModel().positionPreview = getMotionStatus().plannedMotion.hip / getBallModel().position;
        getBallModel().positionPreviewInLFoot = getMotionStatus().plannedMotion.lFoot / ballLeftFoot;
        getBallModel().positionPreviewInRFoot = getMotionStatus().plannedMotion.rFoot / ballRightFoot;

        if(getBallPercept().ballWasSeen || getBallPerceptTop().ballWasSeen)
        {
            getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());
        }

        if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) > 10000.0
           || (*bestModel).getEllipseLocation().major * (*bestModel).getEllipseLocation().minor * M_PI > area95Threshold) // 10s
        {
            // model is not valid enymore after 10s or if the best model does not match the threshold
            getBallModel().valid = false;
        }
        else
        {
            getBallModel().valid = true;
        }
    } else {
        getBallModel().valid = false;
    }

    doDebugRequest();

    lastFrameInfo     = getFrameInfo();
    lastRobotOdometry = getOdometryData();
}

void PlainKalmanFilterBallLocator::predict(ExtendedKalmanFilter4d& filter, double dt) const
{
    /*
        rolling resistance = rolling resitance coefficient * normal force
        F_R = d / R * F_N

        F_N = g * weight
        g = 9.81
        weight = ballMass

        deceleration = F_R/ballMass
    */

    const Eigen::Vector4d& x = filter.getState();
    Eigen::Vector2d u; // control vector

    double deceleration = c_RR*Math::g*1e3;//[mm/s^2]

    // deceleration has to be in opposite direction of velocity
    u <<  -x(1), -x(3);

    // deceleration vector with "absoulte deceleration" (length of vector) of deceleration
    if(u.norm() > 0){
        u.normalize();
    }
    u *= deceleration;

    double time_until_vel_x_zero = 0;
    double time_until_vel_y_zero = 0;
    
    if(fabs(u(0)) > epsilon){
        time_until_vel_x_zero = -x(1)/u(0);
    }
    if(fabs(u(1)) > epsilon){
        time_until_vel_y_zero = -x(3)/u(1);
    }

    if(time_until_vel_x_zero > dt && time_until_vel_y_zero > dt)
    {
        filter.predict(u,dt);
        return;
    }

    if(time_until_vel_x_zero < epsilon && time_until_vel_y_zero < epsilon)
    {
        filter.predict(Eigen::Vector2d::Zero(),dt);
        return;
    }

    if(time_until_vel_x_zero < time_until_vel_y_zero)
    {
        filter.predict(u,time_until_vel_x_zero);
        u(0) = 0;
        dt -= time_until_vel_x_zero;

        if(time_until_vel_y_zero < dt)
        {
            double dt2 = time_until_vel_y_zero - time_until_vel_x_zero;
            filter.predict(u, dt2);
            u(1) = 0;
            dt -= dt2;
        }

        filter.predict(u,dt);
    } else {
        filter.predict(u,time_until_vel_y_zero);
        u(1) = 0;
        dt -= time_until_vel_y_zero;

        if(time_until_vel_x_zero < dt)
        {
            double dt2 = time_until_vel_x_zero - time_until_vel_y_zero;
            filter.predict(u, dt2);
            u(0) = 0;
            dt -= dt2;
        }

        filter.predict(u,dt);
    }
}

void PlainKalmanFilterBallLocator::applyOdometryOnFilterState(ExtendedKalmanFilter4d& filter)
{
    const Eigen::Vector4d& x = filter.getState();
    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();

    Eigen::Vector4d newStateX;
    newStateX << x;

    // construct rotation matrix
    double s = sin(odometryDelta.getAngle());
    double c = cos(odometryDelta.getAngle());

    Eigen::Matrix4d rotation;
    rotation << c, 0, -s,  0,
                0, c,  0, -s,
                s, 0,  c,  0,
                0, s,  0,  c;

    newStateX = rotation*newStateX;

    // translate the location of the filters state (velocity is relative so translating it doesn't make sense)
    newStateX(0) = newStateX(0) + odometryDelta.translation.x;
    newStateX(2) = newStateX(2) + odometryDelta.translation.y;

    // rotate P (translation doesn't affect the covariances)
    Eigen::Matrix4d P = filter.getProcessCovariance();

    Eigen::Matrix4d new_P = rotation * P * rotation.transpose();

    filter.setState(newStateX);
    filter.setCovarianceOfState(new_P);
}

void PlainKalmanFilterBallLocator::doDebugRequest()
{
    if(getBallPercept().ballWasSeen || getBallPerceptTop().ballWasSeen) {
        //to check correctness of the prediction
        DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_real_ball_percept",
          FIELD_DRAWING_CONTEXT;
          PEN("FF0000", 10);
          if(getBallPercept().ballWasSeen) {
              CIRCLE(getBallPercept().bearingBasedOffsetOnField.x, getBallPercept().bearingBasedOffsetOnField.y, getFieldInfo().ballRadius-5);
          } else {
              CIRCLE(getBallPerceptTop().bearingBasedOffsetOnField.x, getBallPerceptTop().bearingBasedOffsetOnField.y, getFieldInfo().ballRadius-5);
          }
        );
    }

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_final_ball",
        FIELD_DRAWING_CONTEXT;
        PEN("FF9900", 10);
        CIRCLE( getBallModel().position.x, getBallModel().position.y, getFieldInfo().ballRadius-10);
    );

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_ball_on_field",
        drawFiltersOnField();
    );

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:reloadParameters",
        reloadParameters();
    );
}

void PlainKalmanFilterBallLocator::doDebugRequestBeforPredictionAndUpdate()
{
    DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_ball_on_field_before",
        drawFiltersOnField();
    );

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:UpdateAssociationFunction:useEuclid",
        updateAssociationFunction = &euclid;
    );

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:UpdateAssociationFunction:useMahalanobis",
        updateAssociationFunction = &mahalanobis;
    );

    DEBUG_REQUEST("PlainKalmanFilterBallLocator:UpdateAssociationFunction:useMaximumLikelihood",
        updateAssociationFunction = &likelihood;
    );
}

void PlainKalmanFilterBallLocator::drawFiltersOnField() const {
    FIELD_DRAWING_CONTEXT;

    for(std::vector<ExtendedKalmanFilter4d>::const_iterator iter = filter.begin(); iter != filter.end(); iter++)
    {
        if(getBallModel().valid)
        {
            if(bestModel == iter)
            {
                if(getBallPercept().ballWasSeen || getBallPerceptTop().ballWasSeen)
                    PEN("FF9900", 20);
                else
                    PEN("0099FF", 20);
            } else {
                PEN("999999", 20);
            }
        }
        else
            PEN("999999", 20);

        const Eigen::Vector4d& state = (*iter).getState();

        CIRCLE( state(0), state(2), getFieldInfo().ballRadius-10);
        ARROW( state(0), state(2),
               state(0)+state(1),
               state(2)+state(3));

        PEN("00FFFF", 20);

        // draw error ellipses for the location
        const Ellipse2d& ellipse_loc = (*iter).getEllipseLocation();
        OVAL_ROTATED(state(0),
                     state(2),
                     ellipse_loc.minor,
                     ellipse_loc.major,
                     ellipse_loc.angle);

        PEN("FF00FF", 20);

        // draw error ellipse for the velocity
        const Ellipse2d& ellipse_vel = (*iter).getEllipseVelocity();
        OVAL_ROTATED(state(0)+state(1),
                     state(2)+state(3),
                     ellipse_vel.minor,
                     ellipse_vel.major,
                     ellipse_vel.angle);
    }
}

void PlainKalmanFilterBallLocator::reloadParameters()
{
    // parameters for initializing new filters
    processNoiseStdSingleDimension << kfParameters.processNoiseStdQ00, kfParameters.processNoiseStdQ01,
                                      kfParameters.processNoiseStdQ10, kfParameters.processNoiseStdQ11;

    measurementNoiseCovariances << kfParameters.measurementNoiseR00, kfParameters.measurementNoiseR10,
                                   kfParameters.measurementNoiseR10, kfParameters.measurementNoiseR11;

    initialStateStdSingleDimension << kfParameters.initialStateStdP00, kfParameters.initialStateStdP01,
                                      kfParameters.initialStateStdP10, kfParameters.initialStateStdP11;

    // filter unspecific parameters
    c_RR              = kfParameters.c_RR;
    area95Threshold   = kfParameters.area95Threshold;

    // UAF thresholds
    euclid.setThreshold(kfParameters.euclidThreshold);
    mahalanobis.setThreshold(kfParameters.mahalanobisThreshold);
    likelihood.setThreshold(kfParameters.maximumLikelihoodThreshold);

    // update existing filters with new process and measurement noise
    Eigen::Matrix2d processNoiseCovariancesSingleDimension;
    processNoiseCovariancesSingleDimension = processNoiseStdSingleDimension.cwiseProduct(processNoiseStdSingleDimension);

    for(std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin(); iter != filter.end(); iter++){
        (*iter).setCovarianceOfProcessNoise(processNoiseCovariancesSingleDimension);
        (*iter).setCovarianceOfMeasurementNoise(measurementNoiseCovariances);
    }
}
