#include "FSRData.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;


FSRData::FSRData()
{
  for(int i=0;i<numOfFSR;i++)
  {
    force[i] = 0.0;
    data[i] = 0.0;
    valid[i] = true;
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
  stream << "Name : force(N) , data"<<endl;
  
  for(int i = 0; i < numOfFSR; i++)
  {
    stream  << getFSRName((FSRID)i) << " : " 
            << force[i] << " , " 
            << data[i] << " "
            << (valid[i]?"\n":"INVALID!!\n");
  }
  stream << "Force Left: " << forceLeft() << std::endl;
  stream << "Force Right: " << forceRight() << std::endl;
}

double FSRData::forceLeft() const
{
  double f = 0;
  for ( int i=LFsrFL; i<=LFsrBR; i++)
  {
    if ( valid[i] )// check if the sensor is valid
    {
      f += force[i];
    }
  }
  return f;
}

double FSRData::forceRight() const
{
  double f = 0;
  for (int i = RFsrFL; i <= RFsrBR; i++)
  {
    if ( valid[i] )// check if the sensor is valid
    {
      f += force[i];
    }
  }
  return f;
}

void Serializer<FSRData>::serialize(const FSRData& representation, std::ostream& stream)
{
  naothmessages::FSRData msg;
  for(size_t i=0; i<FSRData::numOfFSR; i++)
  {
    msg.add_force(representation.force[i]);
    msg.add_data(representation.data[i]);
    msg.add_valid(representation.valid[i]);
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
    representation.force[i] = msg.force(i);
    representation.data[i] = msg.data(i);
    representation.valid[i] = msg.valid(i);
  }
}

double FSRData::forceOf(FSRID fsrId) const 
{
  // check if the sensor is valid
  if (valid[fsrId]) {
    return force[fsrId];
  }
  return -1;
}

