/**
* @file ParallelKinematic.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#ifndef _ParallelKinematic_H
#define	_ParallelKinematic_H

#include "Representations/Infrastructure/JointData.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Vector2.h"

namespace Kinematics {

class ParallelKinematic
{
public:
  class Pose
  {
  public:
    Vector3<double> alpha;
    Vector2<double> rotation;
  };

  ParallelKinematic(){};
  virtual ~ParallelKinematic(){}

  void calculateLegs(const Pose& pose, naoth::JointData& jointData);
  void calculateLegs(const Pose& poseLeft, const Pose& poseRight, naoth::JointData& jointData);
};

} // namespace Kinematics
#endif	/* _ParallelKinematic_H */

