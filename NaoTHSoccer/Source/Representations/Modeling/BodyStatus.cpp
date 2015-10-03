#include "BodyStatus.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<BodyStatus>::serialize(const BodyStatus& representation, std::ostream& stream)
{
  naothmessages::BodyStatus message;
  
  for(int i=0; i < JointData::numOfJoint; i++) 
  {
    message.add_currentsum(representation.currentSum[i]);
  }

  message.set_timestamp(representation.timestamp);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<BodyStatus>::deserialize(std::istream& stream, BodyStatus& representation)
{
  naothmessages::BodyStatus message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  // assure the integrity of the message
  ASSERT( message.currentsum().size() == JointData::numOfJoint || 
    message.currentsum().size() == JointData::numOfJoint - 4);

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    if(i < message.currentsum().size())
    {
      representation.currentSum[i] = message.currentsum(i);
    }
    else // LWristYaw, RWristYaw, LHand, RHand don't exist in old messages
    {
      representation.currentSum[i] = 0;
    }
  }

  representation.timestamp = message.timestamp();
}