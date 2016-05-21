/**
 * @file RemoteControlCommand.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _RemoteControlCommand_H
#define _RemoteControlCommand_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Tools/Math/Pose2D.h>

#include <Representations/Infrastructure/FrameInfo.h>

class RemoteControlCommand : public naoth::Printable
{
public:

  RemoteControlCommand() 
    : controlMode(DIRECT_CONTROL), 
      action(NONE), 
      second_action(SECOND_NONE)
  {}

  enum ControlMode
  {
    DIRECT_CONTROL,
    LOCK_CONTROL,
    NUMBER_OF_CONTROL_MODE_TYPE
  } controlMode;

  enum ActionType
  {
    NONE,
    STAND,
    WALK,
    KICK_RIGHT,
    KICK_LEFT,
    KICK_FORWARD_LEFT,
    KICK_FORWARD_RIGHT,
    NUMBER_OF_ACTION_TYPE
  } action;

  enum SecondActionType
  {
    SECOND_NONE,
    BLINK,
    SAY,
    NUMBER_OF_SECOND_ACTION_TYPE
  } second_action;

  Pose2D target;


  naoth::FrameInfo frameInfoWhenUpdated;

  virtual void print(std::ostream& stream) const
  {
    stream << "RemoteControlCommand" << std::endl;
    stream << "ActionType = " << action << std::endl;
    stream << "target = " << target << std::endl;
  }

  static std::string getActionName(ActionType id);
};


namespace naoth
{
  template<>
  class Serializer<RemoteControlCommand>
  {
  public:
    static void serialize(const RemoteControlCommand& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, RemoteControlCommand& representation);
  };
}

#endif // _RemoteControlCommand_H
