/**
* @file RobotPose.h
*
* Definition of class RobotPose which represents the position and direction of the robot on the field
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _RobotPose_h_
#define _RobotPose_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Printable.h"

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


  virtual void print(std::ostream& stream) const
  {
    stream << "x = " << translation.x << std::endl;
    stream << "y = " << translation.y << std::endl;
    stream << "rotation = " << rotation << std::endl;
    stream << "valid = " << isValid << std::endl;
  }

};

#endif// _RobotPose_h_
