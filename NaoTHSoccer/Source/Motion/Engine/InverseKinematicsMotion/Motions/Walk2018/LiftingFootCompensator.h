/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef _LIFTING_FOOT_COMPENSATOR_H
#define _LIFTING_FOOT_COMPENSATOR_H

#include <ModuleFramework/Module.h>

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetCoMFeetPose.h"
#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(LiftingFootCompensator)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)

    REQUIRE(FrameInfo)
    REQUIRE(InverseKinematicsMotionEngineService)
    REQUIRE(StepBuffer)

    PROVIDE(TargetCoMFeetPose)  // modifying com heigth
END_DECLARE_MODULE(LiftingFootCompensator)

class LiftingFootCompensator : private LiftingFootCompensatorBase
{
  public:
    LiftingFootCompensator(){}

  virtual void execute(){
    InverseKinematic::CoMFeetPose tmp(getTargetCoMFeetPose().pose);
    if(getStepBuffer().first().footStep.liftingFoot() == FootStep::LEFT) {
      tmp.localInRightFoot();
      getTargetCoMFeetPose().pose.com.translation.z += getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.hip.comHeightOffset * tmp.feet.left.translation.z;
      getTargetCoMFeetPose().pose.com.translation.y -= getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.hip.comStepOffsetY  * tmp.feet.left.translation.z;
      //getTargetCoMFeetPose().pose.com.rotateX( parameters().hip.comRotationOffsetX *tmp.feet.left.translation.z );

      PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
    }
    else if(getStepBuffer().first().footStep.liftingFoot() == FootStep::RIGHT)
    {
      tmp.localInLeftFoot();
      getTargetCoMFeetPose().pose.com.translation.z += getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.hip.comHeightOffset * tmp.feet.right.translation.z;
      getTargetCoMFeetPose().pose.com.translation.y += getInverseKinematicsMotionEngineService().getEngine().getParameters().walk.hip.comStepOffsetY * tmp.feet.left.translation.z;
      //getTargetCoMFeetPose().pose.com.rotateX( -parameters().hip.comRotationOffsetX*tmp.feet.right.translation.z );

      PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
    }
  }
};

#endif  /* _LIFTING_FOOT_COMPENSATOR_H */
