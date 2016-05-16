/* 
 * File:   RemoteSymbols.cpp
 */

#include "RemoteSymbols.h"

void RemoteSymbols::registerSymbols(xabsl::Engine& engine)
{

  engine.registerEnumElement("remote_control_action", "remote_control_action.none", RemoteControlCommand::NONE);
  engine.registerEnumElement("remote_control_action", "remote_control_action.stand", RemoteControlCommand::STAND);
  engine.registerEnumElement("remote_control_action", "remote_control_action.walk", RemoteControlCommand::WALK);
  engine.registerEnumElement("remote_control_action", "remote_control_action.kick_right", RemoteControlCommand::KICK_RIGHT);
  engine.registerEnumElement("remote_control_action", "remote_control_action.kick_left", RemoteControlCommand::KICK_LEFT);
  engine.registerEnumElement("remote_control_action", "remote_control_action.kick_forward", RemoteControlCommand::KICK_FORWARD);
  engine.registerEnumElement("remote_control_action", "remote_control_action.blink", RemoteControlCommand::BLINK);

  engine.registerEnumeratedInputSymbol("remote_control.action", "remote_control_action", (int*)(&getRemoteControlCommand().action));

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