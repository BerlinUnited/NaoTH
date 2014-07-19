/**
* @file Walk.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#ifndef _IK_MOTION_H_
#define _IK_MOTION_H_

#include "IKMotion.h"
#include "Walk/FootStep.h"
#include "Walk/FootStepPlanner.h"
#include <list>

#include "Tools/DataStructures/RingBufferWithSum.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Modeling/GroundContactModel.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Modeling/KinematicChain.h"
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/SupportPolygon.h"
#include <Representations/Infrastructure/JointData.h>
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Infrastructure/CalibrationData.h"

BEGIN_DECLARE_MODULE(Walk)
  REQUIRE(RobotInfo)
  REQUIRE(GroundContactModel)
  REQUIRE(MotionRequest)
  REQUIRE(KinematicChainSensor)
  REQUIRE(KinematicChainMotor)
  REQUIRE(GyrometerData)
  REQUIRE(InertialModel)
  REQUIRE(InertialSensorData)
  REQUIRE(SupportPolygon)
  REQUIRE(SensorJointData)
  REQUIRE(CalibrationData)

  REQUIRE(InverseKinematicsMotionEngineService)
  
  PROVIDE(MotionLock)
  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(Walk)

class Walk: private WalkBase, public IKMotion
{
public:
  Walk();
  
  /** */
  void execute();

private:
  /** class describing a single step */
  struct Step 
  {
    Step():
      planningCycle(0),
      executingCycle(0),
      lifted(false),
      character(-1),
      bodyPitchOffset(0.0),
      samplesDoubleSupport(0),
      samplesSingleSupport(0),
      extendDoubleSupport(0),
      numberOfCyclePerFootStep(0),
      stepControlling(false),
      speedDirection(0),
      scale(1.0)
    {}

    int planningCycle;
    int executingCycle;
    bool lifted;
    FootStep footStep;
    double character;
    
    // parameters
    // calcualted parameters of walk
    double bodyPitchOffset;
    int samplesDoubleSupport;
    int samplesSingleSupport;
    int extendDoubleSupport;
    int numberOfCyclePerFootStep;

    // only for step control
    bool stepControlling;
    double speedDirection;
    double scale;
  };

  /** */
  void feetStabilize(double (&position)[naoth::JointData::numOfJoint]);

  /** */
  bool FSRProtection();
  
  /** */
  bool waitLanding();
  
  /** */
  void plan(const MotionRequest& motionRequest);
  
  /** */
  void walk(const WalkRequest& req);
  
  /** */
  void stopWalking();

  /** */
  void stopWalkingWithoutStand();
  
  /** */
  bool canStop() const;
  
  /** */
  void updateParameters(Step& step, double character) const;

  /** calculate the COM error */
  void calculateError();

  /** */
  void manageSteps(const WalkRequest& req);

  /** */
  void planStep();

  /** */
  InverseKinematic::CoMFeetPose executeStep();

  /** */
  RotationMatrix calculateBodyRotation(const InverseKinematic::FeetPose& feet, double pitch) const;

  /** */
  void updateMotionStatus(MotionStatus& motionStatus);
  
  /** */
  Pose3D calculateStableCoMByFeet(InverseKinematic::FeetPose feet, double pitch) const;

  /** */
  void addStep(const Step& step);

  /** */
  void adaptStepSize(FootStep& step) const;


private:
  // TODO: does it have to be static?
  static unsigned int theStepID; // use for step control

  const IKParameters::Walk& theWalkParameters;
  
  InverseKinematic::CoMFeetPose theCoMFeetPose;
  
  int theWaitLandingCount;
  int theUnsupportedCount;
  
  bool isStopping;
  bool stoppingStepFinished;
  WalkRequest stoppingRequest;
  
  std::list<Step> stepBuffer;
  
  FootStepPlanner theFootStepPlanner;


  // observe the com error
  RingBufferWithSum<double, 100> com_errors;
  Vector3d currentComError;
  RingBufferWithSum<Vector3d, 100> currentComErrorBuffer;

  //
  RingBufferWithSum<Vector2<double>, 100> corrections;


  // a buffer of CoMFeetPoses requested in the past
  // needed by stabilization
  RingBuffer<InverseKinematic::CoMFeetPose, 10> commandPoseBuffer;
  RingBuffer<FootStep::Foot, 10> commandFootIdBuffer;

};

#endif // _IK_MOTION_H_
