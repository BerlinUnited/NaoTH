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

// tools
#include "Tools/Math/Probabilistics.h"
#include "Tools.h"

using namespace std;
using namespace mcsl;

MonteCarloSelfLocatorSimple::MonteCarloSelfLocatorSimple() 
  :
  canopyClustering(parameters.thresholdCanopy)
{
  // debug
  DEBUG_REQUEST_REGISTER("MCSLS:reset_samples", "reset the sample set", false);

  // fiald drawings
  DEBUG_REQUEST_REGISTER("MCSLS:draw_Samples", "draw sample set before resampling", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_post_choice", "", false);
  DEBUG_REQUEST_REGISTER("MCSLS:draw_sensor_belief", "", false);


  initializeSampleSet(getFieldInfo().carpetRect, theSampleSet);
}


void MonteCarloSelfLocatorSimple::execute()
{

  DEBUG_REQUEST("MCSLS:reset_samples",
    initializeSampleSet(getFieldInfo().carpetRect, theSampleSet);

    DEBUG_REQUEST("MCSLS:draw_Samples", 
      theSampleSet.drawImportance();
    );
    return;
  );

  updateByOdometry(theSampleSet, parameters.motionNoise);
  lastRobotOdometry = getOdometryData();

  theSampleSet.resetLikelihood();
  updateBySensors(theSampleSet);
  //resampleSimple(theSampleSet);
  resampleGT07(theSampleSet, true);

  DEBUG_REQUEST("MCSLS:draw_Samples", 
    theSampleSet.drawImportance();
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

    if(noise) {
      applySimpleNoise(sampleSet[i], parameters.motionNoiseDistance, parameters.motionNoiseAngle);
    }
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
  }//end update by goal posts

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
      PEN("00000099",1);
      LINE(sample.translation.x, sample.translation.y, expectedPostPosition.x, expectedPostPosition.y);
    );

    if(seenPost.positionReliable) {
      sample.likelihood *= computeDistanceWeight(seenDistance, expectedDistance, cameraHeight, sigmaDistance);
    }
    sample.likelihood *= computeAngleWeight(seenAngle, expectedAngle, sigmaAngle);

  } //end for j (samples)
}

void MonteCarloSelfLocatorSimple::resampleSimple(SampleSet& sampleSet, int number) const
{
  //sampleSet.normalize();
  sampleSet.sort();
  for(int i = 0; i < number; i++) {
    createRandomSample(getFieldInfo().carpetRect, sampleSet[i]);
  }
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
      if(noise) {
        applySimpleNoise(sampleSet[n], parameters.processNoiseDistance, parameters.processNoiseAngle);
      }
      
      n++;
      count++;
    }//end while

    if (n >= oldSampleSet.size()-numberOfPartiklesToResample) break;
  }//end for

  // generate some random samples
  for(; n < oldSampleSet.size(); n++) {
    createRandomSample(getFieldInfo().carpetRect, sampleSet[n]);
  }

  // fill up by copying random samples (shouldn't happen)
  while (n < sampleSet.size()) 
  {
    int i = Math::random(sampleSet.size());
    sampleSet[n++] = sampleSet[i];
  }
}


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
    }//end for
  }//end for

  updateBySensors(sampleSet);
  
  double maxValue = 0;
  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      maxValue = max(maxValue, sampleSet.samples[idx++].likelihood);
    }//end for
  }//end for

  if(maxValue == 0) return;

  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2<double> point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = sampleSet.samples[idx++].likelihood / maxValue;
      DebugDrawings::Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }//end for
  }//end for
}//end draw_closest_points