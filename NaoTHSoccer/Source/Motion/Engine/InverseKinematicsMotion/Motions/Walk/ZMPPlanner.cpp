/**
* @file ZMPPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#include "ZMPPlanner.h"

#include "Tools/DataStructures/Spline.h"

using namespace InverseKinematic;

Vector2d ZMPPlanner::simplest(const FootStep& step, double offsetX, double offsetY)
{
  Pose3D supFoot = step.supFoot();
  supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
  return Vector2d(supFoot.translation.x, supFoot.translation.y);
}//end simplest


Vector2d ZMPPlanner::betterOne(
  const FootStep& step, double offsetX, double offsetY,
  double cycle, 
  double samplesDoubleSupport, double samplesSingleSupport,
  double offset, double width)
{
  double t = cycle / (samplesSingleSupport+samplesDoubleSupport);
  Pose3D supFoot = step.supFoot();
  supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
  
  Pose3D targetFoot = step.footEnd();
  targetFoot.translate(offsetX, -offsetY * step.liftingFoot(), 0);

  //double offset = 0.8;
  //double width = 0.4;

  double s = 0;
  if(t > offset + width/2.0) {
    s = 1.0;
  } else if(t > offset - width/2.0) {
    double ts = (t - (offset - width/2.0)) / width;
    std::vector<double> x = {0.0, 0.5, 1.0};
    std::vector<double> y = {0.0, 0.5, 1.0};

    tk::spline spline;
    spline.set_boundary(tk::spline::first_deriv,0.0, tk::spline::first_deriv,0.0, false);
    spline.set_points(x,y);
    s = spline(ts);
  }

  

  return Vector2d(
    supFoot.translation.x*(1.0-t) + t*targetFoot.translation.x, 
    supFoot.translation.y*(1.0-s) + s*targetFoot.translation.y);
}//end betterOne
