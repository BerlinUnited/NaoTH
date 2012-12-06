/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Declaration of class ActiveGoalLocatorSimpleParticle
 */

#ifndef _ActiveGoalLocator_h_
#define _ActiveGoalLocator_h_

#include <ModuleFramework/Module.h>

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

// local stuff
#include "AGLParameters.h"
#include "AGLSampleSet.h"

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
  virtual ~ActiveGoalLocator(){}

  /** */
  virtual void execute();

private:

  /** */
  class PostHypothesis
  {
  public:
      PostHypothesis(){}

      AGLSampleSet sampleSet;

      /*
      CLUSTERING NOT USED YET
      CanopyClustering<AGLSampleSet> canopyClustering;

      // wrapper for easy use
      void cluster()
      {
        canopyClustering.cluster(sampleSet);
      }

      unsigned int cluster(const Vector2<double>& start)
      {
        return canopyClustering.cluster(sampleSet, start);
      }
      */
  };


  /** */
  AGLParameters parameters;

  /** */
  OdometryData lastRobotOdometry;

  /** percept buffer */
  typedef RingBuffer<AGLBSample, 67> AGLSampleBuffer;
  AGLSampleBuffer theSampleBuffer;
  CanopyClustering<AGLSampleBuffer> ccTrashBuffer;
  
  /** */
  std::vector<PostHypothesis> ccSamples;


private:

  /** update the buffer */
  void updateByFrameNumber(AGLSampleBuffer& sampleSet, const unsigned int frames) const;



  /** odometry update */
  void updateByOdometry(AGLSampleSet& sampleSet, const Pose2D& odometryDelta) const;
  void updateByOdometry(AGLSampleBuffer& sampleSet, const Pose2D& odometryDelta) const;

  /** sensor update */
  double getWeightingOfPerceptAngle(const AGLSampleSet& sampleSet, const GoalPercept::GoalPost& post);
  void updateByGoalPerceptAngle(AGLSampleSet& sampleSet, const GoalPercept::GoalPost& post);

  /** resamping */
  void resampleGT07(AGLSampleSet& sampleSet, bool noise);

  
  
  /** check if a new hypothesis may be creted */
  void checkTrashBuffer(AGLSampleBuffer& sampleBuffer);

  /** create a new hypothsis */
  void initFilterByBuffer(const int& largestClusterID, AGLSampleBuffer& sampleSetBuffer, AGLSampleSet& sampleSet);
  
  

  /** debug visualization */
  void debugDrawings();
  void debugPlots();
  void debugStdOut();


  //Tools
  string convertIntToString(int number);

};

#endif //_ActiveGoalLocator_h_
