#include "WifiModeSetter.h"

using namespace naoth;

void WifiModeSetter::execute() {

    // reset any old sound request
    getSoundPlayData().mute = true;
    getSoundPlayData().soundFile = "";
    

    const ButtonEvent& chest = getButtonState()[ButtonState::Chest];
    if(chest.isDoubleClick()) {
        // switch state
        getWifiMode().wifiEnabled = !getWifiMode().wifiEnabled;
        // play sound so the handler knows something changed
        getSoundPlayData().mute = false;
        getSoundPlayData().soundFile = getWifiMode().wifiEnabled ? "wifi_enabled.wav" : "wifi_disabled.wav";
    }

    // if Wifi is disabled, show a blinking chest button to get the attention of the robot handler
    getWifiModeSetterLEDRequest().ignore = true;

    if(!getWifiMode().wifiEnabled) {
        unsigned int currentFrame = getFrameInfo().getFrameNumber();

        // start blinking every 30th frame
        if(currentFrame % 30 == 0) {
            frameWhenColorChanged = getFrameInfo().getFrameNumber();
            
            // invert the previous chest button request
            getWifiModeSetterLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::RED] = 
                1.0 - getLEDData().theMultiLED[LEDData::ChestButton][LEDData::RED];
            getWifiModeSetterLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 
                1.0 - getLEDData().theMultiLED[LEDData::ChestButton][LEDData::GREEN];
            getWifiModeSetterLEDRequest().request.theMultiLED[LEDData::ChestButton][LEDData::BLUE] =
                1.0 - getLEDData().theMultiLED[LEDData::ChestButton][LEDData::BLUE];
        }

        if(currentFrame >= frameWhenColorChanged && currentFrame < frameWhenColorChanged + 5) {
            // show the color for 4 frames
            getWifiModeSetterLEDRequest().ignore = false;
        }
    }
}