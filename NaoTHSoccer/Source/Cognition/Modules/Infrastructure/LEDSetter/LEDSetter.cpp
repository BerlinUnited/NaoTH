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

  // 1. Behavior
  //   - ears
  //   - head
  //   - eyes
  copyMonoLEDData(getBehaviorLEDRequest(), LEDData::EarRight0, LEDData::EarLeft324);
  copyMonoLEDData(getBehaviorLEDRequest(), LEDData::HeadFrontLeft0, LEDData::HeadRearRight2);
  copyMultiLEDData(getBehaviorLEDRequest(), LEDData::FaceRight0, LEDData::FaceLeft315);

  // 2. GameController: feet and chest button and head from GameController
  //  - chest button
  //  - head
  copyMultiLEDData(getGameControllerLEDRequest(), LEDData::FootLeft, LEDData::ChestButton);
  // NOTE: GameController shows kickoff state in initial, ready and set
  // TODO: Behavior uses the head to visualize striker and charging state, which colides with this.
  //copyMonoLEDData(getGameControllerLEDRequest(), LEDData::HeadFrontLeft0, LEDData::HeadRearRight2);

  // 3. WifiModeSetter
  //  - chest button
  if(!getWifiModeSetterLEDRequest().ignore) {
    // Wifi status can override chest button from GameController
    copyMultiLEDData(getWifiModeSetterLEDRequest(), LEDData::ChestButton, LEDData::ChestButton);
  }

  // 4. FrameRateCheck
  //   - ears
  //   - head
  //   - eyes
  // NOTE: FrameRateCheck requests all LEDs to be set to white, but only ears, head and eyes are copied here
  if(!getFrameRateCheckLEDRequest().ignore)
  {
    // get all head LEDs from frame rate check
    copyMonoLEDData(getFrameRateCheckLEDRequest(), LEDData::EarRight0, LEDData::EarLeft324);
    copyMonoLEDData(getFrameRateCheckLEDRequest(), LEDData::HeadFrontLeft0, LEDData::HeadRearRight2);

    // copy both eyes
    copyMultiLEDData(getFrameRateCheckLEDRequest(), LEDData::FaceRight0, LEDData::FaceLeft315);
  }

} // end execute

void LEDSetter::copyMultiLEDData(const LEDRequest& data, int from, int to)
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

void LEDSetter::copyMonoLEDData(const LEDRequest& data, int from, int to)
{
  for(int i=from; i <= to; i++)
  {
    if(data.request.theMonoLED[i] != getLEDData().theMonoLED[i])
    {
      getLEDData().theMonoLED[i] = data.request.theMonoLED[i];
      getLEDData().change = true;
    }
  }
}//end copyData
