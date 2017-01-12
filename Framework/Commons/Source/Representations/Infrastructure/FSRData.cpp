#include "FSRData.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;


FSRData::FSRData()
{
  for(int i=0;i<numOfFSR;i++)
  {
//    force[i] = 0.0;
    data[i] = 0.0;
  }
}

FSRData::~FSRData()
{
}

const string FSRData::getFSRName(FSRID fsr) // static
{
  switch(fsr)
  {
    case LFsrFL: return string("LFsrFL");
    case LFsrFR: return string("LFsrFR");
    case LFsrBL: return string("LFsrBL");
    case LFsrBR: return string("LFsrBR");
    case RFsrFL: return string("RFsrFL");
    case RFsrFR: return string("RFsrFR");
    case RFsrBL: return string("RFsrBL");
    case RFsrBR: return string("RFsrBR");
    default: return string("Unknown FSR");
  }
}

void FSRData::print(ostream& stream) const
{
  stream << "Name : force(N) , data"<< std::endl;
  
  for(int i = 0; i < numOfFSR; i++)
  {
    stream  << getFSRName((FSRID)i) << " : " 
//            << force[i] << " , " 
            << data[i] << " ";
  }
}

void Serializer<FSRData>::serialize(const FSRData& representation, std::ostream& stream)
{
  naothmessages::FSRData msg;
  for(size_t i=0; i<FSRData::numOfFSR; i++)
  {
//    msg.add_force(representation.force[i]);
    msg.add_data(representation.data[i]);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<FSRData>::deserialize(std::istream& stream, FSRData& representation)
{
  google::protobuf::io::IstreamInputStream buf(&stream);
  naothmessages::FSRData msg;
  msg.ParseFromZeroCopyStream(&buf);
  for (int i = 0; i < FSRData::numOfFSR; i++)
  {
//    representation.force[i] = msg.force(i);
    representation.data[i] = msg.data(i);
  }
}
