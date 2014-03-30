/**
* @file MonteCarloSelfLocatorSimple.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class MonteCarloSelfLocatorSimple
*/

#ifndef _MonteCarloSelfLocatorSimple_h_
#define _MonteCarloSelfLocatorSimple_h_

#include <ModuleFramework/Module.h>

#include "Tools/Math/Geometry.h"
#include "Tools/CameraGeometry.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

// representations
// basic info
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"

// motion / kinematics
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Perception/CameraMatrix.h"

// situation
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/SituationStatus.h"

// sensor percepts
#include "Representations/Perception/GoalPercept.h"

// local models
#include "Representations/Modeling/ProbabilisticQuadCompas.h"

// this are the results :)
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"

// basic tools
#include "SampleSet.h"
#include "CanopyClustering.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(MonteCarloSelfLocatorSimple)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo) // only for visualization of the pose

  REQUIRE(OdometryData)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(MotionStatus)
  REQUIRE(BodyState)
  REQUIRE(SituationStatus)

  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)

  REQUIRE(SensingGoalModel)
  REQUIRE(ProbabilisticQuadCompas)

  PROVIDE(RobotPose)
  PROVIDE(SelfLocGoalModel)
END_DECLARE_MODULE(MonteCarloSelfLocatorSimple)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class MonteCarloSelfLocatorSimple : private MonteCarloSelfLocatorSimpleBase
{
public:
  MonteCarloSelfLocatorSimple();
  virtual ~MonteCarloSelfLocatorSimple(){}

  void execute();

private: // local types

  enum State {
    LOCALIZE,
    TRACKING
  } state;

  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("MCSLSParameters")
    {
      PARAMETER_REGISTER(thresholdCanopy) = 900;
      PARAMETER_REGISTER(resamplingThreshhold) = 0.01;

      PARAMETER_REGISTER(processNoiseDistance) = 70;
      PARAMETER_REGISTER(processNoiseAngle) = 0.1;

      PARAMETER_REGISTER(motionNoise) = false;
      PARAMETER_REGISTER(motionNoiseDistance) = 5.0;
      PARAMETER_REGISTER(motionNoiseAngle) = 0.01;

      PARAMETER_REGISTER(updateByGoalPost) = true;
      PARAMETER_REGISTER(goalPostSigmaDistance) = 0.1;
      PARAMETER_REGISTER(goalPostSigmaAngle) = 0.1;

      PARAMETER_REGISTER(updateByCompas) = true;

      // load from the file after registering all parameters
      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    double thresholdCanopy;
    double resamplingThreshhold;

    double processNoiseDistance;
    double processNoiseAngle;

    bool motionNoise;
    double motionNoiseDistance;
    double motionNoiseAngle;

    bool updateByGoalPost;
    double goalPostSigmaDistance;
    double goalPostSigmaAngle;

    bool updateByCompas;
    
    virtual ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }
  } parameters;

private: // data members
  OdometryData lastRobotOdometry;
  SampleSet theSampleSet;
  SampleSet setBeforeResampling; // sort of 'double buffering'
  CanopyClustering<SampleSet> canopyClustering;

private: // workers
  void updateByOdometry(SampleSet& sampleSet, bool noise) const;

  bool updateBySensors(SampleSet& sampleSet) const;
  void updateByGoalPosts(const GoalPercept& goalPercept, SampleSet& sampleSet) const;
  void updateBySingleGoalPost(const GoalPercept::GoalPost& goalPost, SampleSet& sampleSet) const;
  void updateByCompas(SampleSet& sampleSet) const;

  void resampleSimple(SampleSet& sampleSet, int number) const;
  void resampleGT07(SampleSet& sampleSet, bool noise) const;
  void resampleCool(SampleSet& sampleSet, int n) const;

  int sensorResetBySensingGoalModel(SampleSet& sampleSet, int n) const;

  void calculatePose(SampleSet& sampleSet);

private: //debug
  void draw_sensor_belief() const;
  void drawPosition() const;
};

#endif //_MonteCarloSelfLocatorSimple_h_
