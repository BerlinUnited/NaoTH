/**
* @file Walk.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#ifndef _Walk2018_H_
#define _Walk2018_H_

#include "../IKMotion.h"
#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include <Representations/Infrastructure/JointData.h>

#include "Representations/Infrastructure/CalibrationData.h"
#include <Representations/Infrastructure/GyrometerData.h>
#include "Representations/Modeling/InertialModel.h"
#include <Representations/Infrastructure/InertialSensorData.h>

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/CommandBuffer.h"

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Representations/Debug/Stopwatch.h"

// tools
#include <queue>
#include "CoMErrorProvider.h"
#include "FootStepPlanner2018.h"
#include "ZMPPlanner2018.h"
#include "../IKPose.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(Walk2018)
  PROVIDE(DebugModify)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)

  REQUIRE(FrameInfo)
  REQUIRE(RobotInfo)
  REQUIRE(InverseKinematicsMotionEngineService)
  REQUIRE(MotionRequest)

  REQUIRE(InertialSensorData)
  REQUIRE(IMUData)
  REQUIRE(CalibrationData)
  REQUIRE(InertialModel)
  REQUIRE(GyrometerData)
  REQUIRE(KinematicChainSensor)

  PROVIDE(MotionLock)
  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)


  // PROVIDE AND REQUIRE
  PROVIDE(StepBuffer)
  PROVIDE(CommandBuffer)

END_DECLARE_MODULE(Walk2018)

class Walk2018: private Walk2018Base, public IKMotion, public ModuleManager
{
public:
  Walk2018();
  
  virtual void execute();

private:
  ModuleCreator<CoMErrorProvider>*    theCoMErrorProvider;
  ModuleCreator<FootStepPlanner2018>* theFootStepPlanner;
  ModuleCreator<ZMPPlanner2018>*      theZMPPlanner;

  InverseKinematic::CoMFeetPose theCoMFeetPose;

  const IKParameters::Walk& parameters() const {
    return getEngine().getParameters().walk;
  }

private:
  void planZMP();
  void executeStep();

  Pose3D calculateLiftingFootPos(const Step& step) const;



  void updateMotionStatus(MotionStatus& motionStatus) const;

private: // stabilization
  void feetStabilize(const Step& executingStep, double (&position)[naoth::JointData::numOfJoint]) const;
};

#endif // _Walk2018_H_
