/**
* @file GPSData.cpp
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Benjamin</a>
*
*/

#include "GPSData.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<GPSData>::serialize(const GPSData& representation, std::ostream& stream)
{
  naothmessages::GPSData message;
  
  DataConversion::toMessage(representation.data, *(message.mutable_pose()));

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<GPSData>::deserialize(std::istream& stream, GPSData& representation)
{
  naothmessages::GPSData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  DataConversion::fromMessage(message.pose(), representation.data);
}