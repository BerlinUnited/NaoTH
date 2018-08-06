#include "CentreOfPressure.h"
#include "Tools/DataConversion.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<CentreOfPressure>::serialize(const CentreOfPressure& representation, std::ostream& stream)
{
  naothmessages::CentreOfPressure msg;
  DataConversion::toMessage(representation.in_and_only_left_foot.CoP, *(msg.mutable_in_and_only_left_foot_cop()));
  msg.set_in_and_only_left_foot_magnitude(representation.in_and_only_left_foot.magnitude);
  msg.set_in_and_only_left_foot_valid(representation.in_and_only_left_foot.valid);

  DataConversion::toMessage(representation.in_and_only_right_foot.CoP, *(msg.mutable_in_and_only_right_foot_cop()));
  msg.set_in_and_only_right_foot_magnitude(representation.in_and_only_right_foot.magnitude);
  msg.set_in_and_only_right_foot_valid(representation.in_and_only_right_foot.valid);

  DataConversion::toMessage(representation.in_kinematic_chain_origin.CoP, *(msg.mutable_in_kinematic_chain_origin_cop()));
  msg.set_in_kinematic_chain_origin_magnitude(representation.in_kinematic_chain_origin.magnitude);
  msg.set_in_kinematic_chain_origin_valid(representation.in_kinematic_chain_origin.valid);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<CentreOfPressure>::deserialize(std::istream& stream, CentreOfPressure& representation)
{
  naothmessages::CentreOfPressure msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  DataConversion::fromMessage(msg.in_and_only_left_foot_cop(), representation.in_and_only_left_foot.CoP);
  representation.in_and_only_left_foot.magnitude = msg.in_and_only_left_foot_magnitude();
  representation.in_and_only_left_foot.valid     = msg.in_and_only_left_foot_valid();

  DataConversion::fromMessage(msg.in_and_only_right_foot_cop(), representation.in_and_only_right_foot.CoP);
  representation.in_and_only_right_foot.magnitude = msg.in_and_only_right_foot_magnitude();
  representation.in_and_only_right_foot.valid     = msg.in_and_only_right_foot_valid();

  DataConversion::fromMessage(msg.in_kinematic_chain_origin_cop(), representation.in_kinematic_chain_origin.CoP);
  representation.in_kinematic_chain_origin.magnitude = msg.in_kinematic_chain_origin_magnitude();
  representation.in_kinematic_chain_origin.valid     = msg.in_kinematic_chain_origin_valid();
}
