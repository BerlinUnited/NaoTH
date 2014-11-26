#include "Representations/Infrastructure/GyrometerData.h"
#include "Messages/CommonTypes.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;

void GyrometerData::print(ostream& stream) const
{
  stream << "x = " << data.x << " rad/s (" << rawData.x << ")" << endl;
  stream << "y = " << data.y << " rad/s (" << rawData.y << ")" << endl;
  stream << "z = " << data.z << " rad/s (" << rawData.z << ")" << endl;
}

void Serializer<GyrometerData>::serialize(const GyrometerData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  for(int i=0; i < 2; i++)
  {
    msg.add_v(representation.data[i]);
    msg.add_v(representation.rawData[i]);
  }
  msg.add_v(representation.ref);

  ::google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<GyrometerData>::deserialize(std::istream& stream, GyrometerData& representation)
{
  naothmessages::DoubleVector msg;

  ::google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  for(int i=0; i<2; i++)
  {
    representation.data[i] = msg.v(i*2);
    representation.rawData[i] = msg.v(i*2+1);
  }
  representation.ref = msg.v(msg.v_size()-1);
}
