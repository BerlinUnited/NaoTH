
#include "RemoteControlCommand.h"


#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<RemoteControlCommand>::serialize(const RemoteControlCommand& representation, std::ostream& stream)
{
  naothmessages::RemoteControlCommand p;

  p.set_controlmode((naothmessages::RemoteControlCommand_ControlMode)representation.controlMode);
  p.set_action((naothmessages::RemoteControlCommand_ActionType)representation.action);
  p.set_second_action((naothmessages::RemoteControlCommand_SecondActionType)representation.second_action);

  DataConversion::toMessage(representation.target, *p.mutable_target());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<RemoteControlCommand>::deserialize(std::istream& stream, RemoteControlCommand& representation)
{
  naothmessages::RemoteControlCommand p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  if(p.has_controlmode()) {
    representation.controlMode = (RemoteControlCommand::ControlMode)p.controlmode();
  }

  if(p.has_action()) {
    representation.action = (RemoteControlCommand::ActionType)p.action();
  }

  if(p.has_second_action()) {
    representation.second_action = (RemoteControlCommand::SecondActionType)p.second_action();
  }

  if(p.has_target()) {
    DataConversion::fromMessage(p.target(), representation.target);
  }
  
}//end deserialize


std::string RemoteControlCommand::getActionName(ActionType id) 
{
  switch(id) 
  {
  case NONE: return "NONE";
  case STAND: return "STAND";
  case WALK: return "WALK";
  case KICK_RIGHT: return "KICK_RIGHT";
  case KICK_LEFT: return "KICK_LEFT";
  case KICK_FORWARD_LEFT: return "KICK_FORWARD_LEFT";
  case KICK_FORWARD_RIGHT: return "KICK_FORWARD_RIGHT";
  default: ASSERT(false);
  }

  ASSERT(false);
  return "unknown";
}

