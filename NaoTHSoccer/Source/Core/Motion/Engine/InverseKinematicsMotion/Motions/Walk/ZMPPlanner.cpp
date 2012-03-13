/**
* @file ZMPPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#include "ZMPPlanner.h"


using namespace InverseKinematic;

Vector2d ZMPPlanner::simplest(const FootStep& step, double offsetX, double offsetY)
{
  Pose3D supFoot = step.supFoot();
  supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
  return Vector2d(supFoot.translation.x, supFoot.translation.y);
}//end simplest


Vector2d ZMPPlanner::betterOne(const FootStep& step, double offsetX, double offsetY,
  double cycle, double samplesDoubleSupport, double samplesSingleSupport, double extendDoubleSupport)
{
  double doubleSupportEnd = samplesDoubleSupport / 2 + extendDoubleSupport;
  double doubleSupportBegin = samplesDoubleSupport / 2 + samplesSingleSupport;
  samplesSingleSupport -= extendDoubleSupport;

  
  // interpolate in the double support phase
  if (cycle <= doubleSupportEnd)
  {
    double t = (doubleSupportEnd + cycle) / samplesDoubleSupport;

    Pose3D supFoot = step.supFoot();
    supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
    Vector2d supZMP(supFoot.translation.x, supFoot.translation.y);

    Pose3D beginFoot = step.footBegin();
    beginFoot.translate(offsetX, -offsetY * step.liftingFoot(), 0);
    Vector2d liftZMP(beginFoot.translation.x, beginFoot.translation.y);

    return supZMP*t + liftZMP*(1.0-t);
  }
  else if (cycle >= doubleSupportBegin)
  {
    double t = (cycle-doubleSupportBegin) / samplesDoubleSupport;

    Pose3D supFoot = step.supFoot();
    supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
    Vector2d supZMP(supFoot.translation.x, supFoot.translation.y);

    Pose3D endFoot = step.footEnd();
    endFoot.translate(offsetX, -offsetY * step.liftingFoot(), 0);
    Vector2d liftZMP(endFoot.translation.x, endFoot.translation.y);

    return supZMP*(1.0-t) + liftZMP*t;
  }
  else
  {
    Pose3D supFoot = step.supFoot();
    supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
    return Vector2d(supFoot.translation.x, supFoot.translation.y);
  }
}//end betterOne
