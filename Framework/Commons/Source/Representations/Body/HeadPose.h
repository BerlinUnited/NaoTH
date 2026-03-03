/**
* @file HeadPose.h
*
* @author <a href="mailto:mellmanninformatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of the class MotionStatus
*/

#ifndef HEAD_POSE_H
#define HEAD_POSE_H

#include "Tools/Math/Pose3D.h"

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include <string>

/**
* This describes the HeadPose
*/
class HeadPose : public naoth::Printable
{
public:

  /** constructor */
  HeadPose() : timestamp(0) {}
  ~HeadPose(){}
  
  // time in ns
  uint64_t timestamp;
  Pose3D pose;

  virtual void print(std::ostream& stream) const
  {
    stream << "timestamp = " << timestamp << '\n';
    stream << "pose = " << pose << '\n';
  }
};

namespace naoth
{
  template<>
  class Serializer<HeadPose>
  {
  public:
    static void serialize(const HeadPose& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, HeadPose& representation);
  };
}

#endif // HEAD_POSE_H
