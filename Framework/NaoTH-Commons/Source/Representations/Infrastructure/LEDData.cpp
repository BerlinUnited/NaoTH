
#include "Representations/Infrastructure/LEDData.h"

#include <string>

using namespace naoth;

LEDData::LEDData()
{
  for(int i=0;i<numOfMonoLED;i++)
  {
    theMonoLED[i] = 0.0;
  }

  for(int i=0;i<numOfMultiLED;i++)
  {
    for(int j=0;j<numOfLEDColor;j++)
    {
      theMultiLED[i][j] = 0.0;
    }
  }

  change = true;
}

void LEDData::print(ostream& stream) const
{
  for(int i=0; i < numOfMonoLED; i++)
  {
    stream << getLEDName((MonoLEDID) i) << ": " << theMonoLED[i] << std::endl;
  }
  for(int i=0; i < numOfMultiLED; i++)
  {
    stream << getLEDName((MultiLEDID) i)
    << ": " << theMultiLED[i][RED] << " "
    << theMultiLED[i][GREEN] << " " << theMultiLED[i][BLUE] << std::endl;
  }
}//end print

LEDData::~LEDData()
{
}

string LEDData::getLEDName(MonoLEDID led)
{
  switch(led)
  {
    case EarRight0: return string("EarRight0");
    case EarRight36: return string("EarRight36");
    case EarRight72: return string("EarRight72");
    case EarRight108: return string("EarRight108");
    case EarRight144: return string("EarRight144");
    case EarRight180: return string("EarRight180");
    case EarRight216: return string("EarRight216");
    case EarRight252: return string("EarRight252");
    case EarRight288: return string("EarRight288");
    case EarRight324: return string("EarRight324");
    case EarLeft0: return string("EarLeft0");
    case EarLeft36: return string("EarLeft36");
    case EarLeft72: return string("EarLeft72");
    case EarLeft108: return string("EarLeft108");
    case EarLeft144: return string("EarLeft144");
    case EarLeft180: return string("EarLeft180");
    case EarLeft216: return string("EarLeft216");
    case EarLeft252: return string("EarLeft256");
    case EarLeft288: return string("EarLeft288");
    case EarLeft324: return string("EarLeft324");
    default: return string("Unknown MonoLED");
  }//end switch
}//end getLEDName

string LEDData::getLEDName(MultiLEDID led)
{
  switch(led)
  {
    case FaceRight0: return string("FaceRight0");
    case FaceRight45: return string("FaceRight45");
    case FaceRight90: return string("FaceRight90");
    case FaceRight135: return string("FaceRight135");
    case FaceRight180: return string("FaceRight180");
    case FaceRight225: return string("FaceRight225");
    case FaceRight270: return string("FaceRight270");
    case FaceRight315: return string("FaceRight315");
    case FaceLeft0: return string("FaceLeft0");
    case FaceLeft45: return string("FaceLeft45");
    case FaceLeft90: return string("FaceLeft90");
    case FaceLeft135: return string("FaceLeft135");
    case FaceLeft180: return string("FaceLeft180");
    case FaceLeft225: return string("FaceLeft225");
    case FaceLeft270: return string("FaceLeft270");
    case FaceLeft315: return string("FaceLeft315");
    case FootLeft: return string("FootLeft");
    case FootRight: return string("FootRight");
    case ChestButton: return string("ChestButton");
    default: return string("Unknown MultiLED");
  }//end switch
}//end getLEDName
