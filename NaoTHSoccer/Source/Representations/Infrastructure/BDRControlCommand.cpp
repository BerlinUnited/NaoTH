
#include "BDRControlCommand.h"


#include "Messages/BDRMessages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<BDRControlCommand>::serialize(const BDRControlCommand& representation, std::ostream& stream)
{
  naothmessages::BDRControlCommand p;

  p.set_behaviormode((naothmessages::BDRBehaviorMode)representation.behaviorMode);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<BDRControlCommand>::deserialize(std::istream& stream, BDRControlCommand& representation)
{
  naothmessages::BDRControlCommand p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  if(p.has_behaviormode()) {
    representation.behaviorMode = (BDRControlCommand::BehaviorMode)p.behaviormode();
  }
}//end deserialize


std::string BDRControlCommand::getBehaviorModeName(BehaviorMode id) 
{
  switch(id) 
  {
  case DO_NOTHING: return "DO_NOTHING";
  case AUTONOMOUS_PLAY: return "AUTONOMOUS_PLAY";
  case WARTUNG: return "WARTUNG";
  default: ASSERT(false);
  }

  ASSERT(false);
  return "unknown";
}

