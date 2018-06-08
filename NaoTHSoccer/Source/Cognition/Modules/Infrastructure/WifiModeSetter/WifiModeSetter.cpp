#include "WifiModeSetter.h"

using namespace naoth;

void WifiModeSetter::execute() {
    const ButtonEvent& chest = getButtonState()[ButtonState::Chest];
    if(chest.eventState == ButtonEvent::CLICKED && chest.clicksInSequence == 2) {
        // switch state
        getWifiMode().wifiEnabled = !getWifiMode().wifiEnabled;
    }

    // if Wifi is disabled, show a blinking chest button to get the attention of the robot handler
    getWifiModeSetterLEDRequest().ignore = true;
    if(!getWifiMode().wifiEnabled && getFrameInfo().getFrameNumber() % 15 == 0 ) {

        getWifiModeSetterLEDRequest().ignore = false;

        getWifiModeSetterLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
        getWifiModeSetterLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 1.0;
        getWifiModeSetterLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 1.0;
    }
}