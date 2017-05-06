/**
 * @file BatteryAlert.cpp
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 */

#include "BatteryAlert.h"

BatteryAlert::BatteryAlert()
  : timeWhenLastPlayedSound(0)
{
}

void BatteryAlert::execute()
{
  const double minimalCharge = 0.06; // 6%
  const double soundRepeatInterval = 22000; // 22s

  //if the battery is lower than 6% repeat sound every 22s
  if(getBatteryData().charge < minimalCharge &&  
     getBodyState().isDischarging && // only make noise it not charging
     getFrameInfo().getTime() > timeWhenLastPlayedSound + soundRepeatInterval) 
  {
    timeWhenLastPlayedSound = getFrameInfo().getTime();
    getSoundPlayData().mute = false;
    getSoundPlayData().soundFile = "battery_low.wav";
  } 
  else if (getSoundPlayData().soundFile == "battery_low.wav")
  {
    getSoundPlayData().mute = true;
    getSoundPlayData().soundFile = "";
  }
} // end execute


