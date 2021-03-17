/**
* @file RobotPose.h
*
* Definition of class RobotPose which represents the position and direction of the robot on the field
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef RobotPose_h
#define RobotPose_h

#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include "Tools/Debug/DebugDrawings.h"
namespace naoth
{
class RobotPose: public Pose2D, public naoth::Printable
{
public:
  RobotPose() : isValid(false) {}

  RobotPose (const Pose2D& other) : isValid(false)
  {
    Pose2D::operator=(other);
  }

  virtual ~RobotPose(){}

  const RobotPose& operator=(const RobotPose& other)
  {
    Pose2D::operator=(other);
    this->isValid = other.isValid;
    return *this;
  }

  // NOTE: only the pose is modified the all the other members are left untouched
  const RobotPose& operator=(const Pose2D& other)
  {
    Pose2D::operator=(other);
    return *this;
  } 

  // indicates whether the pose is valid :)
  bool isValid;

  // estimated principal axes of the position belief (not for angle)
  // can be used as a measure for how 'precise' the position estimation is
  Vector2d principleAxisMajor;
  Vector2d principleAxisMinor;


  virtual void print(std::ostream& stream) const;
  void draw(DrawingCanvas2D& canvas, bool drawDeviation=false) const;
  
};

template<>
class Serializer<RobotPose>
{
public:
  static void serialize(const RobotPose& representation, std::ostream& stream);
  static void deserialize(std::istream& stream, RobotPose& representation);
};
}
#endif// RobotPose_h
