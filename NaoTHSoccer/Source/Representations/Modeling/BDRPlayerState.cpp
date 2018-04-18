
#include "BDRPlayerState.h"


#include "Messages/BDRMessages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;
/*
void Serializer<BDRPlayerState>::serialize(const BDRPlayerState& representation, std::ostream& stream)
{
  naothmessages::BDRPlayerState p;

  p.set_behaviormode((naothmessages::BDRBehaviorMode)representation.behaviorMode);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<BDRPlayerState>::deserialize(std::istream& stream, BDRPlayerState& representation)
{
  naothmessages::BDRPlayerState p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  if(p.has_behaviormode()) {
    representation.behaviorMode = (BDRPlayerState::BehaviorMode)p.behaviormode();
  }
}//end deserialize
*/

std::string BDRPlayerState::getActivityName(Activity id)
{
  switch(id)
  {
    case pre_playing      : return "pre_playing";
    case playing          : return "playing";
    case pre_entertaining : return "pre_entertaining";
    case entertaining     : return "entertaining";
    case pre_servicing    : return "pre_servicing";
    case servicing        : return "servicing";
    case doing_nothing    : return "doing_nothing";
    case initializing     : return "initializing";
    default : ASSERT(false);
  }
}

