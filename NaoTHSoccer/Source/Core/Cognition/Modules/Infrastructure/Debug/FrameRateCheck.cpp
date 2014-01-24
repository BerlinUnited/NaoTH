#include "FrameRateCheck.h"

FrameRateCheck::FrameRateCheck()
{
}

FrameRateCheck::~FrameRateCheck()
{
}

void FrameRateCheck::execute()
{
  getFrameRateCheckLEDRequest().ignore = true;
  // TODO: which framerate should be reached?
  if(getFrameInfo().getTimeSince(lastFrameInfo.getTime()) > 40)
  {
    getFrameRateCheckLEDRequest().ignore = false;
    LEDData& ledData = getFrameRateCheckLEDRequest().request;
    ledData.change=true;
    for(int i=0; i < LEDData::numOfMonoLED; i++)
    {
      ledData.theMonoLED[i] = 1.0;
    }
    for(int i=0; i < LEDData::numOfMultiLED; i++)
    {
      for(int c=0; c < LEDData::numOfLEDColor; c++)
      {
        ledData.theMultiLED[i][c] = 1.0;
      }
    }
  }
  lastFrameInfo = getFrameInfo();
}


