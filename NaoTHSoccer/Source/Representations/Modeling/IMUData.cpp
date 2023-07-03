#include "IMUData.h"
#include "Tools/DataConversion.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<IMUData>::serialize(const IMUData& representation, std::ostream& stream)
{
  naothmessages::IMUData msg;
  DataConversion::toMessage(representation.location, *(msg.mutable_location()));
  DataConversion::toMessage(representation.velocity, *(msg.mutable_velocity()));
  DataConversion::toMessage(representation.acceleration, *(msg.mutable_acceleration()));
  DataConversion::toMessage(representation.acceleration_sensor, *(msg.mutable_acceleration_sensor()));

  DataConversion::toMessage(representation.rotation, *(msg.mutable_rotation()));
  DataConversion::toMessage(representation.rotational_velocity, *(msg.mutable_rotational_velocity()));
  DataConversion::toMessage(representation.rotational_velocity_sensor, *(msg.mutable_rotational_velocity_sensor()));
  DataConversion::toMessage(representation.orientation, *(msg.mutable_orientation()));
  DataConversion::toMessage(representation.orientation_rotvec, *(msg.mutable_orientation_rotvec()));

  msg.set_has_been_reset(representation.has_been_reset);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<IMUData>::deserialize(std::istream& stream, IMUData& representation)
{
  naothmessages::IMUData msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  DataConversion::fromMessage(msg.orientation(),representation.orientation);

  DataConversion::fromMessage(msg.location(), representation.location);
  DataConversion::fromMessage(msg.velocity(), representation.velocity);
  DataConversion::fromMessage(msg.acceleration(), representation.acceleration);
  DataConversion::fromMessage(msg.acceleration_sensor(), representation.acceleration_sensor);

  DataConversion::fromMessage(msg.rotation(), representation.rotation);
  DataConversion::fromMessage(msg.rotational_velocity(), representation.rotational_velocity);
  DataConversion::fromMessage(msg.rotational_velocity_sensor(), representation.rotational_velocity_sensor);
  DataConversion::fromMessage(msg.orientation(), representation.orientation);
  DataConversion::fromMessage(msg.orientation_rotvec(), representation.orientation_rotvec);

  representation.has_been_reset = msg.has_been_reset();
}
