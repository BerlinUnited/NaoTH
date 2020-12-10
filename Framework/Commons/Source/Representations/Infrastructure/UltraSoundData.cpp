#include "Representations/Infrastructure/UltraSoundData.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>


using namespace naoth;

// todo: change this to mm?
const double UltraSoundReceiveData::MIN_DIST = 0.25 * 1000.0;
const double UltraSoundReceiveData::INVALID = 2.55 * 1000.0;

void UltraSoundReceiveData::init()
{
  rawdata = INVALID;
  for(int i = 0; i < numOfUSEcho; i++)
  {
    dataLeft[i] = INVALID;
    dataRight[i] = INVALID;
  }
}

void UltraSoundReceiveData::print(std::ostream& stream) const
{
  stream
    << "UltraSoundReceiveData" << std::endl
    << "---------------------" << std::endl
    << "rawdata = " << rawdata << std::endl;

  for(int i = 0; i < numOfUSEcho; i++) {
    stream << "data[" << i << "]: left = " << dataLeft[i] << " , "  << "right = " << dataRight[i] << std::endl;
  }
}


void Serializer<UltraSoundReceiveData>::deserialize(std::istream& stream, UltraSoundReceiveData& representation)
{
  naothmessages::UltraSoundReceiveData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.rawdata = message.rawdata();

  // some logfiles crash, because we usedto have 9 echos
  ASSERT(message.dataleft_size() <= message.dataright_size());
  ASSERT(message.dataleft_size() <= representation.numOfUSEcho);

  for(int i = 0; i < message.dataleft_size(); i++)
  {
    representation.dataLeft[i] = message.dataleft(i);
    representation.dataRight[i] = message.dataright(i);
  }
}//end deserialize


void Serializer<UltraSoundReceiveData>::serialize(const UltraSoundReceiveData& representation, std::ostream& stream)
{
  naothmessages::UltraSoundReceiveData message;
  
  message.set_rawdata(representation.rawdata);

  for(int i = 0; i < representation.numOfUSEcho; i++)
  {
    message.add_dataleft(representation.dataLeft[i]);
    message.add_dataright(representation.dataRight[i]);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}//end serialize


void UltraSoundSendData::print(std::ostream& stream) const
{
  stream 
    << "UltraSoundSendData" << std::endl
    << "---------------------" << std::endl
    << "mode = " << mode << std::endl;
}
