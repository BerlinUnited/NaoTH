#include "MultiKalmanBallLocator.h"

#include <Eigen/Core>
#include "Tools/Association.h"

MultiKalmanBallLocator::MultiKalmanBallLocator()
{
    // Modify number of models
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:remove_all_models",     "remove all models",                                                             false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:allow_just_one_model",  "allows only one model to be generated (all updates are applied to that model)", false);

    // Debug Drawings
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_real_ball_percept",          "draw the real incomming ball percept",                               false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_ball_on_field_before",       "draw the modelled ball on the field before prediction and update",   false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_ball_on_field",              "draw the modelled ball on the field before update",                  false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_ball_on_field_after",        "draw the modelled ball on the field after prediction and update",    false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_assignment",                 "draws the assignment of the ball percept to the filter",             false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_final_ball",                 "draws the final i.e. best model",                                    false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_final_ball_postion_at_rest", "draws the final i.e. best model's rest position",                    false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_covariance_ellipse",         "draws the ellipses representing the covariances",                    false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_last_known_ball",            "draws the last known ball", 	                                   false);

    // Plotting Related Debug Requests
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:plot_prediction_error",     "plots the prediction errors in x (horizontal angle) and y (vertical angle)", false);

    // Update Association Function Debug Requests
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:UpdateAssociationFunction:useEuclid",            "minimize Euclidian distance in measurement space",                                false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:UpdateAssociationFunction:useMahalanobis",       "minimize Mahalanobis distance in measurement space (no common covarince matrix)", false);
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:UpdateAssociationFunction:useMaximumLikelihood", "maximize likelihood of measurement in measurement space ",                        true );

    // Parameter Related Debug Requests
    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:reloadParameters",          "reloads the kalman filter parameters from the kfParameter object", false);

    DEBUG_REQUEST_REGISTER("MultiKalmanBallLocator:draw_trust_the_ball", "..", false);

    h.ballRadius = getFieldInfo().ballRadius;

    updateAssociationFunction = &likelihood;

    getDebugParameterList().add(&params);

    filter.reserve(10);  // set capacity for improved performance

    reloadParameters();
}

MultiKalmanBallLocator::~MultiKalmanBallLocator()
{
    getDebugParameterList().remove(&params);
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
    // NOTE: make sure at least one filter remains, so filter is not empty
    Filters::iterator iter = filter.begin();
    while(iter != filter.end() && filter.size() > 1) {
      // TODO: here we make a linear approximation depending n the distance - does it make problems? Can we have a better solution?
      double distance = Vector2d(iter->getState()(0), iter->getState()(2)).abs();
      double threshold_radius = params.area95Threshold_radius.factor * distance + params.area95Threshold_radius.offset;
      // HACK: we need ballSeenFilter here because the other condition didn't seem to work as expected
      // compare area of the position uncertainty ellipse with a circle representing the maximum uncertainty a ball can have
      // Note: pi is avoided on both sides of the inequality
      if(!iter->ballSeenFilter.value() && (*iter).getEllipseLocation().major * (*iter).getEllipseLocation().minor > threshold_radius*threshold_radius){
          iter = filter.erase(iter);
      } else {
          ++iter;
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
    for(Filters::iterator iter = filter.begin(); iter != filter.end(); ++iter) {
      predict(*iter, dt);
    }

    doDebugRequestBeforUpdate();

    // sensor update
    if(params.association.use_normal) {
        updateByPerceptsNormal();
    } else if(params.association.use_cool) {
        updateByPerceptsCool();
    } else if(params.association.use_greedy) {
        // need to handle bottom and top percepts independently because both cameras can observe the same ball
        updateByPerceptsGreedy(CameraInfo::Bottom);
        updateByPerceptsGreedy(CameraInfo::Top);
    }

    // NOTE: (Heinrich) update the "ball seen" values
    for(Filters::iterator iter = filter.begin(); iter != filter.end(); ++iter) {
      bool updated = iter->getLastUpdateFrame().getFrameNumber() == getFrameInfo().getFrameNumber();
      (*iter).ballSeenFilter.setParameter(params.g0, params.g1);
      (*iter).ballSeenFilter.update(updated, 0.3, 0.7);
    }

    // estimate the best model
    if(params.use_covariance_based_selection) {
        bestModel = selectBestModelBasedOnCovariance();
    } else {
        bestModel = selectBestModel();
    }

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

/*
 * Perform a greedy 1:1-matching of filters (hypothesis) and measurements (ball percept).
 * The association depends on the horizontal and vertical angles of the percept and
 * the hypothesis in the image. Each combination is assigned a score. The combination
 * with the best score is choosen and the filter updated with the measurement.
 * After that the second best and so on...
 */
void MultiKalmanBallLocator::updateByPerceptsGreedy(CameraInfo::CameraID camera)
{
    // set correct camera matrix and info in functional
    if(camera == CameraInfo::Bottom) {
        h.camMat  = getCameraMatrix();
        h.camInfo = getCameraInfo();
    } else {
        h.camMat  = getCameraMatrixTop();
        h.camInfo = getCameraInfoTop();
    }

    // phase 1: filter percepts
    std::vector<Eigen::Vector2d, Eigen::aligned_allocator<Eigen::Vector2d> > measurements;
    std::vector<Vector2d> positions;

    for(const MultiBallPercept::BallPercept& percept: getMultiBallPercept().getPercepts()){
        Eigen::Vector2d z;

        if(percept.cameraId == camera){
            // transform measurement into angles
            Vector2d angles = CameraGeometry::pixelToAngles(h.camInfo, percept.centerInImage.x, percept.centerInImage.y);
            z << angles.x, angles.y;
            measurements.push_back(z);

            // needed if a new filter has to be created
            positions.push_back(percept.positionOnField);
        }
    }

    // abort if there are no measurements for the current camera
    if(measurements.empty()) return;

    // if no filter exists, skip directly to phase 4
    if(!filter.empty())
    {
        // phase 2: calculate #measurement x #filter score-matrix
        // Note: it depends on the association function whether a low or a high score is better
        Eigen::MatrixXd scores = Eigen::MatrixXd::Zero(measurements.size(), filter.size());
        for(size_t i = 0; i < measurements.size(); ++i){
            for(size_t j = 0; j < filter.size(); ++j){
                scores(i,j) = updateAssociationFunction->associationScore(filter[j], measurements[i], h);
            }
        }

        // create index vector for filters
        std::vector<int> filterIndices;
        for(size_t i = 0; i < filter.size(); ++i){
            filterIndices.push_back((int)i);
        }

        // phase 3: greedyly update each filter by best matching percept
        while(!(filterIndices.empty() || measurements.empty())){
            // phase 3.1: find location of best entry
            // which is depending on the updateAssociationFunction a global maximum or global minimum
            Eigen::Index maxColIdx, minColIdx, maxRowIdx, minRowIdx, bestColIdx, bestRowIdx;
            double max = scores.maxCoeff(&maxRowIdx, &maxColIdx);
            double min = scores.minCoeff(&minRowIdx, &minColIdx);

            if(updateAssociationFunction->better(min, max)){
                bestColIdx = minColIdx;
                bestRowIdx = minRowIdx;
            } else {
                bestColIdx = maxColIdx;
                bestRowIdx = maxRowIdx;
            }

            // phase 3.2: update filter
            bool allowJustOneModel = false;
            DEBUG_REQUEST("MultiKalmanBallLocator:allow_just_one_model",
                allowJustOneModel = true;
            );

            // if the score is not in the valid range continue with phase 4
            // because no other possible matching could be better than the current one
            if(!allowJustOneModel && !updateAssociationFunction->inRange(scores(bestRowIdx,bestColIdx))) {
                break;
            } else {
                DEBUG_REQUEST("MultiKalmanBallLocator:draw_assignment",
                    FIELD_DRAWING_CONTEXT;
                    PEN("FF0000", 10);
                    const Eigen::Vector4d state = filter[filterIndices[bestColIdx]].getState();
                    LINE(positions[bestRowIdx].x, positions[bestRowIdx].y, state(0), state(2));
                    TEXT_DRAWING((positions[bestRowIdx].x + state(0)) / 2, (positions[bestRowIdx].y + state(2)) / 2, scores(bestRowIdx,bestColIdx));
                );

                DEBUG_REQUEST("MultiKalmanBallLocator:plot_prediction_error",
                    Eigen::Vector2d prediction_error;
                    prediction_error = measurements[bestRowIdx] - filter[filterIndices[bestColIdx]].getStateInMeasurementSpace(h);

                    PLOT("MultiKalmanBallLocator:Innovation:x", prediction_error(0));
                    PLOT("MultiKalmanBallLocator:Innovation:y", prediction_error(1));
                );

                filter[filterIndices[bestColIdx]].update(measurements[bestRowIdx], h, getFrameInfo());
            }

            // phase 3.3: remove matching pair of hypothesis and measurement
            // from the set of matchable options, that means:
            // - remove bestColIdx from filterIndices
            // - remove the measurement and its position from measurements and positions
            // - remove their corresponding row and column in the score matrix
            filterIndices.erase(filterIndices.begin() + bestColIdx);
            measurements.erase(measurements.begin() + bestRowIdx);
            positions.erase(positions.begin() + bestRowIdx);

            Eigen::Index numRowsAfterResize = scores.rows() - 1;
            Eigen::Index numColsAfterResize = scores.cols() - 1;
            Eigen::Index numRowsAfterBestRow = numRowsAfterResize - bestRowIdx;
            Eigen::Index numColsAfterBestCol = numColsAfterResize - bestColIdx;

            // delete the row by moving all rows after the best row one row up and resize the matrix accordingly
            if(bestRowIdx < numRowsAfterResize)
                scores.block(bestRowIdx, 0, numRowsAfterBestRow, scores.cols()) = scores.bottomRows(numRowsAfterBestRow).eval();
            scores.conservativeResize(numRowsAfterResize, scores.cols());

            // delete the column by moving all columns right of the best column one column to the left and resize the matrix accordingly
            if(bestColIdx < numColsAfterResize)
                scores.block(0, bestColIdx, numRowsAfterResize, numColsAfterBestCol) = scores.rightCols(numColsAfterBestCol).eval();
            scores.conservativeResize(numRowsAfterResize, numColsAfterResize);
        }
    }

    // phase 4: create new filter for each percept which couldn't be associated with a filter
    // Diskussion: vielleicht filter erstellen wenn keine da ist und dann die zweite beobachtung dazu matchen ...
    for(const Vector2d& p : positions){
        Eigen::Vector4d newState;
        newState << p.x, 0, p.y, 0;
        // add BallHypothesis to filter vector
        filter.emplace_back(getFrameInfo(), newState, processNoiseStdSingleDimension, measurementNoiseCovariances, initialStateStdSingleDimension);
    }

    // TODO maybe add possibility to merge filters
    // TODO maybe add possibility to apply update of observation to every filter in a bayesian fashion
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
    Eigen::Vector2d vel; // control vector
    vel <<  x(1), x(3);
    double abs_velocity = vel.norm();

    double time_until_vel_zero = 0;

    if(abs_velocity > epsilon){
        time_until_vel_zero = -abs_velocity/getFieldInfo().ballDeceleration;
    }

    if(time_until_vel_zero < epsilon)
    {
        filter.predict(Eigen::Vector2d::Zero(),dt);
    } else {
        // ballDeceleration is negative so the deceleration will be in opposite direction of current velocity
        Eigen::Vector2d u = vel.normalized() * getFieldInfo().ballDeceleration;
        if(time_until_vel_zero >= dt) {
            filter.predict(u,dt);
        } else {
            filter.predict(u, time_until_vel_zero);
            dt -= time_until_vel_zero;
            filter.predict(Eigen::Vector2d::Zero(), dt);
        }
    }

    return;
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

// TODO: returns the first model as best model even if it is "not seen"
//		 it might be better to return an invalid iterator and handle this case outside
//		 handling this better might make last_known_ball symbol obsolete
MultiKalmanBallLocator::Filters::const_iterator MultiKalmanBallLocator::selectBestModel() const
{
  // find the best model for the ball: closest hypothesis that is "known"
  Filters::const_iterator bestModel = filter.end();
  double minDistance = 0;

  for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); ++iter) {
    double distance = Vector2d(iter->getState()(0), iter->getState()(2)).abs();
    if( bestModel == filter.end() ||
       (iter->ballSeenFilter.value() && !bestModel->ballSeenFilter.value()) ||
       (iter->ballSeenFilter.value() == bestModel->ballSeenFilter.value() && distance < minDistance))
    {
      bestModel = iter;
      minDistance = distance;
    }
  }

  return bestModel;
}

MultiKalmanBallLocator::Filters::const_iterator MultiKalmanBallLocator::selectBestModelBasedOnCovariance() const
{
  Filters::const_iterator bestModel = filter.end();
  double value = 0;

  // find the best seen model for the ball based on covariance
  for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); ++iter) {
      double temp = iter->getEllipseLocation().major * iter->getEllipseLocation().minor * Math::pi;
      if(bestModel == filter.end()
         || (iter->ballSeenFilter.value() && !bestModel->ballSeenFilter.value())
         || (iter->ballSeenFilter.value() == bestModel->ballSeenFilter.value() && temp < value)) {
          bestModel = iter;
          value = temp;
      }
  }

  return bestModel;
}

void MultiKalmanBallLocator::provideBallModel(const BallHypothesis& model)
{
  getBallModel().valid = true;
  getBallModel().knows = model.ballSeenFilter.value();
  getBallModel().setFrameInfoWhenBallWasSeen(model.getLastUpdateFrame());

  // set ball model representation
  const Eigen::Vector4d& x = model.getState();
  getBallModel().position.x = x(0);
  getBallModel().position.y = x(2);
  getBallModel().speed.x = x(1);
  getBallModel().speed.y = x(3);

  // transform ball model into feet coordinates
  const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;
  Vector2d ballLeftFoot  = lFoot.projectXY()/getBallModel().position;
  Vector2d ballRightFoot = rFoot.projectXY()/getBallModel().position;

  //set preview ball model representation
  getBallModel().positionPreview = getMotionStatus().plannedMotion.hip / getBallModel().position;
  getBallModel().positionPreviewInLFoot = getMotionStatus().plannedMotion.lFoot / ballLeftFoot;
  getBallModel().positionPreviewInRFoot = getMotionStatus().plannedMotion.rFoot / ballRightFoot;

  // determine rest position
  Eigen::Vector2d vel;
  vel <<  x(1), x(3);
  double abs_velocity = vel.norm();

  BallHypothesis modelCopy(model);
  if(abs_velocity > epsilon){
      double time_until_vel_zero = -abs_velocity / getFieldInfo().ballDeceleration;
      Eigen::Vector2d u = vel.normalized() * getFieldInfo().ballDeceleration;
      modelCopy.predict(u, time_until_vel_zero);

      DEBUG_REQUEST("MultiKalmanBallLocator:draw_final_ball_postion_at_rest",
          FIELD_DRAWING_CONTEXT;
          PEN(Color(Color::black), 20);
          CIRCLE(modelCopy.getState()(0), modelCopy.getState()(2), getFieldInfo().ballRadius-10);
      );
  }

  // set position at rest
  getBallModel().position_at_rest.x = modelCopy.getState()(0);
  getBallModel().position_at_rest.y = modelCopy.getState()(2);

  // update last known ball
  if(getBallModel().knows) {
    getBallModel().last_known_ball = getBallModel().position;
  } else {
    // need to update odometry by hand ...
    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
    getBallModel().last_known_ball = odometryDelta * getBallModel().last_known_ball;
  }

  // some final debug stuff
  PLOT("MultiKalmanBallLocator:ballSeenFilter", model.ballSeenFilter.floatValue());
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

    DEBUG_REQUEST("MultiKalmanBallLocator:draw_last_known_ball",
        FIELD_DRAWING_CONTEXT;
        PEN("EF871E", 10);
        CIRCLE(getBallModel().last_known_ball.x, getBallModel().last_known_ball.y, getFieldInfo().ballRadius-10);
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

void MultiKalmanBallLocator::drawFilter(const BallHypothesis& bh, const Color& model_color, Color cov_loc_color, Color cov_vel_color) const
{
    bool draw_covariances = false;
    DEBUG_REQUEST("MultiKalmanBallLocator:draw_covariance_ellipse",
        draw_covariances = true;
    );

    if(!draw_covariances) {
        cov_loc_color[cov_loc_color.Alpha] = 0;
        cov_vel_color[cov_vel_color.Alpha] = 0;
    }

    PEN(model_color.toString(),20);

    const Eigen::Vector4d& state = bh.getState();

    CIRCLE( state(0), state(2), getFieldInfo().ballRadius-10);
    ARROW( state(0), state(2),
           state(0)+state(1),
           state(2)+state(3));

    PEN(cov_loc_color.toString(), 20);
    const Ellipse2d& ellipse_loc = bh.getEllipseLocation();
    OVAL_ROTATED(state(0),
                 state(2),
                 ellipse_loc.minor,
                 ellipse_loc.major,
                 ellipse_loc.angle);

    PEN(cov_vel_color, 20);
    const Ellipse2d& ellipse_vel = bh.getEllipseVelocity();
    OVAL_ROTATED(state(0)+state(1),
                 state(2)+state(3),
                 ellipse_vel.minor,
                 ellipse_vel.major,
                 ellipse_vel.angle);

    if (!draw_covariances) {
      cov_loc_color[cov_loc_color.Alpha] = 1;
      cov_vel_color[cov_vel_color.Alpha] = 1;
    }
}

void MultiKalmanBallLocator::drawFiltersOnField() const
{
    FIELD_DRAWING_CONTEXT;

    Color cov_loc_color("00FFFF");  // cyan
    Color cov_vel_color("FF00FF");  // pink
    Color model_color;

    for(Filters::const_iterator iter = filter.begin(); iter != filter.end(); iter++) {
        if(getBallModel().valid) {
            if((*iter).getLastUpdateFrame().getTime() == getFrameInfo().getTime()) {
                if(bestModel == iter)
                    model_color = "99FF00";  // green
                else
                    model_color = "FF9900";  // orange
            } else {
                    model_color = "0099FF";  // light blue
            }
        } else {
            model_color = "999999";  // grey
        }

        drawFilter(*iter, model_color, cov_loc_color, cov_vel_color);
    }
}

void MultiKalmanBallLocator::reloadParameters()
{
    // parameters for initializing new filters
    processNoiseStdSingleDimension << params.processNoiseStdQ00, params.processNoiseStdQ01,
                                      params.processNoiseStdQ10, params.processNoiseStdQ11;

    measurementNoiseCovariances << params.measurementNoiseR00, params.measurementNoiseR10,
                                   params.measurementNoiseR10, params.measurementNoiseR11;

    initialStateStdSingleDimension << params.initialStateStdP00, params.initialStateStdP01,
                                      params.initialStateStdP10, params.initialStateStdP11;

    // UAF thresholds
    euclid.setThreshold(params.euclidThreshold);
    mahalanobis.setThreshold(params.mahalanobisThreshold);
    likelihood.setThreshold(params.maximumLikelihoodThreshold);

    // update existing filters with new process and measurement noise
    Eigen::Matrix2d processNoiseCovariancesSingleDimension;
    processNoiseCovariancesSingleDimension = processNoiseStdSingleDimension.cwiseProduct(processNoiseStdSingleDimension);

    for(Filters::iterator iter = filter.begin(); iter != filter.end(); iter++){
        (*iter).setCovarianceOfProcessNoise(processNoiseCovariancesSingleDimension);
        (*iter).setCovarianceOfMeasurementNoise(measurementNoiseCovariances);
    }
}
