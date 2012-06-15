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
  RobotPose() 
    : 
    isValid(false)
    {}

  virtual ~RobotPose(){}


  // indicates whether the pose is valid :)
  bool isValid;

  // estimated principal axes of the position belief (not for angle)
  // can be used as a measure for how 'precise' the position estimation is
  Vector2<double> principleAxisMajor;
  Vector2<double> principleAxisMinor;


  void setPose(const Pose2D& other, bool isValid)
  {
    (Pose2D&) *this = other;
    this->isValid=isValid;
  }

  // TODO: is this operator neded?
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
    // why is the pose invalidated?
    //isValid = false;
  }

  const RobotPose& operator=(const Pose2D& other)
  {
    (Pose2D&) *this = other;
    // why is the pose invalidated?
    //this->isValid=false;
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

#endif// __RobotPose_h_
