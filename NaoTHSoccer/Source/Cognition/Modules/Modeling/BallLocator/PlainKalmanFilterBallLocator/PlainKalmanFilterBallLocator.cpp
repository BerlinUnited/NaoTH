#include "PlainKalmanFilterBallLocator.h"

#include <cmath>
#include <Eigen/Eigenvalues>

PlainKalmanFilterBallLocator::PlainKalmanFilterBallLocator():
     epsilon(10e-6)
{
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_ball_on_field",        "draw the modelled ball on the field",                              false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_ball_on_field_before", "draw the modelled ball on the field before prediction and update",  true);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_real_ball_percept",    "draw the real incomming ball percept",                              true);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_final_ball",           "draws the final i.e. best model",                                  false);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:draw_assignment",           "draws the assignment of the ball percept to the filter",            true);
    DEBUG_REQUEST_REGISTER("PlainKalmanFilterBallLocator:reloadParameters",          "reloads the kalman filter parameters from the kfParameter object",  true);

    h.ball_height = 32.5;

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
//     apply odometry on the filter state, to keep it in the robot's local coordinate system
    for(std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin(); iter != filter.end(); iter++){
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
        double x,y;

        if(!getBallPercept().ballWasSeen && getBallPerceptTop().ballWasSeen) {

            Vector2d angles = CameraGeometry::angleToPointInImage(getCameraMatrixTop(),getCameraInfoTop(),getBallPerceptTop().centerInImage.x,getBallPerceptTop().centerInImage.y);

            z << angles.x, angles.y;

            PLOT("PlainKalmanFilterBallLocator:Measurement:Top:horizontal", z(0));
            PLOT("PlainKalmanFilterBallLocator:Measurement:Top:vertical",   z(1));

            // set camera top in functional
            h.camMat  = getCameraMatrixTop();
            h.camInfo = getCameraInfoTop();

            // needed if a new filter has to be created
            x = getBallPerceptTop().bearingBasedOffsetOnField.x;
            y = getBallPerceptTop().bearingBasedOffsetOnField.y;

        } else {

            Vector2d angles = CameraGeometry::angleToPointInImage(getCameraMatrix(),getCameraInfo(),getBallPercept().centerInImage.x,getBallPercept().centerInImage.y);

            z << angles.x, angles.y;

            PLOT("PlainKalmanFilterBallLocator:Measurement:Bottom:horizontal", z(0));
            PLOT("PlainKalmanFilterBallLocator:Measurement:Bottom:vertical",   z(1));

            // set camera bottom in functional
            h.camMat  = getCameraMatrix();
            h.camInfo = getCameraInfo();

            // needed if a new filter has to be created
            x = getBallPercept().bearingBasedOffsetOnField.x;
            y = getBallPercept().bearingBasedOffsetOnField.y;

        }

        if(filter.size() == 0) {
            Eigen::Vector4d newState;
            newState << x, 0, y, 0;
            filter.push_back(ExtendedKalmanFilter4d(newState, processNoiseStdSingleDimension, measurementNoiseStd, initialStateStdSingleDimension));
        }
        else
        {
            // find best matching filter
            double dist;

            dist = distanceToState(filter[0],z);

            dist = isnan(dist) ? std::numeric_limits<double>::infinity() : dist;

            std::vector<ExtendedKalmanFilter4d>::iterator bestPredictor = filter.begin();

            for(std::vector<ExtendedKalmanFilter4d>::iterator iter = bestPredictor; iter != filter.end(); iter++){
                double temp;
                temp = distanceToState(*iter,z);

                temp = isnan(temp) ? std::numeric_limits<double>::infinity() : temp;

                if(temp < dist){
                    dist = temp;
                    bestPredictor = iter;
                }
            }

            if(dist > distanceThreshold)
            {
                Eigen::Vector4d newState;
                newState << x, 0, y, 0;
                filter.push_back(ExtendedKalmanFilter4d(newState, processNoiseStdSingleDimension, measurementNoiseStd, initialStateStdSingleDimension));
            }
            else
            {
                DEBUG_REQUEST("PlainKalmanFilterBallLocator:draw_assignment",
                    FIELD_DRAWING_CONTEXT;
                    PEN("FF0000", 10);
                    const Eigen::Vector4d state = (*bestPredictor).getState();
                    LINE(x,y,state(0),state(2));
                    TEXT_DRAWING((x+state(0))/2,(y+state(2))/2,dist);
                );

                // debug stuff -> should be in a DEBUG_REQUEST
                Eigen::Vector2d predicted_measurement;
                predicted_measurement = (*bestPredictor).getStateInMeasurementSpace(h);

                PLOT("PlainKalmanFilterBallLocator:Innovation:x", z(0)-predicted_measurement(0));
                PLOT("PlainKalmanFilterBallLocator:Innovation:y", z(1)-predicted_measurement(1));

                (*bestPredictor).update(z,h);
            }
        }
    }

    // delete some filter if they are to bad
    if(filter.size() != 1) {
        std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin();
        while(iter != filter.end()){
            if((*iter).getEllipse().major * (*iter).getEllipse().minor * M_PI > area95Threshold){
                filter.erase(iter);
            } else {
                iter++;
            }
        }
    }

    if(filter.size() > 0)
    {
        // find best model
        std::vector<ExtendedKalmanFilter4d>::iterator model = filter.begin();
        double evalue = (*model).getEllipse().major * (*model).getEllipse().minor *M_PI;

        for(std::vector<ExtendedKalmanFilter4d>::iterator iter = model; iter != filter.end(); iter++){
            double temp = (*iter).getEllipse().major * (*iter).getEllipse().minor *M_PI;;
            if(temp < evalue){
                evalue = temp;
                model = iter;
            }
        }

        bestModel = &(*model);
        const Eigen::Vector4d& x = (*model).getState();

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
           || (*model).getEllipse().major * (*model).getEllipse().minor * M_PI > area95Threshold) // 10s
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

double PlainKalmanFilterBallLocator::distanceToState(const ExtendedKalmanFilter4d& filter, const Eigen::Vector2d& z) const
{
    return std::sqrt(((z-filter.getStateInMeasurementSpace(h)).transpose() * Eigen::Matrix2d::Identity() * (z-filter.getStateInMeasurementSpace(h)))(0,0));
}

void PlainKalmanFilterBallLocator::predict(ExtendedKalmanFilter4d& filter, double dt)
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

    double deceleration = c_RR*9810;//[mm/s^2]

    // deceleration has to be in opposite direction of velocity
    u <<  -x(1), -x(3);

    // deceleration vector with "absoult deceleration" (length of vector) of deceleration
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
        filter.prediction(u,dt);
        return;
    }

    if(time_until_vel_x_zero < epsilon && time_until_vel_y_zero < epsilon)
    {
        filter.prediction(Eigen::Vector2d::Zero(),dt);
        return;
    }

    if(time_until_vel_x_zero < time_until_vel_y_zero)
    {
        filter.prediction(u,time_until_vel_x_zero);
        u(0) = 0;
        dt -= time_until_vel_x_zero;

        if(time_until_vel_y_zero < dt)
        {
            double dt2 = time_until_vel_y_zero - time_until_vel_x_zero;
            filter.prediction(u, dt2);
            u(1) = 0;
            dt -= dt2;
        }

        filter.prediction(u,dt);
    } else {
        filter.prediction(u,time_until_vel_y_zero);
        u(1) = 0;
        dt -= time_until_vel_y_zero;

        if(time_until_vel_x_zero < dt)
        {
            double dt2 = time_until_vel_x_zero - time_until_vel_y_zero;
            filter.prediction(u, dt2);
            u(0) = 0;
            dt -= dt2;
        }

        filter.prediction(u,dt);
    }
}

void PlainKalmanFilterBallLocator::applyOdometryOnFilterState(ExtendedKalmanFilter4d& filter)
{
    const Eigen::Vector4d& x = filter.getState();
    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();

    //rotate and translate location part of the filter's state
    Vector2d location = Vector2d(x(0), x(2)); // translation of the model
    location = odometryDelta * location;

    //just rotate the velocity part of the filter's state
    Vector2d velocity = Vector2d(x(1), x(3)); // translation of the model
    velocity.rotate(odometryDelta.getAngle());

    Eigen::Vector4d newStateX;
    newStateX << location.x, velocity.x, location.y, velocity.y;

    // rotate P?

    filter.setState(newStateX);
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
}

void PlainKalmanFilterBallLocator::drawFiltersOnField() {
    FIELD_DRAWING_CONTEXT;

    for(std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin(); iter != filter.end(); iter++)
    {
        if(getBallModel().valid)
        {
            if(bestModel == &(*iter))
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

        const Ellipse2d& ellipse = (*iter).getEllipse();

        CIRCLE( state(0), state(2), getFieldInfo().ballRadius-10);
        ARROW( state(0), state(2),
               state(0)+state(1),
               state(2)+state(3));

        PEN("00FFFF", 20);

        OVAL_ROTATED(state(0),
                     state(2),
                     ellipse.minor,
                     ellipse.major,
                     ellipse.angle);
    }
}

void PlainKalmanFilterBallLocator::reloadParameters()
{
    // parameters for initializing new filters
    processNoiseStdSingleDimension << kfParameters.processNoiseStdQ00, kfParameters.processNoiseStdQ01,
                                      kfParameters.processNoiseStdQ10, kfParameters.processNoiseStdQ11;

    measurementNoiseStd << kfParameters.measurementNoiseStdR00, 0,
                           0, kfParameters.measurementNoiseStdR11;

    initialStateStdSingleDimension << kfParameters.initialStateStdP00, kfParameters.initialStateStdP01,
                                      kfParameters.initialStateStdP10, kfParameters.initialStateStdP11;

    // filter unspecific parameters
    c_RR              = kfParameters.c_RR;
    distanceThreshold = kfParameters.distanceThreshold;
    area95Threshold      = kfParameters.area95Threshold;

    // update existing filters with new process and measurement noise
    Eigen::Matrix2d processNoiseCovariancesSingleDimension;
    processNoiseCovariancesSingleDimension = processNoiseStdSingleDimension.cwiseProduct(processNoiseStdSingleDimension);

    Eigen::Matrix2d measurementNoiseCovariances = Eigen::Matrix2d::Zero();
    measurementNoiseCovariances = measurementNoiseStd.cwiseProduct(measurementNoiseStd);

    for(std::vector<ExtendedKalmanFilter4d>::iterator iter = filter.begin(); iter != filter.end(); iter++){
        (*iter).setCovarianceOfProcessNoise(processNoiseCovariancesSingleDimension);
        (*iter).setCovarianceOfMeasurementNoise(measurementNoiseCovariances);
    }
}
