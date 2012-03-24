#include "Representations/Infrastructure/UltraSoundData.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>


using namespace naoth;

UltraSoundData::UltraSoundData()
  : ultraSoundTimeStep(100)
{
}

UltraSoundData::~UltraSoundData()
{
  
}

UltraSoundReceiveData::UltraSoundReceiveData()
{
  rawdata = 2.55;
  ultraSoundTimeStep = 10;
  init();
}

void UltraSoundReceiveData::init()
{
  rawdata = 2.55;
  for(int i = 0; i < numOfUSEcho; i++)
  {
    dataLeft[i] = 0.0;
    dataRight[i] = 0.0;
  }
}

void UltraSoundReceiveData::print(ostream& stream) const
{
  stream
    << "UltraSoundReceiveData" << std::endl
    << "---------------------" << std::endl
    << "rawdata = " << rawdata << std::endl;
  for(int i = 0; i < numOfUSEcho; i++)
  {
    stream << "data[" << i << "]: left = " << dataLeft[i] << " , "  << "right = " << dataRight[i] << std::endl;
  }
}

UltraSoundReceiveData::~UltraSoundReceiveData()
{

}



void Serializer<UltraSoundReceiveData>::deserialize(std::istream& stream, UltraSoundReceiveData& representation)
{
  naothmessages::UltraSoundReceiveData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.ultraSoundTimeStep = message.ultrasoundtimestep();
  representation.rawdata = message.rawdata();

  ASSERT(message.dataleft_size() == message.dataright_size());
  ASSERT(message.dataleft_size() == representation.numOfUSEcho);

  for(int i = 0; i < message.dataleft_size(); i++)
  {
    representation.dataLeft[i] = message.dataleft(i);
    representation.dataRight[i] = message.dataright(i);
  }//end for
}//end deserialize


void Serializer<UltraSoundReceiveData>::serialize(const UltraSoundReceiveData& representation, std::ostream& stream)
{
  naothmessages::UltraSoundReceiveData message;
  
  message.set_ultrasoundtimestep(representation.ultraSoundTimeStep);
  message.set_rawdata(representation.rawdata);

  for(int i = 0; i < representation.numOfUSEcho; i++)
  {
    message.add_dataleft(representation.dataLeft[i]);
    message.add_dataright(representation.dataRight[i]);
  }//end for

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializePartialToZeroCopyStream(&buf);
}//end serialize





UltraSoundSendData::UltraSoundSendData()
{
  mode = 1;
}

void UltraSoundSendData::setMode(unsigned int _mode)
{
  switch(_mode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      mode = _mode;
      break;

    case 4:
    case 12:
      mode = _mode;
      break;
  }

  if(_mode & 64 == 1)
  {
    ultraSoundTimeStep = 100;
  }
  else
  {
    ultraSoundTimeStep = 10;
  }
}


void UltraSoundSendData::print(ostream& stream) const
{
  stream 
    << "UltraSoundSendData" << std::endl
    << "---------------------" << std::endl
    << "mode = " << mode << std::endl;
}

UltraSoundSendData::~UltraSoundSendData()
{

}
