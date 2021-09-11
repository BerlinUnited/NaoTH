#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/CameraInfo.h"

using namespace naoth;
using namespace std;

CameraSettings::CameraSettings()
{
}

CameraSettings::~CameraSettings()
{
}


void CameraSettings::print(ostream &stream) const
{
  stream <<  "AutoExposure = " << autoExposition << ";" << endl;
  stream <<  "Exposure = " << exposure << ";" << endl;
  stream <<  "Gain = " << gain << ";" << endl;
  stream <<  "Saturation = " << saturation << ";" << endl;
  stream <<  "WhiteBalanceTemperature = " << v5.whiteBalanceTemperature << ";" << endl;

} //end print

CurrentCameraSettings::CurrentCameraSettings()
{
}

CurrentCameraSettingsTop::CurrentCameraSettingsTop()
{
}

CameraSettingsRequest::CameraSettingsRequest(string configName)
    : ParameterList(configName),
      queryCameraSettings(false)
{
  PARAMETER_REGISTER(autoExposition) = false;
  PARAMETER_REGISTER(v5.autoExpositionAlgorithm) = 0;
  PARAMETER_REGISTER(autoWhiteBalancing) = false;
  PARAMETER_REGISTER(backlightCompensation) = false;
  PARAMETER_REGISTER(brightness) = 55;
  PARAMETER_REGISTER(cameraSelection) = 0;
  PARAMETER_REGISTER(contrast) = 1.0;
  PARAMETER_REGISTER(exposure) = 1;
  PARAMETER_REGISTER(v5.fadeToBlack) = false;
  PARAMETER_REGISTER(gain) = 1.0;
  PARAMETER_REGISTER(v5.gammaCorrection) = 220;
  PARAMETER_REGISTER(v5.targetGain) = 5.0;
  PARAMETER_REGISTER(v5.minAnalogGain) = 1.0;
  PARAMETER_REGISTER(v5.maxAnalogGain) = 8.0;
  PARAMETER_REGISTER(horizontalFlip) = 0;
  PARAMETER_REGISTER(hue) = 0;
  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(verticalFlip) = 0;
  PARAMETER_REGISTER(v5.whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(v5.powerlineFrequency) = 50;

  setAutoExposureWeights(100);
  
  
  // V6
/*
                     brightness (int)    : min=-255 max=255 step=1 default=0 value=60
                       contrast (int)    : min=0 max=255 step=1 default=32 value=64
                     saturation (int)    : min=0 max=255 step=1 default=64 value=128
                            hue (int)    : min=-180 max=180 step=1 default=0 value=0
 white_balance_temperature_auto (bool)   : default=1 value=0
                           gain (int)    : min=0 max=1023 step=1 default=16 value=176
                       hue_auto (bool)   : default=0 value=0
      white_balance_temperature (int)    : min=2500 max=6500 step=500 default=2500 value=0
                      sharpness (int)    : min=0 max=9 step=1 default=4 value=0
                  exposure_auto (menu)   : min=0 max=3 default=0 value=1
              exposure_absolute (int)    : min=0 max=1048575 step=1 default=512 value=10000
                 focus_absolute (int)    : min=0 max=250 step=25 default=0 value=0
                     focus_auto (bool)   : default=0 value=0
                     brightness (int)    : min=-255 max=255 step=1 default=0 value=60
                       contrast (int)    : min=0 max=255 step=1 default=32 value=64
                     saturation (int)    : min=0 max=255 step=1 default=64 value=128
                            hue (int)    : min=-180 max=180 step=1 default=0 value=0
 white_balance_temperature_auto (bool)   : default=1 value=0
                           gain (int)    : min=0 max=1023 step=1 default=16 value=176
                       hue_auto (bool)   : default=0 value=0
      white_balance_temperature (int)    : min=2500 max=6500 step=500 default=2500 value=0
                      sharpness (int)    : min=0 max=9 step=1 default=4 value=0

*/
  PARAMETER_REGISTER(v6.autoExposition) = false;

  PARAMETER_REGISTER(v6.exposure) = 10000;
  PARAMETER_REGISTER(v6.saturation) = 64;

  PARAMETER_REGISTER(v6.autoWhiteBalancing) = false;
  PARAMETER_REGISTER(v6.gain) = 16;
  PARAMETER_REGISTER(v6.brightness) = 0;
  PARAMETER_REGISTER(v6.contrast) = 32;
  PARAMETER_REGISTER(v6.sharpness) = 4;
  PARAMETER_REGISTER(v6.hue) = 0;
  
  // initial values that "look" good in the lab
  PARAMETER_REGISTER(v6.gain_red) = 1280;
  PARAMETER_REGISTER(v6.gain_green) = 1024;
  PARAMETER_REGISTER(v6.gain_blue) = 2048;
  
  PARAMETER_REGISTER(v6.horizontalFlip) = false;
  PARAMETER_REGISTER(v6.verticalFlip) = false;

  syncWithConfig();
}

void CameraSettingsRequest::reset()
{
  setAutoExposureWeights(0);
}

void CameraSettingsRequest::setAutoExposureWeights(std::uint8_t w)
{
  for (std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++)
  {
    for (std::size_t j = 0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++)
    {
      autoExposureWeights[i][j] = w;
    }
  }
}

CameraSettings CameraSettingsRequest::getCameraSettings() const
{
  CameraSettings result;

  result.autoExposition = autoExposition;
  result.exposure = exposure;
  result.gain = gain;
  result.saturation = saturation;
  result.brightness = brightness;
  result.contrast = static_cast<float>(contrast);
  result.sharpness = sharpness;
  result.hue = hue;
  result.autoWhiteBalancing = autoWhiteBalancing;
  result.v5.whiteBalanceTemperature = v5.whiteBalanceTemperature;
  result.verticalFlip = verticalFlip;
  result.horizontalFlip = horizontalFlip;

  result.v5.targetGain = static_cast<float>(v5.targetGain);
  // make sure min/max gains are compatible with the choosen target gain
  result.v5.minAnalogGain = std::min(v5.minAnalogGain, result.v5.targetGain);
  result.v5.maxAnalogGain = std::max(v5.maxAnalogGain, result.v5.targetGain);
  result.v5.fadeToBlack = v5.fadeToBlack;
  result.v5.powerlineFrequency = v5.powerlineFrequency;
  result.v5.gammaCorrection = v5.gammaCorrection;
  
  result.backlightCompensation = backlightCompensation;

  // Convert each request to a proper setting and clamp values according to the driver documentation
  // (https://github.com/bhuman/BKernel#information-about-the-camera-driver).
  // The V4L controller might later adjust these values by the ranges reported by driver, but these
  // might be inaccurate or less restricted. Also, for fixed point real numbers the clipping should
  // be performed for the real number range, not the byte-representation.

  

  for (std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++)
  {
    for (std::size_t j = 0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++)
    {
      result.autoExposureWeights[i][j] = autoExposureWeights[i][j];
    }
  }
  
  
  // V6
  result.v6.autoExposition = v6.autoExposition;

  result.v6.exposure = v6.exposure;
  result.v6.saturation = v6.saturation;

  result.v6.autoWhiteBalancing = v6.autoWhiteBalancing;
  result.v6.gain = v6.gain;
  result.v6.brightness = v6.brightness;
  result.v6.contrast = v6.contrast;
  result.v6.sharpness = v6.sharpness;
  result.v6.hue = v6.hue;
  
  result.v6.gain_red = v6.gain_red;
  result.v6.gain_green = v6.gain_green;
  result.v6.gain_blue = v6.gain_blue;
  
  result.v6.horizontalFlip = v6.horizontalFlip;
  result.v6.verticalFlip = v6.verticalFlip;
  
  return result;
}

CameraSettingsRequestTop::CameraSettingsRequestTop()
    : CameraSettingsRequest("CameraSettingsTop")
{
}

CommonCameraSettingsRequest::CommonCameraSettingsRequest(string configName)
    : ParameterList(configName),
      queryCameraSettings(false)
{
  PARAMETER_REGISTER(autoExposition) = false;
  PARAMETER_REGISTER(autoWhiteBalancing) = false;
  PARAMETER_REGISTER(brightness) = 55;
  PARAMETER_REGISTER(exposure) = 1;
  PARAMETER_REGISTER(gain) = 1;

  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 2;
  PARAMETER_REGISTER(v5.whiteBalanceTemperature) = 6500;
  
  PARAMETER_REGISTER(autoExpositionMethod) = "averageY";

  PARAMETER_REGISTER(isActive) = false;

  // V6
  
  syncWithConfig();
}
