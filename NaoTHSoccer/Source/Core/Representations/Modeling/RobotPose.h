/**
* @file RobotPose.h
*
* Definition of class RobotPose which represents the position and direction of the robot on the field
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef __RobotPose_h_
#define __RobotPose_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/ModuleFramework/Representation.h"
#include "Tools/DataStructures/Printable.h"

class RobotPose: public Pose2D, public Printable
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

  virtual void print(ostream& stream) const
  {
    stream << "x = " << translation.x << endl;
    stream << "y = " << translation.y << endl;
    stream << "rotation = " << rotation << endl;
    stream << "valid = " << isValid << endl;
  }//end print

};

REPRESENTATION_INTERFACE(RobotPose);

#endif// __RobotPose_h_
