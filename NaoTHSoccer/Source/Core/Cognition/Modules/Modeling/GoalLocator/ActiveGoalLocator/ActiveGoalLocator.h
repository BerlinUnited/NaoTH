/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Declaration of class ActiveGoalLocator
 */

#ifndef _ActiveGoalLocator_h_
#define _ActiveGoalLocator_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/CameraMatrix.h" //for particlefilter
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/CompassDirection.h"
#include "Representations/Infrastructure/CameraInfo.h"

// Tools
#include <vector>
#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/CanopyClustering.h"
#include "Tools/CameraGeometry.h" //???


// local stuff
#include "AGLParameters.h"
#include "AGLSampleSet.h"
#include "PostParticleFilter.h"
#include "Assoziation.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ActiveGoalLocator)
  REQUIRE(GoalPercept)
  REQUIRE(CameraMatrix)
  REQUIRE(PlayerInfo)
  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(CompassDirection)
  REQUIRE(CameraInfo)

  PROVIDE(LocalGoalModel)
END_DECLARE_MODULE(ActiveGoalLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class ActiveGoalLocator : private ActiveGoalLocatorBase
{

public:
  ActiveGoalLocator();
  virtual ~ActiveGoalLocator(){}

  /** */
  virtual void execute();

private:

  /** */
  typedef PostParticleFilter PostHypothesis;


  /** */
  AGLParameters parameters;

  /** */
  OdometryData lastRobotOdometry;

  /** short term percept buffer */
  typedef RingBuffer<AGLBSample, 67> AGLSampleBuffer;
  AGLSampleBuffer theSampleBuffer;
  CanopyClustering<AGLSampleBuffer> ccSampleBuffer;
  
  /** */
  std::vector<PostHypothesis> postHypotheses;


private:

  void removeSamplesByFrameNumber(AGLSampleBuffer& sampleSet, const unsigned int maxFrames) const;

  /** odometry update */
  void updateByOdometry(AGLSampleBuffer& sampleSet, const Pose2D& odometryDelta) const;
  
  /** check if a new hypothesis may be creted */
  void checkTrashBuffer(AGLSampleBuffer& sampleBuffer);

  /** create a new hypothsis */
  void initFilterByBuffer(const int& largestClusterID, AGLSampleBuffer& sampleSetBuffer, AGLSampleSet& sampleSet);
  
  void estimateGoalModel();

  // experimental
  void calculateGoalModelAssosiations();

  /** debug visualization */
  void debugDrawings();
  void debugPlots();
  void debugStdOut();

  //Tools
  std::string convertIntToString(int number);

};

#endif //_ActiveGoalLocator_h_
