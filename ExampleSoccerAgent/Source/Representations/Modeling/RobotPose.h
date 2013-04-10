/**
* @file RobotPose.h
*
* Definition of class RobotPose which represents the position and direction of the robot on the field
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef __RobotPose_h_
#define __RobotPose_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Printable.h"

class RobotPose: public Pose2D, public naoth::Printable
{
public:

  bool isValid;
 
  RobotPose() 
    : 
    isValid(true) 
    {}
  ~RobotPose(){}
 
  const RobotPose& operator=(const RobotPose& other)
  {
    (Pose2D&) *this = (const Pose2D&) other;
    this->isValid = other.isValid;
    return *this;
  }

  /**
  * Cast Contructor.
  */
  RobotPose (const Pose2D& otherPose) 
  {
    (Pose2D&) *this = otherPose;
    isValid = true;
  }

  const RobotPose& operator=(const Pose2D& other)
  {
    (Pose2D&) *this = other;
    this->isValid=true;
    return *this;
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "x = " << translation.x << std::endl;
    stream << "y = " << translation.y << std::endl;
    stream << "rotation = " << rotation << std::endl;
    stream << "valid = " << isValid << std::endl;
  }//end print

};

#endif// __RobotPose_h_
