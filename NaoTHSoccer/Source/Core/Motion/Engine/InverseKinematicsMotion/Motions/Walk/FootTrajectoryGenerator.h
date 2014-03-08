/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef FOOTTRAJECTORYGENERATOR_H
#define FOOTTRAJECTORYGENERATOR_H

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/CubicSpline.h"

class FootStepTrajectory
{
protected:
  const Pose3D oldFoot;
  const Pose3D targetFoot;
  const double stepHeight;
  const double samplesDoubleSupport;
  const double samplesSingleSupport; 
  const double extendDoubleSupport;

  virtual Pose3D genTrajectory(double t) = 0;

public:
  FootStepTrajectory(
    const Pose3D& oldFoot, 
    const Pose3D& targetFoot,
    double stepHeight,
    double samplesDoubleSupport, 
    double samplesSingleSupport, 
    double extendDoubleSupport
    )
    : oldFoot(oldFoot),
      targetFoot(targetFoot),
      stepHeight(stepHeight),
      samplesDoubleSupport(samplesDoubleSupport),
      samplesSingleSupport(samplesSingleSupport),
      extendDoubleSupport(extendDoubleSupport)
  {
  }

  Pose3D operator() (double cycle)
  {
    double doubleSupportEnd = samplesDoubleSupport / 2 + extendDoubleSupport;
    double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;

    if (cycle <= doubleSupportEnd)
    {
      return oldFoot;
    }
    else if (cycle <= doubleSupportBegin)
    {
      double t = 1 - (doubleSupportBegin - cycle) / (samplesSingleSupport - extendDoubleSupport);
      return genTrajectory(t);
    }
    else
    {
      return targetFoot;
    }
  }

};


class WalkTrajectory: public FootStepTrajectory
{
public:
  WalkTrajectory(
    const Pose3D& oldFoot, 
    const Pose3D& targetFoot,
    double stepHeight,
    double samplesDoubleSupport, 
    double samplesSingleSupport, 
    double extendDoubleSupport
    )
    : FootStepTrajectory(
        oldFoot, 
        targetFoot,
        stepHeight,
        samplesDoubleSupport, 
        samplesSingleSupport, 
        extendDoubleSupport
        )
  {
  }

protected:
  virtual Pose3D genTrajectory (double t)
  {
    // parameter for the step curve
    // NOTE: xp is used to interpolate the motion in x/y-plane, while
    //       yp is for the mition in the z-axis
    double xp = (1 - cos(t*Math::pi))*0.5;
    double yp = (1 - cos(t*Math::pi2))*0.5;

    Pose3D foot;

    foot.translation.z = targetFoot.translation.z + yp*stepHeight;
    foot.translation.x = (1.0 - xp) * oldFoot.translation.x + xp * targetFoot.translation.x;
    foot.translation.y = (1.0 - xp) * oldFoot.translation.y + xp * targetFoot.translation.y;

    foot.rotation = RotationMatrix::interpolate(oldFoot.rotation, targetFoot.rotation, xp);

    return foot;
  }
};


class StepControlTrajectory: public FootStepTrajectory
{
private:
  const double speedDirection;

public:
  StepControlTrajectory(
    const Pose3D& oldFoot, 
    const Pose3D& targetFoot,
    double stepHeight,
    double samplesDoubleSupport, 
    double samplesSingleSupport, 
    double extendDoubleSupport,
    double speedDirection
    )
    : FootStepTrajectory(
        oldFoot, 
        targetFoot,
        stepHeight,
        samplesDoubleSupport, 
        samplesSingleSupport, 
        extendDoubleSupport
        ),
      speedDirection(speedDirection)
  {
  }

protected:
  virtual Pose3D genTrajectory (double t)
  {
    double xp = (1 - cos(t*Math::pi))*0.5;
    double zp = (1 - cos(t*Math::pi2))*0.5;

    double s = 0.7;

    if(t < s) {
      xp = (1 - cos(t/s*Math::pi))*0.5;
    } else {
      xp = 1.0;
    }

    // TODO: optmize
    Pose3D speedTarget = targetFoot;
    speedTarget.translate(cos(speedDirection)*30, sin(speedDirection)*30, 0);
    
    std::vector<Vector2d > vecX;
    vecX.push_back(Vector2d(0.0, oldFoot.translation.x));
    vecX.push_back(Vector2d(1.0, targetFoot.translation.x));
    vecX.push_back(Vector2d(1.1, speedTarget.translation.x));
    CubicSpline theCubicSplineX(vecX);

    std::vector<Vector2d > vecY;
    vecY.push_back(Vector2d(0.0, oldFoot.translation.y));
    vecY.push_back(Vector2d(1.0, targetFoot.translation.y));
    vecY.push_back(Vector2d(1.1, speedTarget.translation.y));
    CubicSpline theCubicSplineY(vecY);

    Pose3D foot;
    foot.translation.z = targetFoot.translation.z + zp*stepHeight;

    foot.translation.x = theCubicSplineX.y(xp);
    foot.translation.y = theCubicSplineY.y(xp);

    foot.rotation = RotationMatrix::interpolate(oldFoot.rotation, targetFoot.rotation, xp);

    return foot;
  }
};


class FootTrajectorGenerator
{
public:
  static Pose3D genTrajectory(const Pose3D& oldFoot, const Pose3D& targetFoot,
    double cycle, double samplesDoubleSupport, double samplesSingleSupport, double extendDoubleSupport,
    double stepHeight, double footPitchOffset, double footYawOffset, double footRollOffset, double curveFactor);

  static Pose3D stepControl(const Pose3D& oldFoot, const Pose3D& targetFoot,
    double cycle, double samplesDoubleSupport, double samplesSingleSupport, double extendDoubleSupport,
    double stepHeight, double footPitchOffset, double footYawOffset, double footRollOffset, double curveFactor,
                            double speedDirection, double scale);
};


#endif  /* FOOTTRAJECTORYGENERATOR_H */

