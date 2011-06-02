/**
* @file ZMPPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#include "ZMPPlanner.h"

using namespace InverseKinematic;

Pose3D ZMPPlanner::simplest(const FootStep& step, double height)
{
  Pose3D zmp;
  const Pose3D& supFoot = step.supFoot();
  zmp.translation = Vector3<double>(supFoot.translation.x, supFoot.translation.y, height);
  return zmp;
}