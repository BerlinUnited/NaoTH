/**
* @file ParticleFilterBallLocator.h
*
* @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
* Declaration of class ParticleFilterBallLocator
*/

#ifndef __ParticleFilterBallLocator_h_
#define __ParticleFilterBallLocator_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/CameraMatrix.h"

#include "Representations/Modeling/BallModel.h"

#include "PFBLParameters.h"
#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/RingBuffer.h"
#include <vector>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(ParticleFilterBallLocator)
  REQUIRE(BallPercept)
  REQUIRE(BodyState)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(OdometryData)
  REQUIRE(MotionStatus)
  REQUIRE(KinematicChain)
  REQUIRE(CameraMatrix)

  PROVIDE(BallModel)
END_DECLARE_MODULE(ParticleFilterBallLocator)


//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class ParticleFilterBallLocator : private ParticleFilterBallLocatorBase
{

public:
  ParticleFilterBallLocator();
  virtual ~ParticleFilterBallLocator(){}

  virtual void execute();


private:

  class Sample
  {
  public:
    Sample() : likelihood(0), moving(false){}
    Vector2<double> position;
    Vector2<double> speed;
    double likelihood;
    bool moving;
  };

  RingBuffer<BallPercept, 10> perceptBuffer;
  typedef std::vector<Sample> SampleSet;

  SampleSet theSampleSet;
  Pose2D lastRobotOdometry;
  naoth::FrameInfo lastFrameInfo;
  PFBLParameters parameters;

  void updateByBallPercept(SampleSet& sampleSet);
  void motionUpdate(SampleSet& sampleSet, bool noise);

  void resampleGT07(SampleSet& sampleSet);
  Sample generateNewSample();

  /** */
  double computeAngleWeighting(
                                double measuredAngle, 
                                double expectedAngle,
                                double standardDeviation, 
                                double bestPossibleWeighting = 1.0) const;

  /** */
  double computeDistanceWeighting(
                                double measuredDistance, 
                                double expectedDistance,
                                double cameraZ,
                                double standardDeviation, 
                                double bestPossibleWeighting = 1.0) const;

  void updatePreviewModel();

  /** some debug drawings */
  void drawBallModel(const BallModel& ballModel) const;
  void drawSamples(const SampleSet& sampleSet) const;
  void drawMotionUpdateBySpeed(const Vector2<double> position, const Vector2<double> positionWithSpeed) const;

};

#endif //__ParticleFilterBallLocator_h_


