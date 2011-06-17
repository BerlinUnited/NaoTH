/**
* @file ZMPPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#include "ZMPPlanner.h"

using namespace InverseKinematic;

Vector2d ZMPPlanner::simplest(const FootStep& step)
{
  const Pose3D& supFoot = step.supFoot();
  return Vector2d(supFoot.translation.x, supFoot.translation.y);
}
