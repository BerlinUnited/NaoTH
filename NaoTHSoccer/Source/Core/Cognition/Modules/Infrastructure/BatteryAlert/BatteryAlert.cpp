/**
 * @file BatteryAlert.cpp
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 */


#include "BatteryAlert.h"

BatteryAlert::BatteryAlert()
{

}

void BatteryAlert::execute()
{

    //since battery is lower than 6% repeat sound every 22s
    static double whenPlayed = getFrameInfo().getTimeInSeconds();

    if(getBatteryData().charge < 0.06 && getFrameInfo().getTimeInSeconds() - whenPlayed > 22) {

        whenPlayed = getFrameInfo().getTimeInSeconds();
        getSoundPlayData().mute = false;
        getSoundPlayData().soundFile = "battery_low.wav";

    } else {

        getSoundPlayData().mute = true;
        getSoundPlayData().soundFile = "";

    }


} // end execute

BatteryAlert::~BatteryAlert()
{
}
