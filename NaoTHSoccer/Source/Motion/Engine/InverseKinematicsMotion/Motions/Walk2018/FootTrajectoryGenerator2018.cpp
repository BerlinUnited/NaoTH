/**
 * @File:   FootTrajectoryGenerator.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#include "FootTrajectoryGenerator2018.h"
#include "Tools/Math/CubicSpline.h"

using namespace std;

void FootTrajectoryGenerator2018::execute(){
    const Step& executingStep = getStepBuffer().first();

    switch(executingStep.footStep.liftingFoot())
    {
      case FootStep::LEFT:
      {
        getTargetCoMFeetPose().pose.feet.left = calculateLiftingFootPos(executingStep);
        getTargetCoMFeetPose().pose.feet.right = executingStep.footStep.supFoot();
        break;
      }
      case FootStep::RIGHT:
      {
        getTargetCoMFeetPose().pose.feet.left = executingStep.footStep.supFoot();
        getTargetCoMFeetPose().pose.feet.right = calculateLiftingFootPos(executingStep);
        break;
      }
      case FootStep::NONE:
      {
        getTargetCoMFeetPose().pose.feet = executingStep.footStep.begin();
        break;
      }
      default: ASSERT(false);
    }

    PLOT("Walk:trajectory:x",getTargetCoMFeetPose().pose.feet.left.translation.x);
    PLOT("Walk:trajectory:y",getTargetCoMFeetPose().pose.feet.left.translation.y);
    PLOT("Walk:trajectory:z",getTargetCoMFeetPose().pose.feet.left.translation.z);
}

Pose3D FootTrajectoryGenerator2018::calculateLiftingFootPos(const Step& step) const
{
  if ( step.type == Step::STEP_CONTROL && step.walkRequest.stepControl.type == WalkRequest::StepControlRequest::KICKSTEP)
  {
    return stepControl(
      step.footStep.footBegin(),
      step.footStep.footEnd(),
      step.executingCycle,
      step.samplesDoubleSupport,
      step.samplesSingleSupport,
      parameters.kickHeight,
      0, //footPitchOffset
      0, //footRollOffset
      step.walkRequest.stepControl.speedDirection,
      step.walkRequest.stepControl.scale);
  }
  else
  {
    if(parameters.useSplineFootTrajectory)
    {
      return genTrajectoryWithSplines(
              step.footStep.footBegin(),
              step.footStep.footEnd(),
              step.executingCycle,
              step.samplesSingleSupport,
              parameters.stepHeight,
              0, // footPitchOffset
              0  // footRollOffset
            );
    }
    else
    {
      return genTrajectory(
              step.footStep.footBegin(),
              step.footStep.footEnd(),
              step.executingCycle,
              step.samplesDoubleSupport,
              step.samplesSingleSupport,
              parameters.stepHeight,
              0, // footPitchOffset
              0  // footRollOffset
      );
    }
  }

  ASSERT(false);
  return Pose3D(); // is never reached
}

Pose3D FootTrajectoryGenerator2018::genTrajectory(
  const Pose3D& oldFoot, 
  const Pose3D& targetFoot,
  double cycle, 

  double samplesDoubleSupport, 
  double samplesSingleSupport, 
  double stepHeight, 
  double footPitchOffset,
  double footRollOffset
 ) const
{
  double doubleSupportEnd = samplesDoubleSupport / 2;
  double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;

  if (cycle <= doubleSupportEnd)
  {
    return oldFoot;
  }
  else if (cycle <= doubleSupportBegin)
  {
    double t = 1 - (doubleSupportBegin - cycle) / samplesSingleSupport;

    // parameter for the step curve
    // NOTE: xp is used to interpolate the motion in x/y-plane, while
    //       yp is for the mition in the z-axis
    double xp = (1 - cos(t*Math::pi))*0.5;
    double yp = (1 - cos(t*Math::pi2))*0.5;

    Pose3D foot;
    foot.translation.z = targetFoot.translation.z + yp*stepHeight;
    foot.translation.x = (1.0 - xp) * oldFoot.translation.x + xp * targetFoot.translation.x;
    foot.translation.y = (1.0 - xp) * oldFoot.translation.y + xp * targetFoot.translation.y;

    foot.rotation = RotationMatrix::getRotationX(footRollOffset * yp);
    foot.rotation.rotateY(Math::sgn(targetFoot.translation.x - oldFoot.translation.x) * footPitchOffset * yp);
    RotationMatrix rot = RotationMatrix::interpolate(oldFoot.rotation, targetFoot.rotation, xp);
    foot.rotation *= rot;

    return foot;
  }
  else
  {
    return targetFoot;
  }
}

Pose3D FootTrajectoryGenerator2018::stepControl(
  const Pose3D& oldFoot, 
  const Pose3D& targetFoot,
  double cycle, 

  double samplesDoubleSupport, 
  double samplesSingleSupport, 
  double stepHeight, 
  double footPitchOffset, 
  double footRollOffset,
  double speedDirection,
  double scale
  ) const
{
  double doubleSupportEnd = samplesDoubleSupport / 2;
  double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;

  if (cycle <= doubleSupportEnd)
  {
    return oldFoot;
  }
  else if (cycle <= doubleSupportBegin)
  {
    double t = 1 - (doubleSupportBegin - cycle) / samplesSingleSupport;
    //double xp = (1 - cos(t*Math::pi))*0.5;
    double xp = (t < scale) ? (1-cos(t/scale*Math::pi))*0.5 : 1.0;
    double zp = (1 - cos(t*Math::pi2))*0.5;

    // TODO: optmize
    // TODO: use tk::spline?
    Pose3D speedTarget = targetFoot;
    speedTarget.translate(cos(speedDirection) * 30, sin(speedDirection) * 30, 0);
    vector<Vector2d > vecX;
    vecX.push_back(Vector2d(0.0, oldFoot.translation.x));
    vecX.push_back(Vector2d(1.0, targetFoot.translation.x));
    vecX.push_back(Vector2d(1.1, speedTarget.translation.x));
    CubicSpline theCubicSplineX(vecX);

    vector<Vector2d > vecY;
    vecY.push_back(Vector2d(0.0, oldFoot.translation.y));
    vecY.push_back(Vector2d(1.0, targetFoot.translation.y));
    vecY.push_back(Vector2d(1.1, speedTarget.translation.y));
    CubicSpline theCubicSplineY(vecY);

    Pose3D foot;
    foot.translation.z = targetFoot.translation.z + zp*stepHeight;

    foot.translation.x = theCubicSplineX.y(xp);
    foot.translation.y = theCubicSplineY.y(xp);

    foot.rotation = RotationMatrix::getRotationX(footRollOffset * zp);
    foot.rotation.rotateY(Math::sgn(targetFoot.translation.x - oldFoot.translation.x) * footPitchOffset * zp);
    RotationMatrix rot = RotationMatrix::interpolate(oldFoot.rotation, targetFoot.rotation, xp);
    foot.rotation *= rot;

    return foot;
  }
  else
  {
    return targetFoot;
  }
}

Pose3D FootTrajectoryGenerator2018::genTrajectoryWithSplines(
  const Pose3D& oldFoot,
  const Pose3D& targetFoot,
  double cycle,

  double duration,
  double stepHeight,
  double footPitchOffset,
  double footRollOffset
 ) const
{
    double t = cycle/duration;

    double xyp = theCubicSplineXY(t);
    double zp  = theCubicSplineZ(t);

    Pose3D foot;
    foot.translation.z = targetFoot.translation.z + zp*stepHeight;
    foot.translation.x = (1.0 - xyp) * oldFoot.translation.x + xyp * targetFoot.translation.x;
    foot.translation.y = (1.0 - xyp) * oldFoot.translation.y + xyp * targetFoot.translation.y;

    foot.rotation = RotationMatrix::getRotationX(footRollOffset * zp);
    foot.rotation.rotateY(Math::sgn(targetFoot.translation.x - oldFoot.translation.x) * footPitchOffset * zp);
    RotationMatrix rot = RotationMatrix::interpolate(oldFoot.rotation, targetFoot.rotation, xyp);
    foot.rotation *= rot;

    return foot;
}
