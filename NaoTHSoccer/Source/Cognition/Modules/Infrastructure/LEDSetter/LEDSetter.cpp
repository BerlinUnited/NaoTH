/* 
 * File:   LEDSetter.cpp
 * Author: thomas
 * 
 * Created on 22. April 2009, 16:55
 */

#include "LEDSetter.h"

void LEDSetter::execute()
{
  getLEDData().change = false;

  // 1. FrameRateCheck and Behavior
  //   - Ears
  //   - Eyes
  // NOTE: FrameRateCheck overrides Behavior if not explicitly disabled
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

  // 2. GameController: feet and chest button and head from GameController
  //  - chest button
  //  - head
  copyMultiLEDData(getGameControllerLEDRequest(), LEDData::FootLeft, LEDData::ChestButton);
  // NOTE: GameController shows kickoff state in initial, ready and set
  copyMonoLEDData(getGameControllerLEDRequest(), LEDData::HeadFrontLeft0, LEDData::HeadRearRight2);


  // 3. WifiModeSetter
  //  - chest button
  if(!getWifiModeSetterLEDRequest().ignore) {
    // Wifi status can override chest button from GameController
    copyMultiLEDData(getWifiModeSetterLEDRequest(), LEDData::ChestButton, LEDData::ChestButton);
  }

} // end execute

void LEDSetter::copyMultiLEDData(const LEDRequest &data, int from, int to)
{
  for(int i = from; i <= to; i++)
  {
    for(int c = LEDData::RED; c < LEDData::numOfLEDColor; ++c) {
      // check for all colors
      if(data.request.theMultiLED[i][c] != getLEDData().theMultiLED[i][c])
      {
        getLEDData().theMultiLED[i][c] = data.request.theMultiLED[i][c];
        getLEDData().change = true;
      }
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
