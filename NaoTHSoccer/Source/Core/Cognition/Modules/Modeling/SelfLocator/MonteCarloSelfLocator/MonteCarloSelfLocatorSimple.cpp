/**
* @file MonteCarloSelfLocatorSimple.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MonteCarloSelfLocatorSimple
*/

#include "MonteCarloSelfLocatorSimple.h"

// debug
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

// tools
#include "Tools/Math/Probabilistics.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools.h"

using namespace std;
using namespace mcsl;

MonteCarloSelfLocatorSimple::MonteCarloSelfLocatorSimple() 
  :
  state(LOCALIZE),
  theSampleSet(100),
  canopyClustering(parameters.thresholdCanopy)
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

  // resampling
  DEBUG_REQUEST_REGISTER("MCSLS:resample_sus", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:resample_simple", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:resample_gt", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:resample_hm", "", false);

  // resulting position
  DEBUG_REQUEST_REGISTER("MCSLS:draw_Cluster", "draw the clustered particle set", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_position","draw robots position (self locator)", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_deviation", "", false);

  initializeSampleSet(getFieldInfo().carpetRect, theSampleSet);
}


void MonteCarloSelfLocatorSimple::execute()
{

  DEBUG_REQUEST("MCSLS:reset_samples",
    
    if(parameters.resetOwnHalf) {
      initializeSampleSet(getFieldInfo().ownHalfRect, theSampleSet);
    } else {
      initializeSampleSet(getFieldInfo().carpetRect, theSampleSet);
    }
    
    mhBackendSet = theSampleSet;
    state = LOCALIZE;

    DEBUG_REQUEST("MCSLS:draw_Samples", 
      theSampleSet.drawImportance();
    );
    return;
  );


  // (III) treat the situation when the robot has been lifted from the ground
  // (keednapped)
  bool motion_ok = getMotionStatus().currentMotion == motion::stand ||
                   getMotionStatus().currentMotion == motion::init;

  if( motion_ok && // only in stand or init(!)
      getBodyState().fall_down_state == BodyState::upright && parameters.treatLiftUp && (
     !getBodyState().standByLeftFoot && !getBodyState().standByRightFoot && // no foot is on the ground
      getFrameInfo().getTimeSince(getBodyState().foot_state_time) > parameters.maxTimeForLiftUp )) // we lose the ground contact for more then 1s
  {
    
    if(parameters.resetOwnHalf) {
      initializeSampleSet(getFieldInfo().ownHalfRect, theSampleSet);
    } else {
      initializeSampleSet(getFieldInfo().carpetRect, theSampleSet);
    }

    mhBackendSet = theSampleSet;
    state = LOCALIZE;

    DEBUG_REQUEST("MCSLS:draw_Samples", 
      theSampleSet.drawImportance();
    );
    DEBUG_REQUEST("MCSLS:draw_position",
      drawPosition();
    );
    return;
  }//end if

  if(parameters.treatInitState && getMotionStatus().currentMotion == motion::init) {
    DEBUG_REQUEST("MCSLS:draw_Samples", 
      theSampleSet.drawImportance();
    );
    DEBUG_REQUEST("MCSLS:draw_position",
      drawPosition();
    );
    return;
  }

  updateByOdometry(theSampleSet, parameters.motionNoise);
  lastRobotOdometry = getOdometryData();


  theSampleSet.resetLikelihood();
  updateBySensors(theSampleSet);
  
  if(state == LOCALIZE) {
    if(getGameData().gameState == GameData::set) {
      updateByOwnHalf(theSampleSet);
    } else {
      updateByStartPositions(theSampleSet);
    }
  }

  if(state == TRACKING) {
    if(parameters.updateByOldPose && 
       getGoalPercept().getNumberOfSeenPosts() == 0 // no goals seen
      )
    {
      updateByOldPose(theSampleSet);
    }
  }

  

  // NOTE: statistics has to bew after updates and before resampling
  updateStatistics(theSampleSet);

  DEBUG_REQUEST("MCSLS:draw_Samples",
    theSampleSet.drawImportance();
  );

  DEBUG_REQUEST("MCSLS:draw_BackendSamples",
    mhBackendSet.drawImportance(false);
  );


  if(state == LOCALIZE) 
  {
    // local optimization
    resampleMH(theSampleSet);

    // sensor resetting
    if(getSensingGoalModel().someGoalWasSeen) {
      sensorResetBySensingGoalModel(theSampleSet, theSampleSet.size()-1);
      resampleSUS(theSampleSet, theSampleSet.size());
    }

    // global search
    //resampleSimple(theSampleSet, (int)(effective_number_of_samples+0.5));

    canopyClustering.cluster(mhBackendSet);
    
    Moments2<2> tmpMoments;
    Sample tmpPose = mhBackendSet.meanOfLargestCluster(tmpMoments);

    if(tmpMoments.getRawMoment(0,0) > 0.8*mhBackendSet.size() &&
       (getGoalPercept().getNumberOfSeenPosts() > 0 || getGoalPerceptTop().getNumberOfSeenPosts() > 0)
      ) {
      theSampleSet = mhBackendSet;
      state = TRACKING;
    }

    DEBUG_REQUEST("MCSLS:draw_state",
      FIELD_DRAWING_CONTEXT;
      TEXT_DRAWING(0, 0, "LOCALIZE");
    );
    DEBUG_REQUEST("MCSLS:draw_Samples_effective",
      mhBackendSet.drawImportance(false);
    );
  }
  else if(state == TRACKING)
  {
    // sensor resetting
    //sensorResetBySensingGoalModel(theSampleSet, theSampleSet.size()-1);

    if(parameters.resampleSUS) {
      resampleSUS(theSampleSet, theSampleSet.size());
    }
    if(parameters.resampleGT07) {
      resampleGT07(theSampleSet, true);
    }

    calculatePose(theSampleSet);

    DEBUG_REQUEST("MCSLS:draw_state",
      FIELD_DRAWING_CONTEXT;
      TEXT_DRAWING(0, 0, "TRACKING");
    );
    DEBUG_REQUEST("MCSLS:draw_Samples_effective",
      theSampleSet.drawImportance();
    );
  }

  const double resamplingPercentage = std::max(0.0, 1.0 - fastWeighting / slowWeighting);
  //const double numberOfResampledSamples = theSampleSet.size() * (1.0 - resamplingPercentage);
  PLOT("resamplingPercentage", resamplingPercentage);
  

  DEBUG_REQUEST("MCSLS:resample_sus",
    /*nt n = */ resampleSUS(theSampleSet, theSampleSet.size()); //  (int)(effective_number_of_samples+0.5)
    sensorResetBySensingGoalModel(theSampleSet, theSampleSet.size()-1);
  );

  /************************************
   * STEP VII: execude some debug requests (drawings)
   ************************************/
  DEBUG_REQUEST("MCSLS:resample_gt", 
    resampleGT07(theSampleSet, true);
  );

  DEBUG_REQUEST("MCSLS:resample_hm", 
    resampleMH(theSampleSet);
  );
  
  DEBUG_REQUEST("MCSLS:resample_simple", 
    resampleSimple(theSampleSet, (int)(theSampleSet.size() - effective_number_of_samples+0.5));
  );

  DEBUG_REQUEST("MCSLS:draw_sensor_belief",
    draw_sensor_belief();
  );

}//end execute


void MonteCarloSelfLocatorSimple::updateByOdometry(SampleSet& sampleSet, bool noise) const
{
  Pose2D odometryDelta = getOdometryData() - lastRobotOdometry;
  for (unsigned int i = 0; i < sampleSet.size(); i++)
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


bool MonteCarloSelfLocatorSimple::updateBySensors(SampleSet& sampleSet) const
{
  if(parameters.updateByGoalPost)
  {
    if(getGoalPercept().getNumberOfSeenPosts() > 0) {
      updateByGoalPosts(getGoalPercept(), sampleSet);
    }
    if(getGoalPerceptTop().getNumberOfSeenPosts() > 0) {
      updateByGoalPosts(getGoalPerceptTop(), sampleSet);
    }
  }

  if(parameters.updateByCompas && getProbabilisticQuadCompas().isValid()) {
    updateByCompas(sampleSet);
  }

  if(parameters.updateByLinePoints)
  {
    if(getLineGraphPercept().edgels.size() > 0) {
      updateByLinePoints(getLineGraphPercept(), sampleSet);
    }
  }

  return true;
}

void MonteCarloSelfLocatorSimple::updateByGoalPosts(const GoalPercept& goalPercept, SampleSet& sampleSet) const
{
  for(int i = 0; i < goalPercept.getNumberOfSeenPosts(); i++)
  {
    const GoalPercept::GoalPost& seenPost = goalPercept.getPost(i);
    updateBySingleGoalPost(seenPost, sampleSet);
  }
}

void MonteCarloSelfLocatorSimple::updateBySingleGoalPost(const GoalPercept::GoalPost& seenPost, SampleSet& sampleSet) const
{
  const double sigmaAngle    = parameters.goalPostSigmaAngle;
  const double sigmaDistance = parameters.goalPostSigmaDistance;
  const double cameraHeight  = getCameraMatrix().translation.z;

  const double seenDistance = seenPost.position.abs();
  const double seenAngle = seenPost.position.angle();

  const Vector2d* leftGoalPosition = NULL;
  const Vector2d* rightGoalPosition = NULL;

  for (unsigned int j = 0; j < sampleSet.size(); j++)
  { 
    Sample& sample = sampleSet[j];
    Vector2d expectedPostPosition;

    // each particle decides for itself
    if(fabs(Math::normalize(sample.rotation + seenAngle)) < Math::pi_2)
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

void MonteCarloSelfLocatorSimple::updateByCompas(SampleSet& sampleSet) const
{
  for(unsigned int i = 0; i < sampleSet.size(); i++) {
    Sample& sample = sampleSet[i];
    sample.likelihood *= getProbabilisticQuadCompas().probability(-sample.rotation);
  }
}

void MonteCarloSelfLocatorSimple::updateByLinePoints(const LineGraphPercept& lineGraphPercept, SampleSet& sampleSet) const
{
  const double sigmaDistance = parameters.goalPostSigmaDistance;
  //const double sigmaAngle    = parameters.goalPostSigmaAngle;
  const double cameraHeight  = getCameraMatrix().translation.z;

  for(size_t i = 0; i < lineGraphPercept.edgels.size() && i < (size_t)parameters.linePointsMaxNumber; i++) 
  {
    const Vector2d& seen_point_relative = lineGraphPercept.edgels[i].point;

    for(unsigned int s=0; s < sampleSet.size(); s++)
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


void MonteCarloSelfLocatorSimple::updateByStartPositions(SampleSet& sampleSet) const
{
  double offserY = 0;
  Vector2d startLeft(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yLength/2.0 - offserY);
  Vector2d endLeft(                               -500, getFieldInfo().yLength/2.0 - offserY);

  Vector2d startRight(startLeft.x, -startLeft.y);
  Vector2d endRight(endLeft.x, -endLeft.y);

  LineDensity leftStartingLine(startLeft, endLeft, -Math::pi_2, 500, 0.1);
  LineDensity rightStartingLine(startRight, endRight, Math::pi_2, 500, 0.1);

  for(unsigned int i = 0; i < sampleSet.size(); i++) {
    if(sampleSet[i].translation.y > 0) {
      sampleSet[i].likelihood *= leftStartingLine.update(sampleSet[i]);
    } else {
      sampleSet[i].likelihood *= rightStartingLine.update(sampleSet[i]);
    }
  }

  DEBUG_REQUEST("MCSLS:draw_state",
    FIELD_DRAWING_CONTEXT;
    leftStartingLine.draw();
    rightStartingLine.draw();
  );
}

void MonteCarloSelfLocatorSimple::updateByOwnHalf(SampleSet& sampleSet) const
{
  for(unsigned int s=0; s < sampleSet.size(); s++)
  {
    Sample& sample = sampleSet[s];

    if(!getFieldInfo().ownHalfRect.inside(sample.translation)) {
      sample.likelihood *= parameters.downWeightFactorOwnHalf;
    }

    double angleDiff = Math::normalize(sample.rotation - 0);
    sample.likelihood *=  Math::gaussianProbability(angleDiff, 0.2);
  }
}


void MonteCarloSelfLocatorSimple::updateByOldPose(SampleSet& sampleSet) const
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

void MonteCarloSelfLocatorSimple::updateByPose(SampleSet& sampleSet, Pose2D pose, double sigmaDistance, double /*sigmaAngle*/) const
{
  for (unsigned int j = 0; j < sampleSet.size(); j++)
  {   
    double distDif = (sampleSet[j].translation - pose.translation).abs();
    //sampleSet[j].likelihood *= computeAngleWeighting(pose.rotation, sampleSet[j].rotation, sigmaAngle, bestPossibleAngleWeighting);
    sampleSet[j].likelihood *= Math::gaussianProbability(distDif,sigmaDistance); 
  }
}


void MonteCarloSelfLocatorSimple::updateStatistics(SampleSet& sampleSet)
{
  double cross_entropy = 0.0;
  double avg = 0.0;
  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    avg += sampleSet[i].likelihood;
    cross_entropy -= log(sampleSet[i].likelihood);
  }
  avg /= sampleSet.size();
  cross_entropy /= sampleSet.size();
  PLOT("MonteCarloSelfLocatorSimple:w_average",avg);
  PLOT("MonteCarloSelfLocatorSimple:cross_entropy",avg);

  sampleSet.normalize();

  // effective number of particles
  static RingBufferWithSum<double, 30>  effective_number_of_samples_buffer;
  double sum2 = 0.0;
  for (unsigned int i = 0; i < sampleSet.size(); i++) {
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
}


void MonteCarloSelfLocatorSimple::resampleSimple(SampleSet& sampleSet, int number) const
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

  double threshold = 1.0/sampleSet.size();

  int k = 0;
  for(unsigned int i = 0; i < sampleSet.size() && i < 10; i++) {
    if(sampleSet[i].likelihood < threshold && k + number < (int)sampleSet.size()) {
      createRandomSample(getFieldInfo().carpetRect, sampleSet[i]); 
      k++;
    }
  }
}

//Metropolis–Hastings
void MonteCarloSelfLocatorSimple::resampleMH(SampleSet& sampleSet)
{
  if(mhBackendSet.size() != sampleSet.size()) {
    mhBackendSet = sampleSet;
  }

  double radius = 200;
  MODIFY("resampleMH:radius", radius);
  double angle = 0.2;
  MODIFY("resampleMH:angle", angle);

  double threshold = 1.0/sampleSet.size();
  MODIFY("resampleMH:threshold", threshold);

  sampleSet.normalize();
  mhBackendSet.normalize();

  //int k = 0;
  for(unsigned int i = 0; i < sampleSet.size(); i++) {
    if(sampleSet[i].likelihood < mhBackendSet[i].likelihood && Math::random() > sampleSet[i].likelihood) {
      sampleSet[i] = mhBackendSet[i]; // reject
    } else {
      mhBackendSet[i] = sampleSet[i]; // accept
    }

    if(sampleSet[i].likelihood < threshold) {
      createRandomSample(parameters.resetOwnHalf ? getFieldInfo().ownHalfRect : getFieldInfo().carpetRect, sampleSet[i]);
    } else {
      applySimpleNoise(sampleSet[i], radius, angle);
    }
  }

  //mainSet = sampleSet;
}

int MonteCarloSelfLocatorSimple::resampleSUS(SampleSet& sampleSet, int n) const
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
  unsigned int j = 0;
  for(unsigned int i = 0; i < oldSampleSet.size(); i++)
  {
    currentSum += oldSampleSet[i].likelihood;

    // select the particle to copy
    while(targetSum < currentSum && j < oldSampleSet.size())
    {
      sampleSet[j] = oldSampleSet[i];
      targetSum += likelihood_step;

      if((getGoalPercept().getNumberOfSeenPosts() > 0 || getGoalPerceptTop().getNumberOfSeenPosts() > 0)) {
        applySimpleNoise(sampleSet[j], parameters.processNoiseDistance, parameters.processNoiseAngle);
      } else {
        applySimpleNoise(sampleSet[j], 0.0, parameters.processNoiseAngle);
      }

      j++;
    }
  }

  return j;
}

void MonteCarloSelfLocatorSimple::resampleGT07(SampleSet& sampleSet, bool noise) const
{
  int numberOfPartiklesToResample = 0;

  SampleSet oldSampleSet = sampleSet;
  oldSampleSet.normalize(parameters.resamplingThreshhold);
  
  //oldSampleSet.sort();

  double sum = -Math::random();
  unsigned int count = 0;

  unsigned int m = 0;  // Zaehler durchs Ausgangs-Set
  unsigned int n = 0;  // Zaehler durchs Ziel-Set

  for(m = 0; m < sampleSet.size(); m++)
  {
    sum += oldSampleSet[m].likelihood * oldSampleSet.size();

    // select the particle to copy
    while(count < sum && count < oldSampleSet.size())
    {
      if (n >= oldSampleSet.size() - numberOfPartiklesToResample) break;
      
      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if(noise && (getGoalPercept().getNumberOfSeenPosts() > 0 || getGoalPerceptTop().getNumberOfSeenPosts() > 0)) {
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
    int i = Math::random(sampleSet.size());
    sampleSet[n++] = sampleSet[i];
  }
}


int MonteCarloSelfLocatorSimple::sensorResetBySensingGoalModel(SampleSet& sampleSet, int n) const
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


void MonteCarloSelfLocatorSimple::calculatePose(SampleSet& sampleSet)
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
    Sample tmpPose = sampleSet.meanOfLargestCluster(tmpMoments);

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
  Sample newPose = sampleSet.meanOfLargestCluster(moments);

  getRobotPose() = newPose;

  moments.getAxes(
    getRobotPose().principleAxisMajor, 
    getRobotPose().principleAxisMinor);

  // TODO: find a beter place for it
  getRobotPose().isValid = (state == TRACKING);

  // update the goal model based on the robot pose
  getSelfLocGoalModel().update(getRobotPose(), getFieldInfo());

  DEBUG_REQUEST("MCSLS:draw_Cluster",
    sampleSet.drawCluster(newPose.cluster);
  );

  DEBUG_REQUEST("MCSLS:draw_position",
    drawPosition();
  );
}//end calculate pose


void MonteCarloSelfLocatorSimple::drawPosition() const
{
  FIELD_DRAWING_CONTEXT;
  if(getRobotPose().isValid)
  {
    switch( getPlayerInfo().gameData.teamColor )
    {
    case GameData::red:
      PEN("FF0000", 20);
      break;
    case GameData::blue:
      PEN("0000FF", 20);
      break;
    default:
      PEN("AAAAAA", 20);
      break;
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

  static Vector2<double> oldPose = getRobotPose().translation;
  if((oldPose - getRobotPose().translation).abs() > 100)
  {
    PLOT2D("MCSL:position_trace",getRobotPose().translation.x, getRobotPose().translation.y);
    oldPose = getRobotPose().translation;
  }
}//end drawPosition

void MonteCarloSelfLocatorSimple::draw_sensor_belief() const
{
  static const int ySize = 20;
  static const int xSize = 30;
  double yWidth = 0.5*getFieldInfo().yFieldLength/(double)ySize;
  double xWidth = 0.5*getFieldInfo().xFieldLength/(double)xSize;

  FIELD_DRAWING_CONTEXT;
  DebugDrawings::Color white(1.0,1.0,1.0,0.0); // transparent
  DebugDrawings::Color black(0.0,0.0,0.0,1.0);

  double fixedRotation = theSampleSet.getMostLikelySample().rotation;

  // create new sample set
  SampleSet sampleSet(xSize*ySize);
  int idx = 0;

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      sampleSet.samples[idx].translation = point;
      sampleSet.samples[idx].rotation = fixedRotation;
      sampleSet.samples[idx].likelihood = 1.0;
      idx++;
    }
  }

  updateBySensors(sampleSet);
  
  double maxValue = 0;
  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      maxValue = max(maxValue, sampleSet.samples[idx++].likelihood);
    }
  }

  if(maxValue == 0) return;

  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = sampleSet.samples[idx++].likelihood / maxValue;
      DebugDrawings::Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }
  }
}//end draw_closest_points