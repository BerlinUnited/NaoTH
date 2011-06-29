#include "Representations/Infrastructure/UltraSoundData.h"

using namespace naoth;

UltraSoundData::UltraSoundData()
{
  ultraSoundTimeStep = 10;
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
  for(int i = 0; i < numOfIRSend; i++)
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
  for(int i = 0; i < numOfIRSend; i++)
  {
    stream << "data[" << i << "]: left = " << dataLeft[i] << " , "  << "right = " << dataRight[i] << std::endl;
  }
}

UltraSoundReceiveData::~UltraSoundReceiveData()
{

}

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
      //receiver.ultraSoundTimeStep = 10;
      break;

    case 4:
    case 12:
      mode = _mode;
      //receiver.ultraSoundTimeStep = 100;
      break;
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
