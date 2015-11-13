/**
 * @File:   FootTrajectoryGenerator.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */


#include "FootTrajectoryGenerator.h"

using namespace std;

Pose3D FootTrajectorGenerator::genTrajectory(
  const Pose3D& oldFoot, 
  const Pose3D& targetFoot,
  double cycle, 

  double samplesDoubleSupport, 
  double samplesSingleSupport, 
  double stepHeight, 
  double footPitchOffset,
  double footRollOffset
 )
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


Pose3D FootTrajectorGenerator::stepControl(
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
  )
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
    Pose3D speedTarget = targetFoot;
    speedTarget.translate(cos(speedDirection)*30, sin(speedDirection)*30, 0);
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

Pose3D FootTrajectorGenerator::genTrajectoryWithSplines(
  const Pose3D& oldFoot,
  const Pose3D& targetFoot,
  double cycle,

  double duration,
  double stepHeight,
  double footPitchOffset,
  double footRollOffset
 )
{
    double t = cycle/duration;

    // parameter for the step curve
    // NOTE: xyp is used to interpolate the motion in x/y-plane, while
    //       zp is for the motion in the z-axis

    vector<Vector2d > vecXY;
    vecXY.push_back(Vector2d(0.0, 0.0));
    vecXY.push_back(Vector2d(0.25, (1 - cos(0.25*Math::pi))*0.5));
    vecXY.push_back(Vector2d(0.5,  (1 - cos(0.5 *Math::pi))*0.5));
    vecXY.push_back(Vector2d(0.75, (1 - cos(0.75*Math::pi))*0.5));
    vecXY.push_back(Vector2d(1.0, 1.0));
    CubicSpline theCubicSplineXY(vecXY);

    vector<Vector2d > vecZ;
    vecZ.push_back(Vector2d(0.0, 0.0));
    vecZ.push_back(Vector2d(0.25, (1 - cos(0.25*Math::pi2))*0.5));
    vecZ.push_back(Vector2d(0.5, 1));
    vecZ.push_back(Vector2d(0.75, (1 - cos(0.75*Math::pi2))*0.5));
    vecZ.push_back(Vector2d(1.0, 0.0));
    CubicSpline theCubicSplineZ(vecZ);

    double xyp = theCubicSplineXY.y(t);
    double zp  = theCubicSplineZ.y(t);

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
