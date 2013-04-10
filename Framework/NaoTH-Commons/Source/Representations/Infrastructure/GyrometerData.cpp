#include "Representations/Infrastructure/GyrometerData.h"
#include "Messages/CommonTypes.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;

GyrometerData::GyrometerData()
{
}

GyrometerData::~GyrometerData()
{
}

void GyrometerData::print(ostream& stream) const
{
  stream << "X : " << data.x << " radian/s (" << rawData.x << ")" << endl;
  stream << "Y : " << data.y << " radian/s (" << rawData.y << ")" << endl;
}//end print

void Serializer<GyrometerData>::serialize(const GyrometerData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i < 2; i++)
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
