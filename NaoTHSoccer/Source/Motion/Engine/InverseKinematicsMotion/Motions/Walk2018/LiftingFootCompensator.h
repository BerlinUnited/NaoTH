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

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(LiftingFootCompensator)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)
    PROVIDE(DebugParameterList)

    REQUIRE(FrameInfo)
    REQUIRE(StepBuffer)

    PROVIDE(TargetCoMFeetPose)  // modifying com translation
END_DECLARE_MODULE(LiftingFootCompensator)

class LiftingFootCompensator : private LiftingFootCompensatorBase
{
  public:
    LiftingFootCompensator(){
      getDebugParameterList().add(&parameters);
    }

    virtual ~LiftingFootCompensator(){
      getDebugParameterList().remove(&parameters);
    }

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
    }

    PLOT("Walk:theCoMFeetPose:total_rotationY",tmp.com.rotation.getYAngle());
  }

  private:
    class Parameters: public ParameterList{
      public:
        Parameters() : ParameterList("LiftingFootCompensator")
        {
          PARAMETER_REGISTER(comHeightOffset) = 0.18;
          PARAMETER_REGISTER(comStepOffsetY)  = 0;

          syncWithConfig();
        }

        double comHeightOffset;
        double comStepOffsetY;
    } parameters;
};

#endif  /* _LIFTING_FOOT_COMPENSATOR_H */
