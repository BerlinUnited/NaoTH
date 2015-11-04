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
#include "Tools/Debug/DebugDrawings.h"

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

  void draw(DrawingCanvas2D& canvas, bool drawDeviation = false) const
  {
    //FIELD_DRAWING_CONTEXT;
    if(isValid)
    {
      /*
      switch( getPlayerInfo().gameData.teamColor )
      {
      case GameData::red:    canvas.pan("FF0000", 20); break;
      case GameData::blue:   canvas.pan("0000FF", 20); break;
      case GameData::yellow: canvas.pan("FFFF00", 20); break;
      case GameData::black:  canvas.pan("000000", 20); break;
      default: canvas.pan("AAAAAA", 20); break;
      }*/
      canvas.pen("000000", 20);
    }
    else
    {
      canvas.pen("AAAAAA", 20);
    }

    canvas.drawRobot(translation.x, translation.y, rotation);


    if(drawDeviation)
    {
      canvas.pen("000000", 20);

      canvas.drawLine(translation.x - principleAxisMajor.x, 
           translation.y - principleAxisMajor.y,
           translation.x + principleAxisMajor.x, 
           translation.y + principleAxisMajor.y);

      canvas.drawLine(translation.x - principleAxisMinor.x, 
           translation.y - principleAxisMinor.y,
           translation.x + principleAxisMinor.x, 
           translation.y + principleAxisMinor.y);

      canvas.drawOvalRotated(translation.x, 
                   translation.y, 
                   principleAxisMinor.abs()*2.0,
                   principleAxisMajor.abs()*2.0,
                   principleAxisMinor.angle());
    }
  }

};

#endif// _RobotPose_h_
