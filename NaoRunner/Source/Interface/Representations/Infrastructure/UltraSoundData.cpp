#include "Representations/Infrastructure/UltraSoundData.h"

UltraSoundData::UltraSoundData()
{
  data = 0.0;
  ultraSoundTimeStep = 10;
}

UltraSoundData::~UltraSoundData()
{
  
}

UltraSoundReceiveData::UltraSoundReceiveData()
{
  ultraSoundTimeStep = 100;
  init();
}

void UltraSoundReceiveData::init()
{
  data = 0.0;
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
    << "data = " << data << std::endl;
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
  data = 4.0;
}

void UltraSoundSendData::setMode(unsigned int mode, UltraSoundReceiveData& receiver)
{
  receiver.init();
  switch(mode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      data = mode;
      receiver.ultraSoundTimeStep = 10;
      break;

    case 4:
    case 12:
      data = mode;
      receiver.ultraSoundTimeStep = 100;
      break;
  }
}


void UltraSoundSendData::print(ostream& stream) const
{
  stream 
    << "UltraSoundSendData" << std::endl
    << "---------------------" << std::endl
    << "data = " << data << std::endl;
}

UltraSoundSendData::~UltraSoundSendData()
{

}
