/* 
 * File:   RemoteSymbols.cpp
 */

#include "RemoteSymbols.h"

void RemoteSymbols::registerSymbols(xabsl::Engine& engine)
{
  // control mode
  engine.registerEnumElement("remote_control_mode", "remote_control_mode.direct", RemoteControlCommand::DIRECT_CONTROL);
  engine.registerEnumElement("remote_control_mode", "remote_control_mode.lock", RemoteControlCommand::LOCK_CONTROL);

  // action type
  engine.registerEnumElement("remote_control_action", "remote_control_action.none", RemoteControlCommand::NONE);
  engine.registerEnumElement("remote_control_action", "remote_control_action.stand", RemoteControlCommand::STAND);
  engine.registerEnumElement("remote_control_action", "remote_control_action.walk", RemoteControlCommand::WALK);
  engine.registerEnumElement("remote_control_action", "remote_control_action.kick_right", RemoteControlCommand::KICK_RIGHT);
  engine.registerEnumElement("remote_control_action", "remote_control_action.kick_left", RemoteControlCommand::KICK_LEFT);
  engine.registerEnumElement("remote_control_action", "remote_control_action.kick_forward_left", RemoteControlCommand::KICK_FORWARD_LEFT);
  engine.registerEnumElement("remote_control_action", "remote_control_action.kick_forward_right", RemoteControlCommand::KICK_FORWARD_RIGHT);

  // second action type
  engine.registerEnumElement("remote_control_second_action", "remote_control_second_action.none", RemoteControlCommand::SECOND_NONE);
  engine.registerEnumElement("remote_control_second_action", "remote_control_second_action.blink", RemoteControlCommand::BLINK);
  engine.registerEnumElement("remote_control_second_action", "remote_control_second_action.say", RemoteControlCommand::SAY);


  // symbols
  engine.registerEnumeratedInputSymbol("remote_control.mode", "remote_control_mode", (int*)(&getRemoteControlCommand().controlMode));
  engine.registerEnumeratedInputSymbol("remote_control.action", "remote_control_action", (int*)(&getRemoteControlCommand().action));
  engine.registerEnumeratedInputSymbol("remote_control.second_action", "remote_control_second_action", (int*)(&getRemoteControlCommand().second_action));

  engine.registerDecimalInputSymbol("remote_control.target.x", &getRemoteControlCommand().target.translation.x);
  engine.registerDecimalInputSymbol("remote_control.target.y", &getRemoteControlCommand().target.translation.y);
  engine.registerDecimalInputSymbol("remote_control.target.rot", &getRemoteControlCommand().target.rotation);

  engine.registerDecimalInputSymbol("remote_control.time_since_update", &timeSinceUpdate);

}//end registerSymbols

RemoteSymbols* RemoteSymbols::theInstance = NULL;

RemoteSymbols::~RemoteSymbols()
{
}

void RemoteSymbols::execute()
{

}

double RemoteSymbols::timeSinceUpdate()
{
  return theInstance->getFrameInfo().getTimeSince(theInstance->getRemoteControlCommand().frameInfoWhenUpdated.getTime());
}