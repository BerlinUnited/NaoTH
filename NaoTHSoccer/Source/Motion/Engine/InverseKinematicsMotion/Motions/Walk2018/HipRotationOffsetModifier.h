/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef _HIP_ROTATION_OFFSET_MODIFIER_H
#define _HIP_ROTATION_OFFSET_MODIFIER_H

#include <ModuleFramework/Module.h>
#include "Tools/Math/Pose3D.h"

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetCoMFeetPose.h"
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"

#include "Tools/DataStructures/Spline.h"

BEGIN_DECLARE_MODULE(HipRotationOffsetModifier)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)

    REQUIRE(Walk2018Parameters)

    REQUIRE(FrameInfo)
    REQUIRE(StepBuffer)

    PROVIDE(TargetCoMFeetPose)  // modifying com rotation
END_DECLARE_MODULE(HipRotationOffsetModifier)

class HipRotationOffsetModifier : private HipRotationOffsetModifierBase
{
  public:
    HipRotationOffsetModifier():
        lastStepLength(),
        hipRotationOffsetBasedOnStepChange(),
        parameters(getWalk2018Parameters().hipRotationOffsetModifierParams) {}

  virtual void execute(){
    // apply rotation offset depending on step change
    const Step& executingStep = getStepBuffer().first();

    // new step, determine hip rotation offset based on step change for this step
    if(executingStep.executingCycle == 0){
        InverseKinematic::FeetPose begin = executingStep.footStep.begin();
        InverseKinematic::FeetPose end   = executingStep.footStep.end();
        Pose3D startFoot, targetFoot;
        if(executingStep.footStep.liftingFoot() == FootStep::LEFT){
                begin.localInRightFoot();
                end.localInRightFoot();
                startFoot  = begin.left;
                targetFoot = end.left;
        } else {
                begin.localInLeftFoot();
                end.localInLeftFoot();
                startFoot  = begin.right;
                targetFoot = end.right;
        }

        Vector2d currentStepLength = targetFoot.projectXY().translation - startFoot.projectXY().translation;
        Vector2d currentStepChange = currentStepLength - lastStepLength;

        hipRotationOffsetBasedOnStepChange = Vector2d(parameters.hipOffsetBasedOnStepChange.x * currentStepChange.x,
                                                      parameters.hipOffsetBasedOnStepChange.y * currentStepChange.y);

        PLOT("Walk:currentStepChange.x", currentStepChange.x);
        PLOT("Walk:currentStepChange.y", currentStepChange.y);

        // remember for next step
        lastStepLength = currentStepLength;
    }

    // TODO: check this
    // TODO: apply hipRotationOffset smoothly
    PLOT("Walk::hipRotationOffsetBasedOnStepChange.x",hipRotationOffsetBasedOnStepChange.x);

    std::vector<double> xA = {0.0, 0.5, 1.0};
    std::vector<double> yA = {0.0, 1.0, 0.0};

    tk::spline scaleOffset;
    scaleOffset.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
    scaleOffset.set_points(xA,yA);

    double t = executingStep.executingCycle/executingStep.numberOfCycles;
    getTargetCoMFeetPose().pose.com.rotation = calculateBodyRotation(getTargetCoMFeetPose().pose.feet, scaleOffset(t)*Math::fromDegrees(hipRotationOffsetBasedOnStepChange.x) + parameters.bodyPitchOffset);
  }


  void reset() {
    lastStepLength = Vector2d();
  }

  private:
    Vector2d lastStepLength;
    Vector2d hipRotationOffsetBasedOnStepChange;

    RotationMatrix calculateBodyRotation(const InverseKinematic::FeetPose& feet, double pitch) const
    {
      double bodyAngleZ = Math::meanAngle(feet.left.rotation.getZAngle(), feet.right.rotation.getZAngle());
      // TODO: check, shouldn't it be RotationMatrix::getRotationY(pitch) * RotationMatrix::getRotationZ(bodyAngleZ)?
      //       i.e. rotation first about some "global" z axis and then about the rotated y (pitch) axis
      return RotationMatrix::getRotationZ(bodyAngleZ) * RotationMatrix::getRotationY(pitch);
    }

  public:
    const HipRotationOffsetModifierParameters& parameters;
};


#endif  /* _HIP_ROTATION_OFFSET_MODIFIER_H */
