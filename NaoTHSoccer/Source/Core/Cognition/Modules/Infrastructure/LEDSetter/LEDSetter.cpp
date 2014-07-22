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
  getLEDData().change = false;

  if(!getFrameRateCheckLEDRequest().ignore && (getFrameInfo().getFrameNumber() / 4) % 2 == 0)
  {
    // get all head LEDs from frame rate check
    copyMonoLEDData(getFrameRateCheckLEDRequest(), LEDData::EarRight0, LEDData::EarLeft324);
    copyMultiLEDData(getFrameRateCheckLEDRequest(), LEDData::FaceRight0, LEDData::FaceLeft315);
  }
  else
  {
    // head LEDs from behavior
    copyMonoLEDData(getBehaviorLEDRequest(), LEDData::EarRight0, LEDData::HeadRearRight2);
    copyMultiLEDData(getBehaviorLEDRequest(), LEDData::FaceRight0, LEDData::FaceLeft315);
  }
  // feet and chest button and head from GameController
  copyMultiLEDData(getGameControllerLEDRequest(), LEDData::FootLeft, LEDData::ChestButton);
  //copyMonoLEDData(getGameControllerLEDRequest(), LEDData::HeadFrontLeft0, LEDData::HeadRearRight2);

} // end execute

void LEDSetter::copyMultiLEDData(const LEDRequest &data, int from, int to)
{
  for(int i=from; i <= to; i++)
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

}

void LEDSetter::copyMonoLEDData(const LEDRequest &data, int from, int to)
{
  // head LEDs from behavior

  for(int i=from; i <= to; i++)
  {
    if(data.request.theMonoLED[i] != getLEDData().theMonoLED[i])
    {
      getLEDData().theMonoLED[i] = data.request.theMonoLED[i];
      getLEDData().change = true;
    }
  }

}//end copyData

LEDSetter::~LEDSetter()
{
}
