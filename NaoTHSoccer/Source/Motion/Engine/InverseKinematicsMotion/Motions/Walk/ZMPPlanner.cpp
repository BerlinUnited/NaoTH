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

Vector2d ZMPPlanner::bezierBased(const FootStep& step, double offsetX, double offsetY,
  double cycle,
  double samplesDoubleSupport, double samplesSingleSupport,
  double inFootScalingY,
  double transitionScaling)
{
  Pose3D supFoot = step.supFoot();
  Pose3D startFoot  = step.footBegin();
  Pose3D targetFoot = step.footEnd();

  // don't apply offset befor determining start and target because applying offsets first will add discontinuities (start of step i won't be target of step i-1)
  double start    = (supFoot.translation.y + startFoot.translation.y)/2;
  double target   = (supFoot.translation.y + targetFoot.translation.y)/2;

  supFoot.translate(offsetX, offsetY * step.liftingFoot(), 0);
  double supFootY = supFoot.translation.y;

  //targetFoot.translate(offsetX, -offsetY * step.liftingFoot(), 0);

  //static std::vector<Vector2d> polygon = {Vector2d(0.0,0.0), Vector2d(0.9,0.0), Vector2d(0.3,1.0),Vector2d(1.0, 1.0)}; // ("time", value)
  //static std::vector<Vector2d> polygon_right_lifting = {Vector2d(0.0,0.0), Vector2d(0.2,1.33), Vector2d(0.8,1.33),Vector2d(1.0, 0.0)}; // ("time", value)
  //static std::vector<Vector2d> polygon_left_lifting = {Vector2d(0.0,0.0), Vector2d(0.2,-1.33), Vector2d(0.8,-1.33),Vector2d(1.0, 0.0)}; // ("time", value)
  static std::vector<Vector2d> trajectory;
  static unsigned int idx;

  if(cycle < 0.000001 || trajectory.empty()){
       idx = 0;
       trajectory.clear();
       //if(step.liftingFoot()) {
            static std::vector<Vector2d> temp;
           // trajectory from start to foot
           std::vector<Vector2d> start_to_foot = {Vector2d(0.0,start),
                                                  Vector2d(transitionScaling/4,(start+supFootY)/2),
                                                  Vector2d(transitionScaling/4,(start+supFootY)/2),
                                                  Vector2d(transitionScaling/2, supFootY)};
           temp = FourPointBezier2D(start_to_foot, 200);
           trajectory.insert(trajectory.end(),temp.begin(),temp.end());

           // trajectory in foot
           std::vector<Vector2d> in_foot =  {Vector2d(transitionScaling/2,supFootY),
                                             Vector2d(transitionScaling/2+transitionScaling/4,supFootY+(supFootY-start)/2*inFootScalingY),
                                             Vector2d(1-transitionScaling/2-transitionScaling/4,supFootY+(supFootY-target)/2*inFootScalingY),
                                             Vector2d(1-transitionScaling/2, supFootY)};
           temp = FourPointBezier2D(in_foot, 200);
           trajectory.insert(trajectory.end(),temp.begin(),temp.end());

           // trajectory from foot to target
           std::vector<Vector2d> foot_to_target = {Vector2d(1-transitionScaling/2,supFootY),
                                                   Vector2d(1-transitionScaling/4,(target+supFootY)/2),
                                                   Vector2d(1-transitionScaling/4,(target+supFootY)/2),
                                                   Vector2d(1.0, target)};
           temp = FourPointBezier2D(foot_to_target, 200);
           trajectory.insert(trajectory.end(),temp.begin(),temp.end());
//       } else {
//           trajectory = FourPointBezier2D(polygon_right_lifting, 200);
//       }
  }

  double t = cycle / (samplesSingleSupport+samplesDoubleSupport);
  for(; trajectory[idx].x < t && idx+1 < trajectory.size(); ++idx);

  double s = trajectory[idx].y;

  return Vector2d(
    supFoot.translation.x*(1.0-t) + t*targetFoot.translation.x,
    //start*(1.0-s) + s*target
    s);
}//end betterOne
