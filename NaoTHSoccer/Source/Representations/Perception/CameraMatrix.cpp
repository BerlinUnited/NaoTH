/**
 * @file CameraMatrix.cpp
 * 
 * Definition of class CameraMatrix
 */ 

#include "CameraMatrix.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <Tools/CameraGeometry.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void CameraMatrix::print(std::ostream& stream) const
{
  //stream << "camera = " << cameraID << std::endl;
  stream << "x-angle [deg] = " << Math::toDegrees(rotation.getXAngle()) << std::endl;
  stream << "y-angle [deg] = " << Math::toDegrees(rotation.getYAngle()) << std::endl;
  stream << "z-angle [deg] = " << Math::toDegrees(rotation.getZAngle()) << std::endl;
  stream << "x-translation [mm] = " << translation.x << std::endl;
  stream << "y-translation [mm] = " << translation.y << std::endl;
  stream << "z-translation [mm] = " << translation.z << std::endl;
  stream << "valid = " << valid << std::endl;
  stream << "timestamp = " << timestamp << std::endl;
}//end print

void Serializer<CameraMatrix>::serialize(const CameraMatrix& representation, std::ostream& stream)
{
  naothmessages::CameraMatrix msg;
  // ACHTUNG: representation.cameraNumber has to be some value in naothmessages::CameraID
  // in particular, it cannot be -1 (!)
  //msg.set_cameraid((naothmessages::CameraID) representation.cameraID);

  msg.set_valid(representation.valid);
  msg.set_timestamp(representation.timestamp);
  naoth::DataConversion::toMessage(representation, *(msg.mutable_pose()));

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}//end serialize

void Serializer<CameraMatrix>::deserialize(std::istream& stream, CameraMatrix& representation)
{
  naothmessages::CameraMatrix msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  //representation.cameraID = (naoth::CameraInfo::CameraID) msg.cameraid();
  if(msg.has_timestamp()) {
    representation.timestamp = msg.timestamp();
  }
  if(msg.has_valid()) {
    representation.valid = msg.valid();
  }
  naoth::DataConversion::fromMessage(msg.pose(), representation);
}//end deserialize
