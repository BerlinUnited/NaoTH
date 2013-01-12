/**
 * @File:   FootTrajectoryGenerator.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */


#include "FootTrajectoryGenerator.h"

using namespace std;

Pose3D FootTrajectorGenerator::genTrajectory(
  const Pose3D& oldFoot, const Pose3D& targetFoot,
  double cycle, double samplesDoubleSupport, double samplesSingleSupport, double extendDoubleSupport,
  double stepHeight, double footPitchOffset, double footYawOffset, double footRollOffset, double curveFactor)
{
  double doubleSupportEnd = samplesDoubleSupport / 2 + extendDoubleSupport;
  double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;
  samplesSingleSupport -= extendDoubleSupport;

  if (cycle <= doubleSupportEnd)
  {
    return oldFoot;
  }
  else if (cycle <= doubleSupportBegin)
  {
    //      cout<<"foot stat 1"<<endl;
    double t = 1 - (doubleSupportBegin - cycle) / samplesSingleSupport;
    //        double xp = 10*t*t*t - 15*t*t*t*t + 6*t*t*t*t*t;
    //double xp = 1 / (1 + exp(-(t - 0.5) * curveFactor)); // this one has jumps for some values of curveFactor
    double xp = (1 - cos(t*Math::pi))*0.5;
    //        double xp = exp( -1 * exp(-10*(t-0.5)) );
    //        double yp = 16*t*t - 32*t*t*t + 16*t*t*t*t;
    //        cout<<"t = "<<t<<endl;
    t = t * Math::pi - Math::pi_2;
    //        double xp = (1+sin(t))*0.5;
    double yp = (1 + cos(t * 2))*0.5; //cos(t);
    //    if (t < 0) yp = cos(t);
    //        cout<<"t= "<<t<<" yp="<<yp<<endl;

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
  double extendDoubleSupport,
  double stepHeight, 
  double footPitchOffset, 
  double footYawOffset, 
  double footRollOffset, 
  double curveFactor,
  double speedDirection
  )
{
  double doubleSupportEnd = samplesDoubleSupport / 2 + extendDoubleSupport;
  double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;
  samplesSingleSupport -= extendDoubleSupport;

  if (cycle <= doubleSupportEnd)
  {
    return oldFoot;
  }
  else if (cycle <= doubleSupportBegin)
  {
    double t = 1 - (doubleSupportBegin - cycle) / samplesSingleSupport;
    //double xp = 1 / (1 + exp(-(t - 0.5) * curveFactor));// this one has jumps for some values of curveFactor
    double xp = (1 - cos(t*Math::pi))*0.5;
    t = t * Math::pi - Math::pi_2;
    double zp = (1 + cos(t * 2))*0.5;

    // TODO: optmize
    Pose3D speedTarget = targetFoot;
    speedTarget.translate(cos(speedDirection)*30, sin(speedDirection)*30, 0);
    vector<Vector2<double> > vecX;
    vecX.push_back(Vector2<double>(0, oldFoot.translation.x));
    vecX.push_back(Vector2<double>(1, targetFoot.translation.x));
    vecX.push_back(Vector2<double>(1.1, speedTarget.translation.x));
    CubicSpline theCubicSplineX;
    theCubicSplineX.init(vecX);

    vector<Vector2<double> > vecY;
    vecY.push_back(Vector2<double>(0, oldFoot.translation.y));
    vecY.push_back(Vector2<double>(1, targetFoot.translation.y));
    vecY.push_back(Vector2<double>(1.1, speedTarget.translation.y));
    CubicSpline theCubicSplineY;
    theCubicSplineY.init(vecY);

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
