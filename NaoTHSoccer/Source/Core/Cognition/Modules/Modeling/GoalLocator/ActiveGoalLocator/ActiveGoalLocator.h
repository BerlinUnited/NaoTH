/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Declaration of class ActiveGoalLocatorSimpleParticle
 */

#ifndef __ActiveGoalLocator_h_
#define __ActiveGoalLocator_h_

#include <ModuleFramework/Module.h>
#include "AGLParameters.h"
#include "AGLSampleSet.h"
#include "AGLSampleBuffer.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/CompassDirection.h"

// Tools
#include <vector>
#include "Tools/Math/Geometry.h"
#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/CanopyClustering.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ActiveGoalLocator)
  REQUIRE(GoalPercept)
  REQUIRE(PlayerInfo)
  REQUIRE(BodyState)
  REQUIRE(CameraMatrix)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(CompassDirection)

  PROVIDE(LocalGoalModel)
END_DECLARE_MODULE(ActiveGoalLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class ActiveGoalLocator : private ActiveGoalLocatorBase
{

public:
  ActiveGoalLocator();
  ~ActiveGoalLocator(){}

  virtual void execute();

private:

  double goalWidth;

  double averageWeighting;

  AGLParameters parameters;


  CanopyClustering<AGLSampleBuffer> ccTrashBuffer;
  AGLSampleBuffer theSampleBuffer;


  OdometryData lastRobotOdometry;

  //needs to check the need for using new percept
  // if high, no need for insert new percept
  double timeFilter;

  class Cluster
  {
  public:
      Cluster():
          canopyClustering(sampleSet){}

      CanopyClustering<AGLSampleSet> canopyClustering;
      AGLSampleSet sampleSet;
  };

  Cluster ccSamples[10];

  void debugDrawings();

  void resampleGT07(AGLSampleSet& sampleSet, bool noise);

  void checkTrashBuffer(AGLSampleBuffer& sampleBuffer);
  void updateByOdometry(AGLSampleSet& sampleSet, const Pose2D& odometryDelta) const;
  void updateByOdometry(AGLSampleBuffer& sampleSet, const Pose2D& odometryDelta) const;
  void updateByFrameNumber(AGLSampleBuffer& sampleSet, const unsigned int frames) const;
  double getWeightingOfPerceptAngle(const AGLSampleSet& sampleSet, const GoalPercept::GoalPost& post);
  void initFilterByBuffer(const int& largestClusterID, AGLSampleBuffer& sampleSetBuffer, AGLSampleSet& sampleSet);
  void updateByGoalPerceptAngle(AGLSampleSet& sampleSet, const GoalPercept::GoalPost& post);
};

#endif //__ActiveGoalLocator_h_
