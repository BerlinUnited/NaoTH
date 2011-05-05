#include "FSRData.h"
#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

// TODO: make it config
const Vector3<double> FSRData::offset[FSRData::numOfFSR] =
{Vector3<double>(70.1,30 ,-46),
 Vector3<double>(70.1,-23,-46),
 Vector3<double>(-30.4,30,-46),
 Vector3<double>(-29.8,-19,-46),
 Vector3<double>(70.1, 23,-46),
 Vector3<double>(70.1,-30,-46),
 Vector3<double>(-29.8,19,-46),
 Vector3<double>(-30.4,-30,-46)
};
   
FSRData::FSRData()
{
  for(int i=0;i<numOfFSR;i++)
  {
    force[i] = 0.0;
    data[i] = 0.0;
    valid[i] = true;
  }//end for
}

FSRData::~FSRData()
{
}

string FSRData::getFSRName(FSRID fsr)
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
  }//end switch
}//end getName

void FSRData::print(ostream& stream) const
{
  stream << "Name : force(N) , data"<<endl;
  double sum = 0;
  for(int i = 0; i < numOfFSR; i++)
  {
    stream << getFSRName((FSRID)i) << " : " << force[i] << " , " << data[i];
    if ( !valid[i] ) stream << " INVALID!!";
    stream << "\n";
    sum += force[i];
  }//end for
  stream << "Sum: " << sum << endl;
}//end print

double FSRData::forceLeft() const
{
  double f = 0;
  for ( int i=LFsrFL; i<=LFsrBR; i++)
  {
    if ( valid[i] )// check if the sensor is valid
    {
      f += force[i];
    }
  }//end for
  return f;
}//end forceLeft

double FSRData::forceRight() const
{
  double f = 0;
  for (int i = RFsrFL; i <= RFsrBR; i++)
  {
    if ( valid[i] )// check if the sensor is valid
    {
      f += force[i];
    }
  }//end for
  return f;
}//end forceRight

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
  // get length of data
  stream.seekg (0, ios::end);
  int length = stream.tellg();
  stream.seekg (0, ios::beg);

  
  google::protobuf::io::IstreamInputStream buf(&stream);

  if ( length == 144 )
  {
    naothmessages::DoubleVector msg;
    msg.ParseFromZeroCopyStream(&buf);
    for (int i = 0; i < FSRData::numOfFSR; i++)
    {
      representation.force[i] = msg.v(i*2);
      representation.data[i] = msg.v(i*2+1);
    }
  }
  else
  {
    naothmessages::FSRData msg;
    msg.ParseFromZeroCopyStream(&buf);
    for (int i = 0; i < FSRData::numOfFSR; i++)
    {
      representation.force[i] = msg.force(i);
      representation.data[i] = msg.data(i);
      representation.valid[i] = msg.valid(i);
    }
  }
}

double FSRData::forceOf(FSRID fsrId) const {
  if (valid[fsrId])// check if the sensor is valid
  {
    return force[fsrId];
  }
  return -1;
}//end forceRight

