
#include "RemoteControlCommand.h"


#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<RemoteControlCommand>::serialize(const RemoteControlCommand& representation, std::ostream& stream)
{
  naothmessages::RemoteControlCommand p;

  p.set_action((naothmessages::RemoteControlCommand_ActionType)representation.action);

  DataConversion::toMessage(representation.target, *p.mutable_target());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<RemoteControlCommand>::deserialize(std::istream& stream, RemoteControlCommand& representation)
{
  naothmessages::RemoteControlCommand p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  if(p.has_action()) {
    representation.action = (RemoteControlCommand::ActionType)p.action();
  }

  if(p.has_target()) {
    DataConversion::fromMessage(p.target(), representation.target);
  }
  
}//end deserialize
