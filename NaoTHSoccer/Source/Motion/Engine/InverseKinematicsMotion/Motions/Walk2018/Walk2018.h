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

#include "Representations/Motion/Walk2018/CoMErrors.h"
#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/ZMPReferenceBuffer.h"
#include "Representations/Motion/Walk2018/CommandBuffer.h"
#include "Representations/Motion/Walk2018/TargetCoMFeetPose.h"
#include "Representations/Motion/Walk2018/TargetHipFeetPose.h"

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
#include "ZMPPreviewController.h"
#include "FootTrajectoryGenerator2018.h"
#include "HipRotationOffsetModifier.h"
#include "LiftingFootCompensator.h"
#include "TorsoRotationStabilizer.h"
#include "FeetStabilizer.h"
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

  REQUIRE(Walk2018Parameters)

  PROVIDE(MotionLock)
  PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)

  // PROVIDE AND REQUIRE
  PROVIDE(StepBuffer)         // reason: resetting after walk is finished
  PROVIDE(ZMPReferenceBuffer) // reason: resetting after walk is finished
  PROVIDE(CommandBuffer)      // reason: write final CoMFeetPose into buffer for CoMErrorProvider and resetting after walk is finished
  PROVIDE(CoMErrors)          // reason: resetting after walk is finished
  PROVIDE(TargetCoMFeetPose)
  PROVIDE(TargetHipFeetPose)
END_DECLARE_MODULE(Walk2018)

class Walk2018: private Walk2018Base, public IKMotion, public ModuleManager
{
public:
  Walk2018();
  //TODO: destructor...?
  //zurzeit werden die representation des walks beim initialisieren aufgeraeumt. vielleicht waere es besser dies im destructor zu tun
  
  virtual void execute();

private:
  ModuleCreator<CoMErrorProvider>*            theCoMErrorProvider;
  ModuleCreator<FootStepPlanner2018>*         theFootStepPlanner;
  ModuleCreator<ZMPPlanner2018>*              theZMPPlanner;
  ModuleCreator<ZMPPreviewController>*        theZMPPreviewController;
  ModuleCreator<FootTrajectoryGenerator2018>* theFootTrajectoryGenerator;
  ModuleCreator<HipRotationOffsetModifier>*   theHipRotationOffsetModifier;
  ModuleCreator<LiftingFootCompensator>*      theLiftingFootCompensator;
  ModuleCreator<TorsoRotationStabilizer>*     theTorsoRotationStabilizer;
  ModuleCreator<FeetStabilizer>*              theFeetStabilizer;

  const GeneralParameters& parameters;

private:
  void calculateTargetCoMFeetPose();
  void calculateJoints();
  void updateMotionStatus(MotionStatus& motionStatus) const;

private:
  // TODO: check if really required and correctly used in updateMotionStatus
  //       Options: deletion or moving into a helper header
  //       some similar function is used by ZMPPlanner2018
  Pose3D calculateStableCoMByFeet(InverseKinematic::FeetPose feet, double pitch) const
  {
    if(getWalk2018Parameters().zmpPlanner2018Params.newZMP_ON) {
      feet.left.translate(getWalk2018Parameters().zmpPlanner2018Params.bezierZMP.offsetX, 0, 0);
      feet.right.translate(getWalk2018Parameters().zmpPlanner2018Params.bezierZMP.offsetX, 0, 0);
    } else {
      feet.left.translate(getWalk2018Parameters().zmpPlanner2018Params.simpleZMP.offsetX, 0, 0);
      feet.right.translate(getWalk2018Parameters().zmpPlanner2018Params.simpleZMP.offsetX, 0, 0);
    }

    Pose3D com;
    com.rotation = calculateBodyRotation(feet, pitch);
    com.translation = (feet.left.translation + feet.right.translation) * 0.5;
    com.translation.z = getWalk2018Parameters().zmpPlanner2018Params.comHeight;

    return com;
  }

  RotationMatrix calculateBodyRotation(const InverseKinematic::FeetPose& feet, double pitch) const
  {
    double bodyAngleZ = Math::meanAngle(feet.left.rotation.getZAngle(), feet.right.rotation.getZAngle());
    return RotationMatrix::getRotationZ(bodyAngleZ) * RotationMatrix::getRotationY(pitch);
  }
};

#endif // _Walk2018_H_
