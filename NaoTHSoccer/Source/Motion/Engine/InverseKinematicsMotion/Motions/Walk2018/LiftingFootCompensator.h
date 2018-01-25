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
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(LiftingFootCompensator)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)

    REQUIRE(Walk2018Parameters)

    REQUIRE(FrameInfo)
    REQUIRE(StepBuffer)

    PROVIDE(TargetCoMFeetPose)  // modifying com translation
END_DECLARE_MODULE(LiftingFootCompensator)

class LiftingFootCompensator : private LiftingFootCompensatorBase
{
  public:
    LiftingFootCompensator() : parameters(getWalk2018Parameters().liftingFootCompensatorParams) {}

  virtual void execute(){
    InverseKinematic::CoMFeetPose tmp(getTargetCoMFeetPose().pose);
    switch(getStepBuffer().first().footStep.liftingFoot()) {
      case FootStep::LEFT :
        tmp.localInRightFoot();
        getTargetCoMFeetPose().pose.com.translation.z += parameters.comHeightOffset * tmp.feet.left.translation.z;
        getTargetCoMFeetPose().pose.com.translation.y -= parameters.comStepOffsetY  * tmp.feet.left.translation.z;
        break;
      case FootStep::RIGHT :
        tmp.localInLeftFoot();
        getTargetCoMFeetPose().pose.com.translation.z += parameters.comHeightOffset * tmp.feet.right.translation.z;
        getTargetCoMFeetPose().pose.com.translation.y += parameters.comStepOffsetY  * tmp.feet.right.translation.z;
        break;
      case FootStep::NONE : break;
    }

    PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
  }

  LiftingFootCompensatorParameters parameters;
};

#endif  /* _LIFTING_FOOT_COMPENSATOR_H */
