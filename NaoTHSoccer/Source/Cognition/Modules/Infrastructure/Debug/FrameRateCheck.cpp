#include "FrameRateCheck.h"


void FrameRateCheck::execute()
{
  getFrameRateCheckLEDRequest().ignore = true;
  getFrameRateCheckLEDRequest().request.change = false;

  // TODO: which framerate should be reached?
  if(getFrameInfo().getTimeSince(lastFrameInfo.getTime()) > 40 && (getFrameInfo().getFrameNumber() / 4) % 2 == 0)
  {
    getFrameRateCheckLEDRequest().ignore = false;
    getFrameRateCheckLEDRequest().request.change = true;

    // set all mono leds to maximal value
    getFrameRateCheckLEDRequest().request.setHead(1.0);
    getFrameRateCheckLEDRequest().request.setEarLeft(1.0);
    getFrameRateCheckLEDRequest().request.setEarRight(1.0);

    // set eyes to white
    getFrameRateCheckLEDRequest().request.setEyeLeft(1.0, 1.0, 1.0);
    getFrameRateCheckLEDRequest().request.setEyeRight(1.0, 1.0, 1.0);

    // feet and the button are not used to conform with the rules.
    //getFrameRateCheckLEDRequest().request.setFootLeft(1.0, 1.0, 1.0);
    //getFrameRateCheckLEDRequest().request.setFootRight(1.0, 1.0, 1.0);
    //getFrameRateCheckLEDRequest().request.setChestButton(1.0, 1.0, 1.0);
  }

  lastFrameInfo = getFrameInfo();
}


