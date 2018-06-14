#include "MultiKalmanBallLocator.h"

#include <Tools/naoth_eigen.h>
#include "Tools/Association.h"
#include <Tools/ColorClasses.h>

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

    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:negative_feedback_by_back_projecting_models", "", true);

    // Parameter Related Debug Requests
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:reloadParameters",          "reloads the kalman filter parameters from the kfParameter object", false);

    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_trust_the_ball", "..", false);

    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:provide_model_based_on_covariance", "..", true);

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
          double distance = Vector2d(iter->getState()(0), iter->getState()(2)).abs();
          double threshold_radius = kfParameters.area95Threshold_radius.factor * distance + kfParameters.area95Threshold_radius.offset;
          if(!iter->ballSeenFilter.value() && (*iter).getEllipseLocation().major * (*iter).getEllipseLocation().minor * Math::pi >  threshold_radius*threshold_radius*2*Math::pi){
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
    //updateByPerceptsCool();

    // need to handle bottom and top percepts independently because both cameras can observe the same ball
    updateByPerceptsNaive(CameraInfo::Bottom);
    updateByPerceptsNaive(CameraInfo::Top);

    // Heinrich: update the "ball seen" values
    for(Filters::iterator iter = filter.begin(); iter != filter.end(); ++iter){
      bool updated = iter->getLastUpdateFrame().getFrameNumber() == getFrameInfo().getFrameNumber();
      (*iter).ballSeenFilter.setParameter(kfParameters.g0, kfParameters.g1);
      (*iter).ballSeenFilter.update(updated, 0.3, 0.7);
    }

    for(BallHypothesis& bs: filter){
        if(bs.getLastUpdateFrame().getFrameNumber() == getFrameInfo().getFrameNumber()) {
            bs.stall_count = 0;
        }
    }

    DEBUG_REQUEST("MultiKalmanBallLocator:negative_feedback_by_back_projecting_models",
        // negative feedback... double check not updated models
        negativeUpdate();

        std::vector<BallHypothesis>::iterator iter = filter.begin();
        while(iter != filter.end()){
            if(iter->stall_count >= 3) {
                iter = filter.erase(iter);
            } else {
                ++iter;
            }
        }
    );

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

void MultiKalmanBallLocator::updateByPerceptsNaive(CameraInfo::CameraID camera)
{
  // set correct camera matrix and info in functional
  if(camera == CameraInfo::Bottom)
  {
      h.camMat  = getCameraMatrix();
      h.camInfo = getCameraInfo();
  }
  else
  {
      h.camMat  = getCameraMatrixTop();
      h.camInfo = getCameraInfoTop();
  }

  // phase 1: filter percepts and create index vector for filters
  std::vector<Eigen::Vector2d> zs;
  std::vector<Vector2d> ps;

  for(const MultiBallPercept::BallPercept& percept : getMultiBallPercept().getPercepts()){
      Eigen::Vector2d z;

      if(percept.cameraId == camera){
        // tansform measurement into angles
        Vector2d angles = CameraGeometry::pixelToAngles(h.camInfo,percept.centerInImage.x,percept.centerInImage.y);
        z << angles.x, angles.y;
        zs.push_back(z);

        // needed if a new filter has to be created
        ps.push_back(percept.positionOnField);
      }
  }

  std::vector<int> f;
  for(size_t i = 0; i < filter.size(); ++i){
     f.push_back((int)i);
  }

  if (!(filter.empty() || zs.empty()))
  {
    // phase 2: calculate all scores
    // row index    = measurement
    // column index = filter
    Eigen::MatrixXd scores = Eigen::MatrixXd::Zero(zs.size(), filter.size());
    for(size_t i = 0; i < zs.size(); ++i){
        for (size_t j = 0; j < filter.size(); ++j){
           scores(i,j) = updateAssociationFunction->associationScore(filter[j], zs[i], h);
        }
    }

    // phase 3: reduce score matrix iteratively by eliminating row and column of best match
    do {
      // phase 3.1: find location of best entry which is according to the updateAssociationFunction a global maximum or minimum
      Eigen::Index maxCol, minCol, maxRow, minRow, bestCol, bestRow;
      double max = scores.maxCoeff(&maxRow, &maxCol);
      double min = scores.minCoeff(&minRow, &minCol);

      if(updateAssociationFunction->better(min,max)){
          bestCol = minCol;
          bestRow = minRow;
      } else {
          bestCol = maxCol;
          bestRow = maxRow;
      }

      // phase 3.2: update filter
      bool allowJustOneModel = false;
      DEBUG_REQUEST("MultiKalmanBallLocator:allow_just_one_model",
          allowJustOneModel = true;
      );

      // if no suitable filter found create a new one
      if(!allowJustOneModel && !updateAssociationFunction->inRange(scores(bestRow,bestCol)))
      {
          // best percept was not good enough to be validly associated to filter so no other will be
          break;
      }
      else
      {
          DEBUG_REQUEST("MultiKalmanBallLocator:draw_assignment",
              FIELD_DRAWING_CONTEXT;
              PEN("FF0000", 10);
              const Eigen::Vector4d state = filter[f[bestCol]].getState();
              LINE(ps[bestRow].x,ps[bestRow].y,state(0),state(2));
              TEXT_DRAWING((ps[bestRow].x+state(0))/2,(ps[bestRow].y+state(2))/2, scores(bestRow,bestCol));
          );

          DEBUG_REQUEST("MultiKalmanBallLocator:plot_prediction_error",
              Eigen::Vector2d prediction_error;
              prediction_error = zs[bestRow] - filter[f[bestCol]].getStateInMeasurementSpace(h);

              PLOT("MultiKalmanBallLocator:Innovation:x", prediction_error(0));
              PLOT("MultiKalmanBallLocator:Innovation:y", prediction_error(1));
          );

          filter[f[bestCol]].update(zs[bestRow],h,getFrameInfo());
      }

      // phase 3.3: remove bestCol and bestRow from vectors of filters f and measurements zs/ps and delete corresponding row and col in the scores matrix
      f.erase(f.begin() + bestCol);
      zs.erase(zs.begin() + bestRow);
      ps.erase(ps.begin() + bestRow);

      long int numRows = scores.rows()-1;
      long int numCols = scores.cols();

      if( bestRow < numRows )
          scores.block(bestRow,0,numRows-bestRow,numCols) = scores.bottomRows(numRows-bestRow).eval();

      scores.conservativeResize(numRows,numCols);

      --numCols;

      if( bestCol < numCols )
          scores.block(0,bestCol,numRows,numCols-bestCol) = scores.rightCols(numCols-bestCol).eval();

      scores.conservativeResize(numRows,numCols);

    } while(!(f.empty() || zs.empty()));
  }

  // phase 4: create new filter for each percept which couldn't be associated with a filter
  for(const Vector2d& p : ps){
    Eigen::Vector4d newState;
    newState << p.x, 0, p.y, 0;
    filter.push_back(BallHypothesis(getFrameInfo(), newState, processNoiseStdSingleDimension, measurementNoiseCovariances, initialStateStdSingleDimension));
  }
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

  DEBUG_REQUEST("MultiKalmanBallLocator:provide_model_based_on_covariance",
    // find the best seen model for the ball based on covariance
    for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); ++iter) {
        double temp = iter->getEllipseLocation().major * iter->getEllipseLocation().minor * Math::pi;
        if(bestModel == filter.end() || temp < value){
            bestModel = iter;
            value = temp;
        }
    }

    return bestModel;
  );

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

void MultiKalmanBallLocator::negativeUpdate() {
    for(BallHypothesis& bh: filter){
       Vector3d position(bh.getState()(0), bh.getState()(2), getFieldInfo().ballRadius);
       // model is in close range
       if(position.abs() < 1500){
           Vector2i pos_in_image;
           // model is visible in image
           if(CameraGeometry::relativePointToImage(getCameraMatrix(), CameraInfo::Bottom, position, pos_in_image)){
               if(getImage().isInside(pos_in_image.x, pos_in_image.y)){
                   // get radius of ball in image
                   double radius_in_image = CameraGeometry::estimatedBallRadius(getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,pos_in_image.x, pos_in_image.y);
                   if(radius_in_image < 0){
                       continue;
                   }

                   radius_in_image += kfParameters.negative_update.radius_offset;

                   // create patch to check
                   uint32_t x_min = static_cast<uint32_t>(Math::clamp(static_cast<int>(pos_in_image.x - radius_in_image), 0, static_cast<int>(getImage().width()-1)));
                   uint32_t x_max = static_cast<uint32_t>(Math::clamp(static_cast<int>(pos_in_image.x + radius_in_image), 0, static_cast<int>(getImage().width()-1)));
                   uint32_t y_min = static_cast<uint32_t>(Math::clamp(static_cast<int>(pos_in_image.y - radius_in_image), 0, static_cast<int>(getImage().height()-1)));
                   uint32_t y_max = static_cast<uint32_t>(Math::clamp(static_cast<int>(pos_in_image.y + radius_in_image), 0, static_cast<int>(getImage().height()-1)));

                   // count green inside
                   int32_t FACTOR = getBallDetectorIntegralImage().FACTOR;
                   double greenInside = getBallDetectorIntegralImage().getDensityForRect(x_min/FACTOR, y_min/FACTOR, x_max/FACTOR, y_max/FACTOR, 1);

                   // decide if stall counting is necessary
                   if(greenInside > kfParameters.negative_update.max_green_inside) {
                     // too much green inside patch
                     RECT_PX(ColorClasses::red, x_min, y_min, x_max, y_max);
                     bh.stall_count++;
                   } else {
                     // still enough different from green
                     RECT_PX(ColorClasses::blue, x_min, y_min, x_max, y_max);
                   }
               }
           }
       }
    }
}
