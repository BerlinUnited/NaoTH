/**
* @file NaoQiMotion.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Kaden, Steffen</a>
*
* the stand motion goes from current pose to the default stand pose.
* running - this motion is cyclic and there is allways running when the motion
*           request is set to stand
* stopped - the motion stopps when the motion request is not stand and the 
*           default stand pose has been reached
* 
*/

#ifndef _NAO_QI_MOTION_H
#define _NAO_QI_MOTION_H

#include "IKMotion.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Modeling/GroundContactModel.h"
#include "Representations/Modeling/InertialModel.h"
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/CalibrationData.h>
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include "Representations/Infrastructure/BDRNaoQi.h"

#include <Representations/Modeling/BodyState.h>

//tools
#include "Tools/JointMonitor.h"
#include "Tools/JointOffsets.h"
#include "Tools/StiffnessController.h"
#include "Tools/DataStructures/Spline.h"

BEGIN_DECLARE_MODULE(NaoQiMotion)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)

  REQUIRE(FrameInfo)

  REQUIRE(RobotInfo)
  REQUIRE(MotionRequest)
  //REQUIRE(GroundContactModel)
  REQUIRE(InertialModel)
  REQUIRE(InertialSensorData)
  REQUIRE(GyrometerData)
  REQUIRE(CalibrationData)
  //REQUIRE(KinematicChainSensor)
  REQUIRE(MotionStatus)
  //PROVIDE(SensorJointData)

  PROVIDE(BDRNaoQiStatus)// just for testing will be REQUIRE

  PROVIDE(BDRNaoQiRequest)

  REQUIRE(InverseKinematicsMotionEngineService)

  // body state
  //REQUIRE(BodyState)

  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(NaoQiMotion)

class NaoQiMotion : private NaoQiMotionBase, public IKMotion
{
public:
  enum State {
    startingNaoQiMotion,
    runningNaoQiMotion,
    stoppingNaoQiMotion,
    goToStand
  };

  State state;
  State lastState;

public:

  NaoQiMotion();
  ~NaoQiMotion(){}

  virtual void execute();

private:
  void calcStandPose();

  bool interpolateToPose();

  void applyPose(const InverseKinematic::CoMFeetPose& p);

private:
  tk::spline interpolationSpline;

  double totalTime;
  double time;
  double height;
  bool standardStand;
  
  InverseKinematic::CoMFeetPose targetPose;
  InverseKinematic::CoMFeetPose startPose;
};

#endif  /* _NAO_QI_MOTION_H */

