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

  RemoteControlCommand() : action(NONE)
  {}

  enum ActionType
  {
    NONE,
    STAND,
    WALK,
    KICK_RIGHT,
    KICK_LEFT,
    KICK_FORWARD,
    BLINK,
    NUMBER_OF_ACTION_TYPE
  } action;

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
