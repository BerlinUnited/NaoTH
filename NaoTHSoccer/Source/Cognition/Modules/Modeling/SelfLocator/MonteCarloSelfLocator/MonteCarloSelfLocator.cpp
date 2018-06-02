/**
* @file MonteCarloSelfLocator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MonteCarloSelfLocator
*/

#include "MonteCarloSelfLocator.h"

// tools
#include "Tools/Math/Probabilistics.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools.h"

using namespace std;
using namespace mcsl;

MonteCarloSelfLocator::MonteCarloSelfLocator() 
  :
  state(LOCALIZE),
  lastState(LOCALIZE),
  islocalized(false),
  updatedByGoalPosts(false),
  theSampleSet(100),
  canopyClustering(parameters.thresholdCanopy),
  slowWeighting(0.0),
  fastWeighting(0.0),
  effective_number_of_samples(0.0)
{
  // debug
  DEBUG_REQUEST_REGISTER("MCSLS:reset_samples", "reset the sample set", false);

  // field drawings
  DEBUG_REQUEST_REGISTER("MCSLS:draw_Samples", "draw sample set before resampling", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_BackendSamples", "draw sample set before resampling", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_Samples_effective", "draw sample set before resampling", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_post_choice", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_sensor_belief", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_sensorResetBySensingGoalModel", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_state", "visualizes the state of the self locator on the field", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_updateByLinePoints", "", false);

  // resampling
  DEBUG_REQUEST_REGISTER("MCSLS:resample_sus", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:resample_simple", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:resample_gt", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:resample_hm", "", false);

  // resulting position
  DEBUG_REQUEST_REGISTER("MCSLS:draw_Cluster", "draw the clustered particle set", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_position","draw robots position (self locator)", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_deviation", "", false);

  DEBUG_REQUEST_REGISTER("MCSLS:state:KIDNAPPED", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:state:BLIND", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:state:LOCALIZE", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:state:TRACKING", "", false);

  initializeSampleSet(getFieldInfo().carpetRect, theSampleSet);
  getDebugParameterList().add(&parameters);
}

MonteCarloSelfLocator::~MonteCarloSelfLocator() 
{
  getDebugParameterList().remove(&parameters);
}


void MonteCarloSelfLocator::execute()
{
  // reset
  updatedByGoalPosts = false;

  DEBUG_REQUEST("MCSLS:reset_samples",

    resetLocator();
    state = LOCALIZE;

    DEBUG_REQUEST("MCSLS:draw_Samples", 
      FIELD_DRAWING_CONTEXT;
      theSampleSet.drawImportance(getDebugDrawings());
    );

    return;
  );

  // treat the situation when the robot has been lifted from the ground
  // (keednapped)

  // only in stand, walk or init(!)
  bool motion_ok_kidnap = getMotionStatus().currentMotion == motion::stand ||
                   getMotionStatus().currentMotion == motion::init ||
                   getMotionStatus().currentMotion == motion::walk;

  // true when the robot was lifted up
  bool body_lift_up =  getBodyState().isLiftedUp;

  if(parameters.treatLiftUp && motion_ok_kidnap && body_lift_up) {
    state = KIDNAPPED;
  }

  bool last_motion_ok = getMotionStatus().lastMotion == motion::stand ||
                        getMotionStatus().lastMotion == motion::walk;                    

  bool motion_ok = (getMotionStatus().currentMotion == motion::stand || 
                    getMotionStatus().currentMotion == motion::walk)
                   // hack: give stand some time in case the last motion was not walk or stand
                   // remark: walk is only executed after walk or stand, so this condition is only relevant for stand
                   && (last_motion_ok || getFrameInfo().getTimeSince(getMotionStatus().time) > 5000); 

  bool body_upright = getBodyState().fall_down_state == BodyState::upright;

  if(state != KIDNAPPED) 
  {
    if(parameters.treatInitState && (!motion_ok || !body_upright || getPlayerInfo().robotState == PlayerInfo::penalized)) 
    {
      state = BLIND;
    }
  }

  DEBUG_REQUEST("MCSLS:state:KIDNAPPED", state = KIDNAPPED; );
  DEBUG_REQUEST("MCSLS:state:BLIND", state = BLIND; );
  DEBUG_REQUEST("MCSLS:state:LOCALIZE", state = LOCALIZE; );
  DEBUG_REQUEST("MCSLS:state:TRACKING", state = TRACKING; );


  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    switch(state) {
      case KIDNAPPED: TEXT_DRAWING(0, 0, "KIDNAPPED"); break;
      case BLIND: TEXT_DRAWING(0, 0, "BLIND"); break;
      case LOCALIZE: TEXT_DRAWING(0, 0, "LOCALIZE"); break;
      case TRACKING: TEXT_DRAWING(0, 0, "TRACKING"); break;
      default: TEXT_DRAWING(0, 0, "DEFAULT");
    }
  );

  static unsigned localize_start = getFrameInfo().getTime();
  if(state != LOCALIZE) {
    localize_start = getFrameInfo().getTime();
  }


  switch(state) 
  {
    case KIDNAPPED:
    {
      resetLocator();
      state = LOCALIZE;
      islocalized = false;
      lastState = KIDNAPPED;
      getRobotPose().isValid = false;
      break;
    }
    case BLIND:
    {
      if(parameters.updateByOdometryWhenBlind) {
        updateByOdometry(theSampleSet, parameters.motionNoise);
      }

      /* do nothing */
      if(!islocalized) {
        state = LOCALIZE;
      } else {
        state = TRACKING;
      }
      lastState = BLIND;
      break;
    }
    case LOCALIZE:
    {
      updateByOdometry(theSampleSet, parameters.motionNoise);
    
      theSampleSet.resetLikelihood();

      // HACK: separate update by the goal posts
      if(parameters.updateByGoalPostLocalize)
      {
        if(getGoalPercept().getNumberOfSeenPosts() > 0) {
          updateByGoalPosts(getGoalPercept(), theSampleSet);
          updatedByGoalPosts = true;
        }
        if(getGoalPerceptTop().getNumberOfSeenPosts() > 0) {
          updateByGoalPosts(getGoalPerceptTop(), theSampleSet);
          updatedByGoalPosts = true;
        }
      }

      updateBySensors(theSampleSet);

      // use prior knowledge
      if(parameters.updateBySituation) //  && lastState == KIDNAPPED
      {
        updateBySituation();
      }


      // NOTE: statistics has to be after updates and before resampling
      // NOTE: normalizes the likelihood
      updateStatistics(theSampleSet);

      resampleMH(theSampleSet);
      //resampleMHOld(theSampleSet);

      sensorResetBySensingGoalModel(theSampleSet, (int)theSampleSet.size() - 1);

      // estimate the state
      canopyClustering.cluster(mhBackendSet);
    
      Moments2<2> moments;
      Sample meanPose = mhBackendSet.meanOfCluster(moments, canopyClustering.getLargestClusterID());

      unsigned localize_time = getFrameInfo().getTimeSince(localize_start);

      if(localize_time > 5000 && moments.getRawMoment(0,0) > 0.8*(double)mhBackendSet.size()) {
        theSampleSet = mhBackendSet; // todo: implement swap
        state = TRACKING;
      }

      calculatePose(mhBackendSet);
      
      DEBUG_REQUEST("MCSLS:draw_Samples_effective", 
        FIELD_DRAWING_CONTEXT;
        mhBackendSet.drawImportance(getDebugDrawings());
      );

      islocalized = true;
      lastState = LOCALIZE;
      break;
    }
    case TRACKING:
    {
      updateByOdometry(theSampleSet, parameters.motionNoise);

      theSampleSet.resetLikelihood();

      // HACK: separate update by the goal posts
      if(parameters.updateByGoalPostTracking)
      {
        if(getGoalPercept().getNumberOfSeenPosts() > 0) {
          updateByGoalPosts(getGoalPercept(), theSampleSet);
          updatedByGoalPosts = true;
        }
        if(getGoalPerceptTop().getNumberOfSeenPosts() > 0) {
          updateByGoalPosts(getGoalPerceptTop(), theSampleSet);
          updatedByGoalPosts = true;
        }
      }

      updateBySensors(theSampleSet);


      //HACK
      if(parameters.updateBySituation) {
        if(getPlayerInfo().robotState == PlayerInfo::set) 
        {
          updateByOwnHalf(theSampleSet);
        }
      }


      // NOTE: statistics has to be after updates and before resampling
      // NOTE: normalizes the likelihood
      updateStatistics(theSampleSet);

      if(parameters.resampleSUS) {
        resampleSUS(theSampleSet, (int)theSampleSet.size());
      }
      if(parameters.resampleGT07) {
        resampleGT07(theSampleSet, true);
      }

      calculatePose(theSampleSet);

      DEBUG_REQUEST("MCSLS:draw_Samples_effective", 
        FIELD_DRAWING_CONTEXT;
        theSampleSet.drawImportance(getDebugDrawings());
      );

      lastState = TRACKING;
      break;
    }
    default: assert(false); // should never be here
  }
  
  lastRobotOdometry = getOdometryData();


  /************************************
  * STEP VII: execude some debug requests (drawings)
  ************************************/

  DEBUG_REQUEST("MCSLS:resample_sus",
    resampleSUS(theSampleSet, (int)theSampleSet.size());
  );

  DEBUG_REQUEST("MCSLS:resample_gt", 
    resampleGT07(theSampleSet, true);
  );

  DEBUG_REQUEST("MCSLS:resample_hm", 
    resampleMH(theSampleSet);
  );
  
  DEBUG_REQUEST("MCSLS:resample_simple", 
    resampleSimple(theSampleSet, (int)((double)theSampleSet.size() - effective_number_of_samples + 0.5));
  );

  DEBUG_REQUEST("MCSLS:draw_sensor_belief",
    draw_sensor_belief();
  );

  DEBUG_REQUEST("MCSLS:draw_Samples",
    FIELD_DRAWING_CONTEXT;
    theSampleSet.drawImportance(getDebugDrawings());
  );

  DEBUG_REQUEST("MCSLS:draw_BackendSamples",
    FIELD_DRAWING_CONTEXT;
    mhBackendSet.drawImportance(getDebugDrawings(), false);
  );

}//end execute

void MonteCarloSelfLocator::resetLocator()
{
  if(parameters.resetOwnHalf && getPlayerInfo().robotState == PlayerInfo::set) {
    initializeSampleSetFixedRotation(getFieldInfo().ownHalfRect, 0, theSampleSet);
  } else {
    initializeSampleSet(getFieldInfo().carpetRect, theSampleSet);
  }
    
  mhBackendSet = theSampleSet;
  //mhBackendSet.setLikelihood(0.0);
}

void MonteCarloSelfLocator::updateByOdometry(SampleSet& sampleSet, bool noise) const
{
  Pose2D odometryDelta = getOdometryData() - lastRobotOdometry;
  for (size_t i = 0; i < sampleSet.size(); i++)
  {
    sampleSet[i] += odometryDelta;
    if(noise)
    {
      sampleSet[i].translation.x += (Math::random()-0.5)*parameters.motionNoiseDistance;
      sampleSet[i].translation.y += (Math::random()-0.5)*parameters.motionNoiseDistance;
      sampleSet[i].rotation = Math::normalize(sampleSet[i].rotation + (Math::random()-0.5)*parameters.motionNoiseAngle);
    }

    /*
    Pose2D odometryModel(odometryDelta);

    if(noise) {
      odometryModel.translation += odometryModel.translation * (Math::random()-0.5)*parameters.motionNoiseDistance;
      odometryModel.rotation += odometryModel.rotation * (Math::random()-0.5)*parameters.motionNoiseAngle;
    }

    sampleSet[i] += odometryModel;
    */
  }
}//end updateByOdometry

void MonteCarloSelfLocator::updateBySituation()
{
  if(getSituationPrior().currentPrior == getSituationPrior().firstReady)
    {
      updateByStartPositions(theSampleSet);
    }
    else if(getSituationPrior().currentPrior == getSituationPrior().positionedInSet)
    {
      updateByOwnHalfLookingForward(theSampleSet);
    }
    else if(getSituationPrior().currentPrior == getSituationPrior().goaliePenalizedInSet)
    {
      updateByGoalBox(theSampleSet);
    }
    else if(getSituationPrior().currentPrior == getSituationPrior().set)
    {
      updateByOwnHalf(theSampleSet);
    }
    else if(getSituationPrior().currentPrior == getSituationPrior().playAfterPenalized)
    {
      updateBySidePositions(theSampleSet);
    }
    else if(getSituationPrior().currentPrior == getSituationPrior().oppHalf)
    {
      updateByOppHalf(theSampleSet);
    }
    else {
      DEBUG_REQUEST("MCSLS:draw_state",
        FIELD_DRAWING_CONTEXT;
        PEN("000000", 30);
        const Vector2d& fieldMin = getFieldInfo().fieldRect.min();
        const Vector2d& fieldMax = getFieldInfo().fieldRect.max();
        BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
        LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
        LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
      );
    }
}

bool MonteCarloSelfLocator::updateBySensors(SampleSet& sampleSet) const
{
  /*
  if(parameters.updateByGoalPost)
  {
    if(getGoalPercept().getNumberOfSeenPosts() > 0) {
      updateByGoalPosts(getGoalPercept(), sampleSet);
    }
    if(getGoalPerceptTop().getNumberOfSeenPosts() > 0) {
      updateByGoalPosts(getGoalPerceptTop(), sampleSet);
    }
  }
  */

  if(parameters.updateByMiddleCircle) {
    updateByMiddleCircle(getLinePercept(), sampleSet);
  }

  if(parameters.updateByCompas && getProbabilisticQuadCompas().isValid()) {
    updateByCompas(sampleSet);
  }

  if(parameters.updateByLinePoints)
  {
    if(!getLineGraphPercept().edgelsOnField.empty()) {
      updateByLinePoints(getLineGraphPercept(), sampleSet);
    }
  }

  if(parameters.updateByLinePercept)
  {
    if(!getLinePercept().lines.empty()) {
      updateByLines(getLinePercept(), sampleSet);
    }
  }

  if(parameters.updateByShortLinePercept)
  {
    if(!getLinePercept().short_lines.empty()) {
      updateByShortLines(getLinePercept(), sampleSet);
    }
  }

  return true;
}

void MonteCarloSelfLocator::updateByGoalPosts(const GoalPercept& goalPercept, SampleSet& sampleSet) const
{
  for(int i = 0; i < goalPercept.getNumberOfSeenPosts(); i++)
  {
    const GoalPercept::GoalPost& seenPost = goalPercept.getPost(i);
    
    // HACK
    if(state == TRACKING && parameters.maxAcceptedGoalErrorWhileTracking > 0) {

      const Vector2d* leftGoalPosition = NULL;
      const Vector2d* rightGoalPosition = NULL;

      if((getRobotPose()*seenPost.position).x > 0)
      {
        leftGoalPosition = &(getFieldInfo().opponentGoalPostLeft);
        rightGoalPosition = &(getFieldInfo().opponentGoalPostRight);
      } else {
        // own goals are switched (!)
        leftGoalPosition = &(getFieldInfo().ownGoalPostRight);
        rightGoalPosition = &(getFieldInfo().ownGoalPostLeft);
      }

      Vector2d globalPercept = getRobotPose() * seenPost.position;
      double min_dist = min((globalPercept - *rightGoalPosition).abs(),(globalPercept - *leftGoalPosition).abs());

      if(min_dist < parameters.maxAcceptedGoalErrorWhileTracking) {
        updateBySingleGoalPost(seenPost, sampleSet);
      }
    } else {
      updateBySingleGoalPost(seenPost, sampleSet);
    }
  }
}

void MonteCarloSelfLocator::updateBySingleGoalPost(const GoalPercept::GoalPost& seenPost, SampleSet& sampleSet) const
{
  const double sigmaAngle    = parameters.goalPostSigmaAngle;
  const double sigmaDistance = parameters.goalPostSigmaDistance;
  const double cameraHeight  = getCameraMatrix().translation.z;

  const double seenDistance = seenPost.position.abs();
  const double seenAngle = seenPost.position.angle();

  const Vector2d* leftGoalPosition = NULL;
  const Vector2d* rightGoalPosition = NULL;

  for (size_t j = 0; j < sampleSet.size(); j++)
  { 
    Sample& sample = sampleSet[j];
    Vector2d expectedPostPosition;

    // each particle decides for itself
    //if(fabs(Math::normalize(sample.rotation + seenAngle)) < Math::pi_2)
    if((sample*seenPost.position).x > 0)
    {
      leftGoalPosition = &(getFieldInfo().opponentGoalPostLeft);
      rightGoalPosition = &(getFieldInfo().opponentGoalPostRight);
    } else {
      // own goals are switched (!)
      leftGoalPosition = &(getFieldInfo().ownGoalPostRight);
      rightGoalPosition = &(getFieldInfo().ownGoalPostLeft);
    }

    if(seenPost.type == GoalPercept::GoalPost::rightPost)
    {
      expectedPostPosition = *rightGoalPosition;
      // switch if the robot is behind the goal
      if( (expectedPostPosition.x < 0 && sample.translation.x < expectedPostPosition.x) ||
          (expectedPostPosition.x > 0 && sample.translation.x > expectedPostPosition.x))
      {
        expectedPostPosition = *leftGoalPosition;
      }
    } else if(seenPost.type == GoalPercept::GoalPost::leftPost) {
      expectedPostPosition = *leftGoalPosition;
      // switch if the robot is behind the goal
      if( (expectedPostPosition.x < 0 && sample.translation.x < expectedPostPosition.x) ||
          (expectedPostPosition.x > 0 && sample.translation.x > expectedPostPosition.x))
      {
        expectedPostPosition = *rightGoalPosition;
      }
    } else { // unknown post
      Vector2d globalPercept = sample * seenPost.position;
      // choose the closest one
      if((globalPercept - *rightGoalPosition).abs() < (globalPercept - *leftGoalPosition).abs()) {
        expectedPostPosition = *rightGoalPosition;
      } else {
        expectedPostPosition = *leftGoalPosition;
      }
    }

    Vector2d relPost = sample/expectedPostPosition;
    double expectedDistance = relPost.abs();
    double expectedAngle = relPost.angle();

    DEBUG_REQUEST("MCSLS:draw_post_choice",
      if(seenPost.type == GoalPercept::GoalPost::rightPost) {
        PEN("FF000099",1);
      } else if(seenPost.type == GoalPercept::GoalPost::leftPost) {
        PEN("0000FF99",1);
      } else {
        PEN("00000099",1);
      }
      LINE(sample.translation.x, sample.translation.y, expectedPostPosition.x, expectedPostPosition.y);
    );

    if(seenPost.positionReliable) {
      sample.likelihood *= computeDistanceWeight(seenDistance, expectedDistance, cameraHeight, sigmaDistance);
    }
    sample.likelihood *= computeAngleWeight(seenAngle, expectedAngle, sigmaAngle);

  } //end for j (samples)
}//end updateBySingleGoalPost

void MonteCarloSelfLocator::updateByCompas(SampleSet& sampleSet) const
{
  for(size_t i = 0; i < sampleSet.size(); i++) {
    Sample& sample = sampleSet[i];
    sample.likelihood *= getProbabilisticQuadCompas().probability(-sample.rotation);
  }
}



void MonteCarloSelfLocator::updateByLinePoints(const LineGraphPercept& lineGraphPercept, SampleSet& sampleSet) const
{
  const double sigmaDistance = parameters.linePointsSigmaDistance;
  //const double sigmaAngle    = parameters.linePointsSigmaAngle;
  const double cameraHeight  = getCameraMatrix().translation.z;

  DEBUG_REQUEST("MCSLS:draw_updateByLinePoints",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 10);
  );

  for(size_t i = 0; i < lineGraphPercept.edgelsOnField.size() && i < (size_t)parameters.linePointsMaxNumber; i++) 
  {
    int idx = Math::random((int)lineGraphPercept.edgelsOnField.size());
    const Vector2d& seen_point_relative = lineGraphPercept.edgelsOnField[idx].point;

    Vector2d seen_point_g = getRobotPose()*seen_point_relative;

    DEBUG_REQUEST("MCSLS:draw_updateByLinePoints",
      CIRCLE(seen_point_g.x, seen_point_g.y, 20);
    );

    for(size_t s=0; s < sampleSet.size(); s++)
    {
      Sample& sample = sampleSet[s];

      Vector2d seen_point_global = sample*seen_point_relative;
      LinesTable::NamedPoint line_point_global = getFieldInfo().fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);
     
      // there is no such line
      if(line_point_global.id == -1) {
        continue;
      }

      // get the line
      //const Math::LineSegment& ref_line = getFieldInfo().fieldLinesTable.getLines()[p.id];

      Vector2d line_point_relative(sample/line_point_global.position);

      sample.likelihood *= computeDistanceWeight(seen_point_relative.abs(), line_point_relative.abs(), cameraHeight, sigmaDistance);
    }
  }
}//end updateByLinePoints


void MonteCarloSelfLocator::updateByLines(const LinePercept& linePercept, SampleSet& sampleSet) const
{
  const double cameraHeight = getCameraMatrix().translation.z;
  const double sigmaDistance = parameters.goalPostSigmaDistance;
  const double sigmaAngle = parameters.goalPostSigmaAngle;
  double shortestLine = 1e+5; // very long...

  // todo: parameter for max lines to update by
  for(size_t lp=0; lp < linePercept.lines.size() && lp < 3; lp++)
  {
    //int idx = Math::random((int)linePercept.lines.size());
    // dont use the lines which are parts of the circle 
    // when the circle itself was detected
    //if(linePercept.lines[idx].type == LinePercept::C && linePercept.middleCircleWasSeen)
    //  continue;

    if(linePercept.lines[lp].lineOnField.getLength() < 300) { // don't use too short lines
      continue;
    }


    //const int centerLine_id = 4; // HACK: see FieldInfo
    // special treatment for the center line
    //if(linePercept.lines[lp].seen_id == LinePercept::center_line)
    //{
      // get the center line
      //const Math::LineSegment& centerLine = getFieldInfo().fieldLinesTable.getLines()[centerLine_id];
    //}//end if


    // TODO: separate class
    //int lineVotes[30] = {0};
    //int maxIdx = 0;

    const Math::LineSegment& relPercept = linePercept.lines[lp].lineOnField;

    for(size_t s=0; s < sampleSet.size(); s++)
    {
      Sample& sample = sampleSet[s];

      // statistics
      shortestLine = std::min(shortestLine, relPercept.getLength());

      // translocation of the line percept to the global coords
      Vector2d abs_begin      = sample*relPercept.begin();
      Vector2d abs_end        = sample*relPercept.end();
      Vector2d abs_direction  = abs_end - abs_begin;
      Vector2d abs_mid        = (abs_begin+abs_end)*0.5;

      // classify the line percept
      // todo: this may make problems when the lines are distored
      int length    = (relPercept.getLength() > 700)?LinesTable::long_lines:LinesTable::short_lines|LinesTable::long_lines;
      int direction = (fabs(abs_direction.x) > fabs(abs_direction.y))?LinesTable::along_lines:LinesTable::across_lines;
      int type      = (linePercept.lines[lp].type == LinePercept::C)?LinesTable::circle_lines:length|direction;

      /*
      const LinesTable::NamedPoint& p_mid = 
        (type&LinesTable::circle_lines)?
          getFieldInfo().fieldLinesTable.get_closest_point_direct(abs_mid, LinesTable::idx_circle):
          getFieldInfo().fieldLinesTable.get_closest_line_point_fast(abs_mid, length, direction);
          */
      // get the closest line in the world
      //LinesTable::NamedPoint p_begin = getFieldInfo().fieldLinesTable.get_closest_point(absPercept.begin(), type);
      //LinesTable::NamedPoint p_end = getFieldInfo().fieldLinesTable.get_closest_point(absPercept.end(), type);
      LinesTable::NamedPoint p_mid = getFieldInfo().fieldLinesTable.get_closest_point(abs_mid, type);

      // there is no such line
      if(p_mid.id == -1) {
        continue;
      }

      // get the line
      const Math::LineSegment& ref_line = getFieldInfo().fieldLinesTable.getLines()[p_mid.id];

      /*
      DEBUG_REQUEST("MCSL:draw_corner_votes",
        // vote for the corner id
        ASSERT(p_mid.id <= 29);
        lineVotes[p_mid.id]++;
        if(lineVotes[p_mid.id] > lineVotes[maxIdx]) 
          maxIdx = p_mid.id;
      );
      */
      // project the perceived line to the reference
      Vector2d p1 = ref_line.projection(abs_begin);
      Vector2d p2 = ref_line.projection(abs_end);
      Vector2d pm = p_mid.position;

      /*
      DEBUG_REQUEST("MCSL:draw_corner_votes",
        if(linePercept.lines[lp].type != LinePercept::C) {
          LINE(p1.x, p1.y, pm.x, pm.y);
          LINE(p2.x, p2.y, pm.x, pm.y);
      });
      */

      {
        Vector2d relP1(sample/p1);
        sample.likelihood *= computeDistanceWeight(relPercept.begin().abs(), relP1.abs(), cameraHeight, sigmaDistance);
        sample.likelihood *= computeAngleWeight(relPercept.begin().angle(), relP1.angle(), sigmaAngle);
      }
      {
        Vector2d relP2(sample/p2);
        sample.likelihood *= computeDistanceWeight(relPercept.end().abs(), relP2.abs(), cameraHeight, sigmaDistance);
        sample.likelihood *= computeAngleWeight(relPercept.end().angle(), relP2.angle(), sigmaAngle);
      }
      {
        Vector2d relPM(sample/pm);
        Vector2d relMidPoint = relPercept.point(0.5*relPercept.getLength());
        sample.likelihood *= computeDistanceWeight(relMidPoint.abs(), relPM.abs(), cameraHeight, sigmaDistance);
        sample.likelihood *= computeAngleWeight(relMidPoint.angle(), relPM.angle(), sigmaAngle);
      }
    }//end for

    /*
    DEBUG_REQUEST("MCSL:draw_corner_votes",
      FIELD_DRAWING_CONTEXT;

      PEN("0000FF", 10);
      for(int i = 0; i < 30; i++)
      {
        if(lineVotes[i] > 0)
        {
          Vector2<double> p = getFieldInfo().fieldLinesTable.getLines()[i].begin();
          Vector2<double> q = getFieldInfo().fieldLinesTable.getLines()[i].end();
          Vector2<double> r = (p+q)*0.5;
          LINE(p.x, p.y, q.x, q.y);
          TEXT_DRAWING(r.x+60, r.y+60, lineVotes[i]);
        }
      }//end for

      // the max vote
      PEN("FF0000", 20);
      Vector2<double> p = getFieldInfo().fieldLinesTable.getLines()[maxIdx].begin();
      Vector2<double> q = getFieldInfo().fieldLinesTable.getLines()[maxIdx].end();
      //Vector2<double> r = (p+q)*0.5;
      LINE(p.x, p.y, q.x, q.y);
    );
    */

  }//end for

  /*
  shortestLine = shortestLine;
  DEBUG_REQUEST("MCSL:plots",
    PLOT("MonteCarloSelfLocator:shortestLine", shortestLine);
  );
  */
}//end updateByLinesTable


void MonteCarloSelfLocator::updateByShortLines(const LinePercept& linePercept, SampleSet& sampleSet) const
{
  const double cameraHeight = getCameraMatrix().translation.z;
  const double sigmaDistance = parameters.goalPostSigmaDistance;
  const double sigmaAngle = parameters.goalPostSigmaAngle;
  double shortestLine = 1e+5; // very long...

  // todo: parameter for max lines to update by
  for(size_t lp=0; lp < linePercept.short_lines.size() && lp < 3; lp++)
  {
    //int idx = Math::random((int)linePercept.lines.size());
    // dont use the lines which are parts of the circle 
    // when the circle itself was detected
    //if(linePercept.lines[idx].type == LinePercept::C && linePercept.middleCircleWasSeen)
    //  continue;

    if(linePercept.short_lines[lp].lineOnField.getLength() < 300) { // don't use too short lines
      continue;
    }


    //const int centerLine_id = 4; // HACK: see FieldInfo
    // special treatment for the center line
    //if(linePercept.lines[lp].seen_id == LinePercept::center_line)
    //{
      // get the center line
      //const Math::LineSegment& centerLine = getFieldInfo().fieldLinesTable.getLines()[centerLine_id];
    //}//end if


    // TODO: separate class
    //int lineVotes[30] = {0};
    //int maxIdx = 0;

    const Math::LineSegment& relPercept = linePercept.short_lines[lp].lineOnField;

    for(size_t s=0; s < sampleSet.size(); s++)
    {
      Sample& sample = sampleSet[s];

      // statistics
      shortestLine = std::min(shortestLine, relPercept.getLength());

      // translocation of the line percept to the global coords
      Vector2d abs_begin      = sample*relPercept.begin();
      Vector2d abs_end        = sample*relPercept.end();
      Vector2d abs_direction  = abs_end - abs_begin;
      Vector2d abs_mid        = (abs_begin+abs_end)*0.5;

      // classify the line percept
      // todo: this may make problems when the lines are distored
      int length    = (relPercept.getLength() > 700)?LinesTable::long_lines:LinesTable::short_lines|LinesTable::circle_lines|LinesTable::long_lines;
      int direction = (fabs(abs_direction.x) > fabs(abs_direction.y))?LinesTable::along_lines:LinesTable::across_lines;
      int type      = (linePercept.short_lines[lp].type == LinePercept::C)?LinesTable::circle_lines:length|direction;

      /*
      const LinesTable::NamedPoint& p_mid = 
        (type&LinesTable::circle_lines)?
          getFieldInfo().fieldLinesTable.get_closest_point_direct(abs_mid, LinesTable::idx_circle):
          getFieldInfo().fieldLinesTable.get_closest_line_point_fast(abs_mid, length, direction);
          */
      // get the closest line in the world
      //LinesTable::NamedPoint p_begin = getFieldInfo().fieldLinesTable.get_closest_point(absPercept.begin(), type);
      //LinesTable::NamedPoint p_end = getFieldInfo().fieldLinesTable.get_closest_point(absPercept.end(), type);
      LinesTable::NamedPoint p_mid = getFieldInfo().fieldLinesTable.get_closest_point(abs_mid, type);

      // there is no such line
      if(p_mid.id == -1) {
        continue;
      }

      // get the line
      const Math::LineSegment& ref_line = getFieldInfo().fieldLinesTable.getLines()[p_mid.id];

      /*
      DEBUG_REQUEST("MCSL:draw_corner_votes",
        // vote for the corner id
        ASSERT(p_mid.id <= 29);
        lineVotes[p_mid.id]++;
        if(lineVotes[p_mid.id] > lineVotes[maxIdx]) 
          maxIdx = p_mid.id;
      );
      */
      // project the perceived line to the reference
      Vector2d p1 = ref_line.projection(abs_begin);
      Vector2d p2 = ref_line.projection(abs_end);
      Vector2d pm = p_mid.position;

      /*
      DEBUG_REQUEST("MCSL:draw_corner_votes",
        if(linePercept.lines[lp].type != LinePercept::C) {
          LINE(p1.x, p1.y, pm.x, pm.y);
          LINE(p2.x, p2.y, pm.x, pm.y);
      });
      */

      {
        Vector2d relP1(sample/p1);
        sample.likelihood *= computeDistanceWeight(relPercept.begin().abs(), relP1.abs(), cameraHeight, sigmaDistance);
        sample.likelihood *= computeAngleWeight(relPercept.begin().angle(), relP1.angle(), sigmaAngle);
      }
      {
        Vector2d relP2(sample/p2);
        sample.likelihood *= computeDistanceWeight(relPercept.end().abs(), relP2.abs(), cameraHeight, sigmaDistance);
        sample.likelihood *= computeAngleWeight(relPercept.end().angle(), relP2.angle(), sigmaAngle);
      }
      {
        Vector2d relPM(sample/pm);
        Vector2d relMidPoint = relPercept.point(0.5*relPercept.getLength());
        sample.likelihood *= computeDistanceWeight(relMidPoint.abs(), relPM.abs(), cameraHeight, sigmaDistance);
        sample.likelihood *= computeAngleWeight(relMidPoint.angle(), relPM.angle(), sigmaAngle);
      }
    }//end for

    /*
    DEBUG_REQUEST("MCSL:draw_corner_votes",
      FIELD_DRAWING_CONTEXT;

      PEN("0000FF", 10);
      for(int i = 0; i < 30; i++)
      {
        if(lineVotes[i] > 0)
        {
          Vector2<double> p = getFieldInfo().fieldLinesTable.getLines()[i].begin();
          Vector2<double> q = getFieldInfo().fieldLinesTable.getLines()[i].end();
          Vector2<double> r = (p+q)*0.5;
          LINE(p.x, p.y, q.x, q.y);
          TEXT_DRAWING(r.x+60, r.y+60, lineVotes[i]);
        }
      }//end for

      // the max vote
      PEN("FF0000", 20);
      Vector2<double> p = getFieldInfo().fieldLinesTable.getLines()[maxIdx].begin();
      Vector2<double> q = getFieldInfo().fieldLinesTable.getLines()[maxIdx].end();
      //Vector2<double> r = (p+q)*0.5;
      LINE(p.x, p.y, q.x, q.y);
    );
    */

  }//end for

  /*
  shortestLine = shortestLine;
  DEBUG_REQUEST("MCSL:plots",
    PLOT("MonteCarloSelfLocator:shortestLine", shortestLine);
  );
  */
}//end updateByLinesTable

void MonteCarloSelfLocator::updateByMiddleCircle(const LinePercept& linePercept, SampleSet& sampleSet) const
{
  if(!linePercept.middleCircleWasSeen) {
    return;
  }

  double sigmaDistance = parameters.sigmaDistanceCenterCircle;
  double sigmaAngle    = parameters.sigmaAngleCenterCircle;
  double cameraHeight  = getCameraMatrix().translation.z;

  Vector2<double> centerCirclePosition; // (0,0)

  for(size_t s=0; s < sampleSet.size(); s++)
  {
    Sample& sample = sampleSet[s];

    // translate the center circle to local coord 
    Vector2d localCircle = sample/centerCirclePosition;

    double expectedDistance = localCircle.abs();
    double expectedAngle = localCircle.angle();

    double seenDistance = linePercept.middleCircleCenter.abs();
    double seenAngle = linePercept.middleCircleCenter.angle();

    //double distanceDiff = fabs(expectedDistance - seenDistance);
    //double angleDiff = Math::normalize(seenAngle - expectedAngle);
    //sample.likelihood *= exp(-pow((distanceDiff)/sigmaDistance,2));
    //sample.likelihood *= exp(-pow((angleDiff)/sigmaAngle,2));

    sample.likelihood *= computeDistanceWeight(seenDistance, expectedDistance, cameraHeight, sigmaDistance);
    sample.likelihood *= computeAngleWeight(seenAngle, expectedAngle, sigmaAngle);
  }//end for

}//end updateByMiddleCircle

void MonteCarloSelfLocator::updateBySidePositions(SampleSet& sampleSet) const
{
  double offserY = 0;
  Vector2d startLeft(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yLength/2.0 - offserY);
  Vector2d endLeft(                               -500, getFieldInfo().yLength/2.0 - offserY);

  Vector2d startRight(startLeft.x, -startLeft.y);
  Vector2d endRight(endLeft.x, -endLeft.y);

  LineDensity leftStartingLine(startLeft, endLeft, -Math::pi_2, parameters.startPositionsSigmaDistance, parameters.startPositionsSigmaAngle);
  LineDensity rightStartingLine(startRight, endRight, Math::pi_2, parameters.startPositionsSigmaDistance, parameters.startPositionsSigmaAngle);

  for(size_t i = 0; i < sampleSet.size(); i++) {
    if(sampleSet[i].translation.y > 0) {
       sampleSet[i].likelihood *= leftStartingLine.update(sampleSet[i]);
    } else {
      sampleSet[i].likelihood *= rightStartingLine.update(sampleSet[i]);
    }
  }

  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    leftStartingLine.draw(getDebugDrawings());
    rightStartingLine.draw(getDebugDrawings());
  );
}

void MonteCarloSelfLocator::updateByStartPositions(SampleSet& sampleSet) const
{
  double offserY = 0;
  Vector2d startLeft(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yLength/2.0 - offserY);
  Vector2d endLeft(                               -500, getFieldInfo().yLength/2.0 - offserY);

  Vector2d startRight(startLeft.x, -startLeft.y);
  Vector2d endRight(endLeft.x, -endLeft.y);

  LineDensity leftStartingLine(startLeft, endLeft, -Math::pi_2, parameters.startPositionsSigmaDistance, parameters.startPositionsSigmaAngle);
  LineDensity rightStartingLine(startRight, endRight, Math::pi_2, parameters.startPositionsSigmaDistance, parameters.startPositionsSigmaAngle);

  //  for(size_t i = 0; i < sampleSet.size(); i++) {
  //    if(sampleSet[i].translation.y > 0) {
  //      sampleSet[i].likelihood *= leftStartingLine.update(sampleSet[i]);
  //    } else {
  //      sampleSet[i].likelihood *= rightStartingLine.update(sampleSet[i]);
  //    }
  //  }

  /*---- HACK BEGIN ----*/
  LineDensity startingLine;
  if(getPlayerInfo().playerNumber == 4 || getPlayerInfo().playerNumber == 6) {
      startingLine = leftStartingLine;
  } else {
      startingLine = rightStartingLine;
  }

  for(size_t i = 0; i < sampleSet.size(); i++) {
      sampleSet[i].likelihood *= startingLine.update(sampleSet[i]);
  }
  /*---- HACK END ----*/

  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    if(getPlayerInfo().playerNumber < 4)
    {
      leftStartingLine.draw(getDebugDrawings());
    }
    else
    {
      rightStartingLine.draw(getDebugDrawings());
    }    
  );
}

void MonteCarloSelfLocator::updateByOwnHalfLookingForward(SampleSet& sampleSet) const
{
  for(size_t s=0; s < sampleSet.size(); s++)
  {
    Sample& sample = sampleSet[s];

    if(!getFieldInfo().ownHalfRect.inside(sample.translation)) {
      sample.likelihood *= parameters.downWeightFactorOwnHalf;
    }

    double angleDiff = Math::normalize(sample.rotation - 0);
    sample.likelihood *=  Math::gaussianProbability(angleDiff, parameters.startPositionsSigmaAngle);
  }

  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    PEN("ff0000", 30);
    const Vector2d& fieldMin = getFieldInfo().ownHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().ownHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  );
}

void MonteCarloSelfLocator::updateByOwnHalf(SampleSet& sampleSet) const
{
  for(size_t s=0; s < sampleSet.size(); s++)
  {
    Sample& sample = sampleSet[s];

    if(!getFieldInfo().ownHalfRect.inside(sample.translation)) {
      sample.likelihood *= parameters.downWeightFactorOwnHalf;
    }
  }

  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = getFieldInfo().ownHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().ownHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  );
}

void MonteCarloSelfLocator::updateByOppHalf(SampleSet& sampleSet) const
{
  for(size_t s=0; s < sampleSet.size(); s++)
  {
    Sample& sample = sampleSet[s];

    if(!getFieldInfo().oppHalfRect.inside(sample.translation)) {
      sample.likelihood *= parameters.downWeightFactorOwnHalf;
    }
  }

  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = getFieldInfo().oppHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().oppHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  );
}

void MonteCarloSelfLocator::updateByGoalBox(SampleSet& sampleSet) const
{
  static const Geometry::Rect2d ownGoalBox(
    Vector2d(getFieldInfo().xPosOwnGroundline, getFieldInfo().yPosRightPenaltyArea) - Vector2d(200, 200), 
    Vector2d(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yPosLeftPenaltyArea) + Vector2d(200, 200));

  for(size_t s=0; s < sampleSet.size(); s++)
  {
    Sample& sample = sampleSet[s];

    if(!ownGoalBox.inside(sample.translation)) {
      sample.likelihood *= parameters.downWeightFactorOwnHalf;
    }

    double angleDiff = Math::normalize(sample.rotation - 0);
    sample.likelihood *=  Math::gaussianProbability(angleDiff, parameters.startPositionsSigmaAngle);
  }

  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = ownGoalBox.min();
    const Vector2d& fieldMax = ownGoalBox.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  );
}


void MonteCarloSelfLocator::updateByOldPose(SampleSet& sampleSet) const
{     
  double sigmaDistance = parameters.oldPoseSigmaDistance;
  //double sigmaAngle = parameters.oldPoseSigmaAngle;

  updateByPose(sampleSet, getRobotPose(), sigmaDistance, 0); // NOTE: rotation is not used
}

/*
void MonteCarloSelfLocator::updateByGoalModel(SampleSet& sampleSet) const
{
  double sigmaDistance = parameters.sigmaDistanceGoalModel;
  double sigmaAngle = parameters.sigmaAngleGoalModel;

  Pose2D pose = getSensingGoalModel().calculatePose(getCompassDirection(), getFieldInfo());

  updateByPose(sampleSet, pose, sigmaDistance, sigmaAngle);
}//end updateByGoalModel
*/

void MonteCarloSelfLocator::updateByPose(SampleSet& sampleSet, Pose2D pose, double sigmaDistance, double /*sigmaAngle*/) const
{
  for (size_t j = 0; j < sampleSet.size(); j++)
  {   
    double distDif = (sampleSet[j].translation - pose.translation).abs();
    //sampleSet[j].likelihood *= computeAngleWeighting(pose.rotation, sampleSet[j].rotation, sigmaAngle, bestPossibleAngleWeighting);
    sampleSet[j].likelihood *= Math::gaussianProbability(distDif,sigmaDistance); 
  }
}


void MonteCarloSelfLocator::updateStatistics(SampleSet& sampleSet)
{
  double cross_entropy = 0.0;
  double avg = 0.0;
  for (size_t i = 0; i < sampleSet.size(); i++) {
    avg += sampleSet[i].likelihood;
    cross_entropy -= log(sampleSet[i].likelihood);
  }
  avg /= (double)sampleSet.size();
  cross_entropy /= (double)sampleSet.size();
  PLOT("MonteCarloSelfLocator:w_average",avg);
  PLOT("MonteCarloSelfLocator:cross_entropy",avg);

  sampleSet.normalize();

  // effective number of particles
  static RingBufferWithSum<double, 30>  effective_number_of_samples_buffer;
  double sum2 = 0.0;
  for (size_t i = 0; i < sampleSet.size(); i++) {
    sum2 += Math::sqr(sampleSet[i].likelihood);
  }
  effective_number_of_samples = 1.0/sum2;
  effective_number_of_samples_buffer.add(effective_number_of_samples);
  PLOT("effective_number_of_samples", effective_number_of_samples);
  PLOT("effective_number_of_samples_buffer", effective_number_of_samples_buffer.getAverage());

  //1.0f - fastWeighting / slowWeighting
  double alphaSlow = 0.0059; //0.0059 0.006
  double alphaFast = 0.006;
  MODIFY("alphaSlow", alphaSlow);
  MODIFY("alphaFast", alphaFast);
  slowWeighting = slowWeighting + alphaSlow * (avg - slowWeighting);
  fastWeighting = fastWeighting + alphaFast * (avg - fastWeighting);
  PLOT("slowWeighting", slowWeighting);
  PLOT("fastWeighting", fastWeighting);

  const double resamplingPercentage = std::max(0.0, 1.0 - fastWeighting / slowWeighting);
  //const double numberOfResampledSamples = theSampleSet.size() * (1.0 - resamplingPercentage);
  PLOT("resamplingPercentage", resamplingPercentage);
}


void MonteCarloSelfLocator::resampleSimple(SampleSet& sampleSet, int number) const
{
  sampleSet.normalize();
  sampleSet.sort(false); // sort in the asscending order - worst are first ;)

  /*
  struct COMP {
    const SampleSet& sampleSet;

    COMP(const SampleSet& sampleSet) : 
      sampleSet(sampleSet) {
    }

    bool operator() (int i, int j) { 
      return sampleSet[i].likelihood < sampleSet[j].likelihood;
    }
  };

  std::vector<int> idx(sampleSet.size());
  for(size_t i = 0; i < idx.size(); i++) {
    idx[i] = i;
  }
  std::sort(idx.begin(), idx.end(), COMP(sampleSet));
  */

  double threshold = 1.0/(double)sampleSet.size();

  int k = 0;
  for(size_t i = 0; i < sampleSet.size() && i < 10; i++) {
    if(sampleSet[i].likelihood < threshold && k + number < (int)sampleSet.size()) {
      createRandomSample(getFieldInfo().carpetRect, sampleSet[i]); 
      k++;
    }
  }
}

//Metropolis Hastings
void MonteCarloSelfLocator::resampleMH(SampleSet& sampleSet)
{
  if(mhBackendSet.size() != sampleSet.size()) {
    mhBackendSet = sampleSet;
  }

  double radius = 200;
  MODIFY("resampleMH:radius", radius);

  double angle = 0.2;
  MODIFY("resampleMH:angle", angle);

  double threshold = 1.0/(double)sampleSet.size();
  MODIFY("resampleMH:threshold", threshold);

  double alpha = 0.1;
  MODIFY("resampleMH:alpha", alpha);

  sampleSet.normalize();
  //mhBackendSet.normalize();

  double backendSum = 0;

  for(size_t j = 0; j < mhBackendSet.size(); j++) 
  {
    // manage the backend set
    if(sampleSet[j].likelihood > mhBackendSet[j].likelihood) {
      mhBackendSet[j] = sampleSet[j]; // accept
    } else {
      mhBackendSet[j].likelihood = (1.0 - alpha)*mhBackendSet[j].likelihood + alpha*sampleSet[j].likelihood; // aging
      sampleSet[j] = mhBackendSet[j]; // reject
    }

    backendSum += mhBackendSet[j].likelihood;

    if(sampleSet[j].likelihood < threshold) { // new rendom particle
      createRandomSample(parameters.resetOwnHalf ? getFieldInfo().ownHalfRect : getFieldInfo().carpetRect, sampleSet[j]);
    } else { // just apply some noise
      applySimpleNoise(sampleSet[j], radius, angle);
    }
  }


  SampleSet oldSampleSet = sampleSet;
  oldSampleSet.normalize(parameters.resamplingThreshhold);

  double likelihood_step = 1.0/100.0; // the step in the weighting so we get exactly n particles
  double targetSum = Math::random()*likelihood_step;
  double currentSum = 0;

  // Stochastic universal sampling
  // i - count over the old sample set
  // j - over the new one :)
  size_t j = 0;
  for(size_t i = 0; i < oldSampleSet.size(); i++)
  {
    currentSum += oldSampleSet[i].likelihood;

    // select the particle to copy
    while(targetSum < currentSum && j < oldSampleSet.size())
    {
      sampleSet[j] = oldSampleSet[i];
      applySimpleNoise(sampleSet[j], radius, angle);
      targetSum += likelihood_step;
      j++;
    }
  }
}

int MonteCarloSelfLocator::resampleSUS(SampleSet& sampleSet, int n) const
{
  SampleSet oldSampleSet = sampleSet;
  oldSampleSet.normalize(parameters.resamplingThreshhold);

  //int n = 100; // number of samples to copy
  double likelihood_step = 1.0/n; // the step in the weighting so we get exactly n particles
  
  double targetSum = Math::random()*likelihood_step;
  double currentSum = 0;

  // Stochastic universal sampling
  // i - count over the old sample set
  // j - over the new one :)
  size_t j = 0;
  for(size_t i = 0; i < oldSampleSet.size(); i++)
  {
    currentSum += oldSampleSet[i].likelihood;

    // select the particle to copy
    while(targetSum < currentSum && j < oldSampleSet.size())
    {
      sampleSet[j] = oldSampleSet[i];
      targetSum += likelihood_step;

      if(updatedByGoalPosts) {
        applySimpleNoise(sampleSet[j], parameters.processNoiseDistance, parameters.processNoiseAngle);
      } else {
        applySimpleNoise(sampleSet[j], 0.0, parameters.processNoiseAngle);
      }

      j++;
    }
  }

  return (int)j;
}

void MonteCarloSelfLocator::resampleGT07(SampleSet& sampleSet, bool noise) const
{
  int numberOfPartiklesToResample = 0;

  SampleSet oldSampleSet = sampleSet;
  oldSampleSet.normalize(parameters.resamplingThreshhold);
  
  //oldSampleSet.sort();

  double sum = -Math::random();
  size_t count = 0;

  size_t m = 0;  // Zaehler durchs Ausgangs-Set
  size_t n = 0;  // Zaehler durchs Ziel-Set

  for(m = 0; m < sampleSet.size(); m++)
  {
    sum += oldSampleSet[m].likelihood * (double)oldSampleSet.size();

    // select the particle to copy
    while(static_cast<double>(count) < sum && count < oldSampleSet.size())
    {
      if (n >= oldSampleSet.size() - numberOfPartiklesToResample) break;
      
      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if(noise && updatedByGoalPosts) {
        applySimpleNoise(sampleSet[n], parameters.processNoiseDistance, parameters.processNoiseAngle);
      } else {
        applySimpleNoise(sampleSet[n], 0.0, parameters.processNoiseAngle);
      }
      
      n++;
      count++;
    }//end while

    if (n >= oldSampleSet.size()-numberOfPartiklesToResample) break;
  }//end for

  // fill up by copying random samples (shouldn't happen)
  while (n < sampleSet.size()) 
  {
    int i = Math::random((int)sampleSet.size());
    sampleSet[n++] = sampleSet[i];
  }
}


int MonteCarloSelfLocator::sensorResetBySensingGoalModel(SampleSet& sampleSet, int n) const
{
  // sensor resetting by whole goal
  if(!getSensingGoalModel().someGoalWasSeen)
//     !getSensingGoalModel().horizonScan ||
//     getSensingGoalModel().goal.calculateCenter().angle() > Math::fromDegrees(60)) // needed when th distortion is high
  {
    return n;
  }
    
  Pose2D pose = getSensingGoalModel().calculatePose(sampleSet[0].rotation, getFieldInfo());

  if(getFieldInfo().carpetRect.inside(pose.translation))
  {
    sampleSet[n].translation = pose.translation;
    sampleSet[n].rotation = pose.rotation;
    n++;

    DEBUG_REQUEST("MCSLS:draw_sensorResetBySensingGoalModel",
      FIELD_DRAWING_CONTEXT;
      PEN("0000FF", 20);
      ARROW(pose.translation.x, pose.translation.y, 
            pose.translation.x + 100*cos(pose.rotation), 
            pose.translation.y + 100*sin(pose.rotation));
    );
  } else {
    Pose2D poseMirrored(pose);
    poseMirrored.translation *= -1;
    poseMirrored.rotate(Math::pi);

    // HACK: try the mirrored pose
    if(getFieldInfo().carpetRect.inside(poseMirrored.translation))
    {
      sampleSet[n].translation = pose.translation;
      sampleSet[n].rotation = pose.rotation;
      n++;

      DEBUG_REQUEST("MCSLS:draw_sensorResetBySensingGoalModel",
        FIELD_DRAWING_CONTEXT;
        PEN("FF0000", 20);
        ARROW(poseMirrored.translation.x, poseMirrored.translation.y, 
              poseMirrored.translation.x + 100*cos(poseMirrored.rotation), 
              poseMirrored.translation.y + 100*sin(poseMirrored.rotation));
      );
    }
  }

  return n;
}//end sensorResetBySensingGoalModel


void MonteCarloSelfLocator::calculatePose(SampleSet& sampleSet)
{
  /************************************
   * STEP V: clustering
   ************************************/

  // try to track the hypothesis
  int clusterSize = canopyClustering.cluster(sampleSet, getRobotPose().translation);
  
  // Hypothesis tracking:
  // the idea is to keep the cluster until it has at lest 1/3 of all particles
  // if not, then jump only if there is anoter cluster having more then 2/3 particles

  // if the hypothesis is to small...
  if(clusterSize < 0.3*(double)sampleSet.size())
  {
    // make new cluseter
    canopyClustering.cluster(sampleSet);

    // find the largest cluster
    Moments2<2> tmpMoments;
    //Sample tmpPose = sampleSet.meanOfLargestCluster(tmpMoments);
    Sample tmpPose = sampleSet.meanOfCluster(tmpMoments, canopyClustering.getLargestClusterID());

    // TODO: make it more efficient
    // if it is not suficiently bigger revert the old clustering
    if(tmpMoments.getRawMoment(0,0) < 0.6*(double)sampleSet.size()) {
      canopyClustering.cluster(sampleSet, getRobotPose().translation);
    } else { // jump => change the state
      //state = LOCALIZE;
    }
  }//end if

  /************************************
   * STEP VI: estimate new position and update the model
   ************************************/

  // estimate the deviation of the pose
  Moments2<2> moments;
  //Sample newPose = sampleSet.meanOfLargestCluster(moments);
  Sample newPose = sampleSet.meanOfCluster(moments, canopyClustering.getLargestClusterID());

  getRobotPose() = newPose;

  moments.getAxes(
    getRobotPose().principleAxisMajor, 
    getRobotPose().principleAxisMinor);

  // TODO: find a beter place for it
  getRobotPose().isValid = (state == TRACKING);

  // update the goal model based on the robot pose
  getSelfLocGoalModel().update(getRobotPose(), getFieldInfo());

  DEBUG_REQUEST("MCSLS:draw_Cluster",
    FIELD_DRAWING_CONTEXT;
    sampleSet.drawCluster(getDebugDrawings(), newPose.cluster);
  );

  DEBUG_REQUEST("MCSLS:draw_position",
    drawPosition();
  );
}//end calculate pose


void MonteCarloSelfLocator::drawPosition() const
{
  FIELD_DRAWING_CONTEXT;
  if(getRobotPose().isValid)
  {
    switch( getPlayerInfo().teamColor )
    {
    case GameData::red:    PEN("FF0000", 20); break;
    case GameData::blue:   PEN("0000FF", 20); break;
    case GameData::yellow: PEN("FFFF00", 20); break;
    case GameData::black:  PEN("000000", 20); break;
    default: PEN("AAAAAA", 20); break;
    }
  }
  else
  {
    PEN("AAAAAA", 20);
  }

  ROBOT(getRobotPose().translation.x,
        getRobotPose().translation.y,
        getRobotPose().rotation);


  DEBUG_REQUEST("MCSLS:draw_deviation",
    PEN("000000", 20);

    LINE(getRobotPose().translation.x - getRobotPose().principleAxisMajor.x, 
         getRobotPose().translation.y - getRobotPose().principleAxisMajor.y,
         getRobotPose().translation.x + getRobotPose().principleAxisMajor.x, 
         getRobotPose().translation.y + getRobotPose().principleAxisMajor.y);

    LINE(getRobotPose().translation.x - getRobotPose().principleAxisMinor.x, 
         getRobotPose().translation.y - getRobotPose().principleAxisMinor.y,
         getRobotPose().translation.x + getRobotPose().principleAxisMinor.x, 
         getRobotPose().translation.y + getRobotPose().principleAxisMinor.y);

    OVAL_ROTATED(getRobotPose().translation.x, 
                 getRobotPose().translation.y, 
                 getRobotPose().principleAxisMinor.abs()*2.0,
                 getRobotPose().principleAxisMajor.abs()*2.0,
                 getRobotPose().principleAxisMinor.angle());
  );
}//end drawPosition

void MonteCarloSelfLocator::draw_sensor_belief() const
{
  static const int ySize = 20;
  static const int xSize = 30;
  double yWidth = 0.5*getFieldInfo().yFieldLength/(double)ySize;
  double xWidth = 0.5*getFieldInfo().xFieldLength/(double)xSize;

  FIELD_DRAWING_CONTEXT;
  Color white(1.0,1.0,1.0,0.0); // transparent
  Color black(0.0,0.0,0.0,1.0);

  double fixedRotation = theSampleSet.getMostLikelySample().rotation;

  // create new sample set
  SampleSet sampleSet(xSize*ySize);
  int idx = 0;

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      sampleSet[idx].translation = point;
      sampleSet[idx].rotation = fixedRotation;
      sampleSet[idx].likelihood = 1.0;
      idx++;
    }
  }

  updateBySensors(sampleSet);
  
  double maxValue = 0;
  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      maxValue = max(maxValue, sampleSet[idx++].likelihood);
    }
  }

  if(maxValue == 0) return;

  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = sampleSet[idx++].likelihood / maxValue;
      Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }
  }
}//end draw_closest_points
