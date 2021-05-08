#include "FSRData.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;


FSRData::FSRData()
  : 
  dataLeft(numOfFSR,0),
  dataRight(numOfFSR,0)
{
}

FSRData::~FSRData()
{
}

const string FSRData::getFSRName(SensorID fsr) // static
{
  switch(fsr)
  {
    case FrontLeft: return string("FrontLeft");
    case FrontRight: return string("FrontRight");
    case RearLeft: return string("RearLeft");
    case RearRight: return string("RearRight");
    default: return string("Unknown FSR");
  }
}

void FSRData::print(ostream& stream) const
{
  stream << "Name : force(N) , data"<< std::endl;
  stream << "Left:"<< std::endl;
  for(size_t i = 0; i < numOfFSR; i++) {
    stream  << getFSRName((SensorID)i) << " : " 
            << dataLeft[i] << " ";
  }

  stream << std::endl << "Right:" << std::endl;
  for(size_t i = 0; i < numOfFSR; i++) {
    stream  << getFSRName((SensorID)i) << " : " 
            << dataRight[i] << " ";
  }
}

void Serializer<FSRData>::serialize(const FSRData& representation, std::ostream& stream)
{
  naothmessages::FSRData msg;

  // NOTE: deprecated to support logs before 12.01.2017
  for(size_t i=0; i<FSRData::numOfFSR; i++) {
//    msg.add_force(representation.force[i]);
    msg.add_data(representation.dataLeft[i]);
  }
  for(size_t i=0; i<FSRData::numOfFSR; i++) {
    msg.add_data(representation.dataRight[i]);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<FSRData>::deserialize(std::istream& stream, FSRData& representation)
{
  google::protobuf::io::IstreamInputStream buf(&stream);
  naothmessages::FSRData msg;
  msg.ParseFromZeroCopyStream(&buf);


  // NOTE: deprecated to support logs before 12.01.2017
  for (int i = 0; i < FSRData::numOfFSR; i++) {
//    representation.force[i] = msg.force(i);
    representation.dataLeft[i] = msg.data(i);
    representation.dataRight[i] = msg.data(FSRData::numOfFSR+i);
  }
}
