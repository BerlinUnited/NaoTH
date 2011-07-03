/**
 * @file CalibrationData.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef CALIBRATIONDATA_H
#define CALIBRATIONDATA_H

#include <Tools/Math/Vector2.h>
#include <Tools/DataConversion.h>
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataStructures/Printable.h>

class CalibrationData: public naoth::Printable
{
public:
  Vector2d inertialSensorOffset;

  virtual void print(ostream& stream) const
  {
    stream << "inertialSensorOffset = "<<inertialSensorOffset<<"\n";
  }
};

namespace naoth
{
  template<>
  class Serializer<CalibrationData>
  {
  public:
    static void serialize(const CalibrationData& representation, naothmessages::CalibrationData* msg)
    {
      DataConversion::toMessage(representation.inertialSensorOffset, *(msg->mutable_inertialsensoroffset()));
    }

    static void serialize(const CalibrationData& representation, std::ostream& stream)
    {
      naothmessages::CalibrationData message;
      serialize(representation, &message);
      google::protobuf::io::OstreamOutputStream buf(&stream);
      message.SerializeToZeroCopyStream(&buf);
    }

    static void deserialize(const naothmessages::CalibrationData* msg, CalibrationData& representation)
    {
      DataConversion::fromMessage(msg->inertialsensoroffset(), representation.inertialSensorOffset);
    }

    static void deserialize(std::istream& stream, CalibrationData& representation)
    {
      naothmessages::CalibrationData message;
      google::protobuf::io::IstreamInputStream buf(&stream);
      message.ParseFromZeroCopyStream(&buf);

      deserialize(&message, representation);
    }
  };
}

#endif // CALIBRATIONDATA_H
