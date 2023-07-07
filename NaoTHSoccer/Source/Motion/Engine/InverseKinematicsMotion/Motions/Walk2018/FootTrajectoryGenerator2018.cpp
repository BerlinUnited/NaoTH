/**
 * @File:   FootTrajectoryGenerator.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#include "FootTrajectoryGenerator2018.h"

using namespace std;

void FootTrajectoryGenerator2018::execute()
{
  const Step& executingStep = getStepBuffer().first();

  switch(executingStep.footStep.liftingFoot())
  {
    case FootStep::LEFT:
    {
      getTargetCoMFeetPose().pose.feet.left  = calculateLiftingFootPos(executingStep);
      getTargetCoMFeetPose().pose.feet.right = executingStep.footStep.supFoot();
      break;
    }
    case FootStep::RIGHT:
    {
      getTargetCoMFeetPose().pose.feet.left  = executingStep.footStep.supFoot();
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
    if (parameters.useSplineFootTrajectoryForSideKicks) //  && step.walkRequest.stepControl.speedDirection != 0)
    {
      //std::cout << "Do the kick step - spline" << std::endl;
      return stepControlNew(
        step.footStep,
        step.executingCycle,
        step.samplesDoubleSupport,
        step.samplesSingleSupport,
        parameters.kickHeight,
        0, //footPitchOffset
        0, //footRollOffset
        step.walkRequest.stepControl.speedDirection,
        step.walkRequest.stepControl.scale,
        parameters.sideKickWidth
        );
    }
    else // forward kick or useSplineFootTrajectoryForSideKicks == false
    {
      //std::cout << "Do the kick step - sin/cos" << std::endl;
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
  }
  else //  (step.type == WALKSTEP (legacy)) or (stepControl.type == WALKSTEP or ZEROSTEP)
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

Pose3D FootTrajectoryGenerator2018::stepControlNew(
    const FootStep& step,
    double cycle,
    double samplesDoubleSupport,
    double samplesSingleSupport,
    double stepHeight,
    double footPitchOffset,
    double footRollOffset,
    double speedDirection,
    double scale,
    double sidekick_width
    ) const
{
    double doubleSupportEnd = samplesDoubleSupport / 2;
    double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;

    if (cycle <= doubleSupportEnd)
    {
        return step.footBegin();
    }
    else if (cycle <= doubleSupportBegin)
    {
        Pose3D /*supFoot,*/ startFoot, targetFoot;

        // poses in support foot coordinates
        if (step.liftingFoot() == FootStep::LEFT)
        {
            InverseKinematic::FeetPose begin = step.begin();
            InverseKinematic::FeetPose end = step.end();

            begin.localInRightFoot();
            end.localInRightFoot();

            //supFoot    = begin.right;
            startFoot = begin.left;
            targetFoot = end.left;
        }
        else
        {
            InverseKinematic::FeetPose begin = step.begin();
            InverseKinematic::FeetPose end = step.end();

            begin.localInLeftFoot();
            end.localInLeftFoot();

            //supFoot    = begin.left;
            startFoot = begin.right;
            targetFoot = end.right;
        }


        // ------- Gewaltkick -------------
        // Experimental: requires startFoot == targetFoot
        // Additional parameters:
        // 
        // bezierZMP.offsetYForKicks=-30
        // gewaltKickLength=50
        // forwardKickTime=1000
        // 
        // X trajectory
        //std::vector<double> t_X = { 0.0, 0.125,  0.35,  0.5, 1.0 };
        //std::vector<double> f_X = { 0.0,   0.0, -0.4,   1.1, 0.0 };

        
        // ------- Short Stepkick -------------
        // simpleZMP.kickOffsetY = -20
        // forwardKickTime = 340
        // limits.maxCtrlLength = 80
        // 
        // X trajectory
        std::vector<double> t_X = { 0.0,  0.25, 1.0 };
        std::vector<double> f_X = { 0.0, -0.3,  1.0 };

        tk::spline theCubicSplineX;
        theCubicSplineX.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
        theCubicSplineX.set_points(t_X, f_X);

        // NOTE: no sidekicks are supported for now
        // Y trajectory
        //std::vector<double> t_Y = { 0.0, 0.25, 0.5, 0.75, 1.0 };
        //std::vector<double> f_Y = { 0.0, 0.5,  1.0, 0.5,  0.0 };
        //
        //tk::spline theCubicSplineY;
        //theCubicSplineY.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
        //theCubicSplineY.set_points(t_Y, f_Y);

        // Z trajectory
        std::vector<double> t_Z = { 0.0, 0.125, 0.35,  0.5, 0.65,  0.875, 1.0 };
        std::vector<double> f_Z = { 0.0, 0.275, 0.775, 1.0, 0.775, 0.275, 0.0 };

        tk::spline theCubicSplineZ;
        theCubicSplineZ.set_boundary(tk::spline::first_deriv, 2.0, tk::spline::first_deriv, -8.0, false);
        theCubicSplineZ.set_points(t_Z, f_Z);

        // time in the single support phase: [0,1]
        double t = 1.0 - (doubleSupportBegin - cycle) / samplesSingleSupport;

        // scale x/y trajectors velocity by scaling the time
        //double t_xy_scaled = (t < scale) ? (1 - cos(t / scale*Math::pi))*0.5 : 1.0;
        
        // time scale: controlling the speed profile of the trajectory
        std::vector<double> t_T = { 0.0, 0.5, 1.0 };
        std::vector<double> f_T = { 0.0, 0.5, 1.0 };

        tk::spline theCubicSplineT;
        theCubicSplineT.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
        theCubicSplineT.set_points(t_T, f_T);

        // calculate the scaled time
        double t_xy_scaled = theCubicSplineT(t);

        double s_Xt = theCubicSplineX(t_xy_scaled);
        //double s_Yt = theCubicSplineY(t_xy_scaled);
        double s_Zt = theCubicSplineZ(t);

        // clculate the next position of the foot by interpolating between startFoot and targetFoot
        Pose3D foot;
        // Experimental: preparation for the Gewaltkick
        //foot.translation.x = (1 - s_Xt) * startFoot.translation.x + s_Xt * (targetFoot.translation.x + parameters.gewaltKickLength);
        foot.translation.x = (1 - s_Xt) * startFoot.translation.x + s_Xt * targetFoot.translation.x;
        foot.translation.y = (1 - s_Xt) * startFoot.translation.y + s_Xt * targetFoot.translation.y;

        // LEGACY: sidekicks
        //foot.translation.y = (1 - t_xy_scaled) * startFoot.translation.y + t_xy_scaled * targetFoot.translation.y + /*step.liftingFoot() **/ s_Y * sidekick_width * std::sin(-speedDirection);
        
        foot.translation.z = startFoot.translation.z + s_Zt * stepHeight;

        // LEGACY?
        // apply custom angles footRollOffset and footPitchOffset
        double t_z_scaled  = (1 - cos(t*Math::pi2))*0.5;
        foot.rotation = RotationMatrix::getRotationX(footRollOffset * t_z_scaled);
        foot.rotation.rotateY(Math::sgn(targetFoot.translation.x - startFoot.translation.x) * footPitchOffset * t_z_scaled);

        // apply the actual rotation trajectory
        RotationMatrix rot = RotationMatrix::interpolate(startFoot.rotation, targetFoot.rotation, t_xy_scaled);
        foot.rotation *= rot;

        return step.supFoot()*foot;
    }
    else
    {
        return step.footEnd();
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
