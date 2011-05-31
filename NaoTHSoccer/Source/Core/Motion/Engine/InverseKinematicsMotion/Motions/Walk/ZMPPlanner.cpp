/**
* @file ZMPPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#include "ZMPPlanner.h"

using namespace InverseKinematic;

Pose3D ZMPPlanner::simplest(const FootStep& step, const FeetPose& feet, double height)
{
  Pose3D zmp;
  zmp.translation = Vector3<double>(0, 0, height);
    
  switch ( step.liftingFoot() )
  {
    case FootStep::LEFT:
    {
      zmp.translation.y = feet.right.translation.y;
      break;
    }
    case FootStep::RIGHT:
    {
      zmp.translation.y = feet.left.translation.y;
      break;
    }
  }
  return zmp;
}