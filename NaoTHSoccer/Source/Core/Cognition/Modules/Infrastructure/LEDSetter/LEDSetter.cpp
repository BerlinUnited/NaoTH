/* 
 * File:   LEDSetter.cpp
 * Author: thomas
 * 
 * Created on 22. April 2009, 16:55
 */

#include "LEDSetter.h"

LEDSetter::LEDSetter()
{

}

void LEDSetter::execute()
{
  copyData(getGameControllerLEDRequest());
} // end execute

void LEDSetter::copyData(const LEDRequest &data)
{
  getLEDData().change = false;
  for(int i=0; i < LEDData::numOfMonoLED; i++)
  {
    if(data.request.theMonoLED[i] != getLEDData().theMonoLED[i])
    {
      getLEDData().theMonoLED[i] = data.request.theMonoLED[i];
      getLEDData().change = true;
    }
  }

  for(int i=0; i < LEDData::numOfMultiLED; i++)
  {
    if(data.request.theMultiLED[i][LEDData::RED]
      != getLEDData().theMultiLED[i][LEDData::RED])
    {
      getLEDData().theMultiLED[i][LEDData::RED] =
       data.request.theMultiLED[i][LEDData::RED];
      getLEDData().change = true;
    }

    if(data.request.theMultiLED[i][LEDData::BLUE]
      != getLEDData().theMultiLED[i][LEDData::BLUE])
    {
      getLEDData().theMultiLED[i][LEDData::BLUE] =
        data.request.theMultiLED[i][LEDData::BLUE];
      getLEDData().change = true;
    }

    if(data.request.theMultiLED[i][LEDData::GREEN]
      != getLEDData().theMultiLED[i][LEDData::GREEN])
    {
      getLEDData().theMultiLED[i][LEDData::GREEN] =
        data.request.theMultiLED[i][LEDData::GREEN];
      getLEDData().change = true;
    }
  }

}//end copyData

LEDSetter::~LEDSetter()
{
}
