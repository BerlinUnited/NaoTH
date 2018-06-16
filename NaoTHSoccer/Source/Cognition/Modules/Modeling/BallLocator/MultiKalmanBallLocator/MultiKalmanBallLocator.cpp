#include "MultiKalmanBallLocator.h"

#include <Tools/naoth_eigen.h>
#include "Tools/Association.h"

MultiKalmanBallLocator::MultiKalmanBallLocator():
     epsilon(10e-6)
{
    // Modify number of models
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:remove_all_models",     "remove all models",                                                             false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:allow_just_one_model",  "allows only one model to be generated (all updates are applied to that model)", false);

    // Debug Drawings
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_real_ball_percept",    "draw the real incomming ball percept",                             false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_ball_on_field_before", "draw the modelled ball on the field before prediction and update", false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_ball_on_field",        "draw the modelled ball on the field before update",                false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_ball_on_field_after",  "draw the modelled ball on the field after prediction and update",  false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_assignment",           "draws the assignment of the ball percept to the filter",           false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_final_ball",           "draws the final i.e. best model",                                  false);

    // Plotting Related Debug Requests
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:plot_prediction_error",     "plots the prediction errors in x (horizontal angle) and y (vertical angle)", false);

    // Update Association Function Debug Requests
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:UpdateAssociationFunction:useEuclid",            "minimize Euclidian distance in measurement space",                                false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:UpdateAssociationFunction:useMahalanobis",       "minimize Mahalanobis distance in measurement space (no common covarince matrix)", false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:UpdateAssociationFunction:useMaximumLikelihood", "maximize likelihood of measurement in measurement space ",                         true);

    // Parameter Related Debug Requests
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:reloadParameters",          "reloads the kalman filter parameters from the kfParameter object", false);

    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_trust_the_ball", "..", false);

    h.ballRadius = getFieldInfo().ballRadius;

    updateAssociationFunction = &likelihood;

    getDebugParameterList().add(&kfParameters);

    reloadParameters();
}

MultiKalmanBallLocator::~MultiKalmanBallLocator()
{
    getDebugParameterList().remove(&kfParameters);
}

void MultiKalmanBallLocator::execute()
{
  // allways reset the model first
  getBallModel().reset();

  // HACK: no updates in ready or when lifted
  if(getPlayerInfo().robotState == PlayerInfo::ready || getBodyState().isLiftedUp) {
    filter.clear();
    return;
  }

    DEBUG_REQUEST("MultiKalmanBallLocator:remove_all_models",
        filter.clear();
    );

    // delete some filter if they are too bad
    if(filter.size() > 1) {
        Filters::iterator iter = filter.begin();
        while(iter != filter.end() && filter.size() > 1) {
          if(!iter->ballSeenFilter.value() && (*iter).getEllipseLocation().major * (*iter).getEllipseLocation().minor * Math::pi > area95Threshold){
                iter = filter.erase(iter);
            } else {
                ++iter;
            }
        }
    }


    // apply odometry on the filter state, to keep it in the robot's local coordinate system
    for(Filters::iterator iter = filter.begin(); iter != filter.end(); ++iter) {
        applyOdometryOnFilterState(*iter);
    }

    doDebugRequestBeforPredictionAndUpdate();

    // prediction
    double dt = getFrameInfo().getTimeInSeconds() - lastFrameInfo.getTimeInSeconds();
    ASSERT(dt > 0);
    for(Filters::iterator iter = filter.begin(); iter != filter.end(); iter++){
      predict(*iter, dt);
    }

    doDebugRequestBeforUpdate();

    // sensor update
    //updateByPerceptsNormal();
    updateByPerceptsCool();

    // Heinrich: update the "ball seen" values
    for(Filters::iterator iter = filter.begin(); iter != filter.end(); ++iter){
      bool updated = iter->getLastUpdateFrame().getFrameNumber() == getFrameInfo().getFrameNumber();
      (*iter).ballSeenFilter.setParameter(kfParameters.g0, kfParameters.g1);
      (*iter).ballSeenFilter.update(updated, 0.3, 0.7);
    }

    // estimate the best model
    bestModel = selectBestModel();

    // fill the ball model representation
    if (bestModel != filter.end()) {
      provideBallModel(*bestModel);
    }
    
    doDebugRequest();

    lastFrameInfo     = getFrameInfo();
    lastRobotOdometry = getOdometryData();
}

void MultiKalmanBallLocator::updateByPerceptsNormal()
{
  // measurement
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {

        Eigen::Vector2d z;
        Vector2d p;

        // set correct camera matrix and info in functional
        if((*iter).cameraId == CameraInfo::Bottom)
        {
            //PLOT("MultiKalmanBallLocator:Measurement:Bottom:horizontal", z(0));
            //PLOT("MultiKalmanBallLocator:Measurement:Bottom:vertical",   z(1));

            h.camMat  = getCameraMatrix();
            h.camInfo = getCameraInfo();
        }
        else 
        {
            //PLOT("MultiKalmanBallLocator:Measurement:Top:horizontal", z(0));
            //PLOT("MultiKalmanBallLocator:Measurement:Top:vertical",   z(1));

            h.camMat  = getCameraMatrixTop();
            h.camInfo = getCameraInfoTop();
        }

        // tansform measurement into angles
        Vector2d angles = CameraGeometry::pixelToAngles(h.camInfo,(*iter).centerInImage.x,(*iter).centerInImage.y);
        z << angles.x, angles.y;

        // needed if a new filter has to be created
        p = (*iter).positionOnField;

        // find best matching filter
        updateAssociationFunction->determineBestPredictor(filter, z, h);

        Filters::iterator bestPredictor = updateAssociationFunction->getBestPredictor();

        bool allowJustOneModel = false;
        DEBUG_REQUEST("MultiKalmanBallLocator:allow_just_one_model",
            allowJustOneModel = true;
        );

        // if no suitable filter found create a new one
        if((!allowJustOneModel && !updateAssociationFunction->inRange()) || bestPredictor == filter.end())
        {
            Eigen::Vector4d newState;
            newState << p.x, 0, p.y, 0;
            filter.push_back(BallHypothesis(getFrameInfo(), newState, processNoiseStdSingleDimension, measurementNoiseCovariances, initialStateStdSingleDimension));
        }
        else
        {
            DEBUG_REQUEST("MultiKalmanBallLocator:draw_assignment",
                FIELD_DRAWING_CONTEXT;
                PEN("FF0000", 10);
                const Eigen::Vector4d state = (*bestPredictor).getState();
                LINE(p.x,p.y,state(0),state(2));
                TEXT_DRAWING((p.x+state(0))/2,(p.y+state(2))/2,updateAssociationFunction->getScore());
            );

            // debug stuff -> should be in a DEBUG_REQUEST
            DEBUG_REQUEST("MultiKalmanBallLocator:plot_prediction_error",
                Eigen::Vector2d prediction_error;
                prediction_error = z - (*bestPredictor).getStateInMeasurementSpace(h);

                PLOT("MultiKalmanBallLocator:Innovation:x", prediction_error(0));
                PLOT("MultiKalmanBallLocator:Innovation:y", prediction_error(1));
            );

            (*bestPredictor).update(z,h,getFrameInfo());
        }
    }// end for
}


void MultiKalmanBallLocator::updateByPerceptsCool()
{
  // update by percepts
  // A ~ goal posts
  // B ~ hypotheses
  Assoziation sensorAssoziation(static_cast<int>(getMultiBallPercept().getPercepts().size()), static_cast<int>(filter.size()));

  int i = 0;
  for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) 
  {
      // set correct camera matrix and info in functional
      if((*iter).cameraId == CameraInfo::Bottom) {
          h.camMat  = getCameraMatrix();
          h.camInfo = getCameraInfo();
      } else {
          h.camMat  = getCameraMatrixTop();
          h.camInfo = getCameraInfoTop();
      }

      // tansform measurement into angles
      Vector2d angles = CameraGeometry::pixelToAngles(h.camInfo,(*iter).centerInImage.x,(*iter).centerInImage.y);
      Eigen::Vector2d z;
      z << angles.x, angles.y;

      int x = 0;
      for(Filters::iterator iter = filter.begin(); iter != filter.end(); ++iter)
      {
        double confidence = updateAssociationFunction->associationScore(*iter, z, h);

        // store best association for measurement
        if(confidence > updateAssociationFunction->getThreshold() &&
          confidence > sensorAssoziation.getW4A(i) && 
          confidence > sensorAssoziation.getW4B(x))
        {
          sensorAssoziation.addAssociation(i, x, confidence);
        }
        x++;
      }
      i++;
  }

  i = 0;
  for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) 
  {
    int x = sensorAssoziation.getB4A(i); // get hypothesis for measurement
    if (x != -1)
    {
      // set correct camera matrix and info in functional
      if((*iter).cameraId == CameraInfo::Bottom) {
          h.camMat  = getCameraMatrix();
          h.camInfo = getCameraInfo();
      } else {
          h.camMat  = getCameraMatrixTop();
          h.camInfo = getCameraInfoTop();
      }

      // tansform measurement into angles
      Vector2d angles = CameraGeometry::pixelToAngles(h.camInfo,(*iter).centerInImage.x,(*iter).centerInImage.y);
      Eigen::Vector2d z;
      z << angles.x, angles.y;

      filter[x].update(z,h,getFrameInfo());
    }
    else
    {
      Vector2d p = (*iter).positionOnField;
      Eigen::Vector4d newState;
      newState << p.x, 0, p.y, 0;
      filter.push_back(BallHypothesis(getFrameInfo(), newState, processNoiseStdSingleDimension, measurementNoiseCovariances, initialStateStdSingleDimension));
    }
    i++;
  }
  
}

void MultiKalmanBallLocator::predict(ExtendedKalmanFilter4d& filter, double dt) const
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

void MultiKalmanBallLocator::applyOdometryOnFilterState(ExtendedKalmanFilter4d& filter)
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

MultiKalmanBallLocator::Filters::const_iterator MultiKalmanBallLocator::selectBestModel() const
{
  Filters::const_iterator bestModel = filter.end();
  double value = 0;

  if(kfParameters.use_covariance_based_selection)
  {
    // find the best seen model for the ball based on covariance
    for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); ++iter) {
        double temp = iter->getEllipseLocation().major * iter->getEllipseLocation().minor * Math::pi;
        if(bestModel == filter.end() || temp < value){
            bestModel = iter;
            value = temp;
        }
    }

    return bestModel;
  }

  // find the best model for the ball: closest hypothesis that is "known"
  for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); ++iter) {
    if(iter->ballSeenFilter.value()) {
      double temp = Vector2d(iter->getState()(0), iter->getState()(2)).abs();
      if(bestModel == filter.end() || temp < value){
          bestModel = iter;
          value = temp;
      }
    }
  }

  return bestModel;
}

void MultiKalmanBallLocator::provideBallModel(const BallHypothesis& model)
{
  getBallModel().valid = true;
  getBallModel().knows = model.ballSeenFilter.value();

  const Eigen::Vector4d& x = model.getState();

  // set ball model representation
  getBallModel().position.x = x(0);
  getBallModel().position.y = x(2);
  getBallModel().speed.x = x(1);
  getBallModel().speed.y = x(3);

  //set preview ball model representation

  const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;

  // transform ball model into feet coordinates
  Vector2d ballLeftFoot  = lFoot.projectXY()/getBallModel().position;
  Vector2d ballRightFoot = rFoot.projectXY()/getBallModel().position;

  getBallModel().positionPreview = getMotionStatus().plannedMotion.hip / getBallModel().position;
  getBallModel().positionPreviewInLFoot = getMotionStatus().plannedMotion.lFoot / ballLeftFoot;
  getBallModel().positionPreviewInRFoot = getMotionStatus().plannedMotion.rFoot / ballRightFoot;

  getBallModel().setFrameInfoWhenBallWasSeen(model.getLastUpdateFrame());

  // predict future ball positions
  const int BALLMODEL_MAX_FUTURE_SECONDS = 11;
  getBallModel().futurePosition.resize(BALLMODEL_MAX_FUTURE_SECONDS);

  getBallModel().futurePosition[0] = getBallModel().position;
  
  BallHypothesis modelCopy(model);
  for(size_t i=1; i < getBallModel().futurePosition.size(); i++)
  {
    predict(modelCopy, 1.0); // predict 1s in the future

    const Eigen::Vector4d& x = modelCopy.getState();
    getBallModel().futurePosition[i] = Vector2d(x(0), x(2));
  }
}

void MultiKalmanBallLocator::doDebugRequestBeforPredictionAndUpdate()
{
    DEBUG_REQUEST("MultiKalmanBallLocator:draw_ball_on_field_before",
        drawFiltersOnField();
    );

    DEBUG_REQUEST("MultiKalmanBallLocator:UpdateAssociationFunction:useEuclid",
        updateAssociationFunction = &euclid;
    );

    DEBUG_REQUEST("MultiKalmanBallLocator:UpdateAssociationFunction:useMahalanobis",
        updateAssociationFunction = &mahalanobis;
    );

    DEBUG_REQUEST("MultiKalmanBallLocator:UpdateAssociationFunction:useMaximumLikelihood",
        updateAssociationFunction = &likelihood;
    );
}

void MultiKalmanBallLocator::doDebugRequestBeforUpdate()
{
    DEBUG_REQUEST("MultiKalmanBallLocator:draw_ball_on_field",
        drawFiltersOnField();
    );
}

void MultiKalmanBallLocator::doDebugRequest()
{
    //PLOT("MultiKalmanBallLocator:ModelIsValid", getBallModel().valid);

    
    //to check correctness of the prediction
    DEBUG_REQUEST("MultiKalmanBallLocator:draw_real_ball_percept",
      if(getMultiBallPercept().wasSeen()) {
        FIELD_DRAWING_CONTEXT;
        PEN("FF0000", 10);
        for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
            CIRCLE((*iter).positionOnField.x, (*iter).positionOnField.y, getFieldInfo().ballRadius-5);
        }
      }
    );
    
    DEBUG_REQUEST("MultiKalmanBallLocator:draw_ball_on_field_after",
        drawFiltersOnField();
    );

    DEBUG_REQUEST("MultiKalmanBallLocator:draw_final_ball",
        FIELD_DRAWING_CONTEXT;
        PEN("FF0000", 10);
        CIRCLE( getBallModel().position.x, getBallModel().position.y, getFieldInfo().ballRadius-10);
    );

    DEBUG_REQUEST("MultiKalmanBallLocator:reloadParameters",
        reloadParameters();
    );

    DEBUG_REQUEST("MultiKalmanBallLocator:draw_trust_the_ball",
        FIELD_DRAWING_CONTEXT;
        for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); iter++)
        {
          if((*iter).ballSeenFilter.value()) {
            PEN("00FF00", 10);
          } else {
            PEN("FF0000", 10);
          }
      
          const Eigen::Vector4d& state = (*iter).getState();
          CIRCLE( state(0), state(2), (*iter).ballSeenFilter.value()*1000);
        }
      );
}

void MultiKalmanBallLocator::drawFiltersOnField() const {
    FIELD_DRAWING_CONTEXT;

    for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); iter++)
    {
        if(getBallModel().valid)
        {
            if((*iter).getLastUpdateFrame().getTime() == getFrameInfo().getTime()) {
                if(bestModel == iter)
                    PEN("99FF00", 20);
                else
                    PEN("FF9900",20);
            } else {
                    PEN("0099FF", 20);
            }
        } else {
                PEN("999999", 20);
        }

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

void MultiKalmanBallLocator::reloadParameters()
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

    for(Filters::iterator iter = filter.begin(); iter != filter.end(); iter++){
        (*iter).setCovarianceOfProcessNoise(processNoiseCovariancesSingleDimension);
        (*iter).setCovarianceOfMeasurementNoise(measurementNoiseCovariances);
    }
}
