/**
 * @file CalibrationData.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _CalibrationData_H
#define _CalibrationData_H


#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <Tools/Math/Vector2.h>
#include <Tools/DataConversion.h>
#include <Tools/DataStructures/Printable.h>

class CalibrationData: public naoth::Printable
{
public:
  CalibrationData() : calibrated(false) {}

  Vector2d inertialSensorOffset;
  Vector2d gyroSensorOffset;
  Vector3d accSensorOffset;

  // is true, when valid offsets are avaliable
  bool calibrated;

  virtual void print(ostream& stream) const
  {
    stream << "inertialSensorOffset = " << inertialSensorOffset << endl;
    stream << "gyroSensorOffset = " << gyroSensorOffset << endl;
    stream << "accSensorOffset = " << accSensorOffset << endl;
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
      DataConversion::toMessage(representation.gyroSensorOffset, *(msg->mutable_gyrosensoroffset()));
      DataConversion::toMessage(representation.accSensorOffset, *(msg->mutable_accsensoroffset()));
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
      DataConversion::fromMessage(msg->gyrosensoroffset(), representation.gyroSensorOffset);
      DataConversion::fromMessage(msg->accsensoroffset(), representation.accSensorOffset);
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

#endif // _CalibrationData_H
