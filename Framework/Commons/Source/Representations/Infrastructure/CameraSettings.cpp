#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/CameraInfo.h"

using namespace naoth;
using namespace std;

CameraSettings::CameraSettings()
{
  data[CameraSelection] = CameraInfo::Bottom;

  for (int i = 0; i < numOfCameraSetting; i++)
  {
    data[i] = 0;
  }
}

CameraSettings::~CameraSettings()
{
}

string CameraSettings::getCameraSettingsName(CameraSettingID id)
{
  switch (id)
  {
  case FPS:
    return "FPS";
    break;
  case ResolutionWidth:
    return "ResolutionWidth";
    break;
  case ResolutionHeight:
    return "ResolutionHeight";
    break;
  case CameraSelection:
    return "CameraSelection";
    break;
  default:
    return "Unknown CameraSetting";
    break;
  } //end switch
} //end getCameraSettingsName

void CameraSettings::print(ostream &stream) const
{
  stream <<  "AutoExposure = " << autoExposition << ";" << endl;
  stream <<  "Exposure = " << exposure << ";" << endl;
  stream <<  "Gain = " << gain << ";" << endl;
  stream <<  "Saturation = " << saturation << ";" << endl;
  stream <<  "WhiteBalanceTemperature = " << whiteBalanceTemperature << ";" << endl;

  for (int i = 0; i < numOfCameraSetting; i++)
  {
    stream << getCameraSettingsName((CameraSettingID)i) << " = " << data[i] << ";" << endl;
  }
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
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(v5.powerlineFrequency) = 50;

  setAutoExposureWeights(100);

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

  // set some default/constant values
  result.data[CameraSettings::FPS] = 30;
  result.data[CameraSettings::ResolutionHeight] = IMAGE_HEIGHT;
  result.data[CameraSettings::ResolutionWidth] = IMAGE_WIDTH;

  result.autoExposition = autoExposition;
  result.exposure = exposure;
  result.gain = gain;
  result.saturation = saturation;
  result.brightness = brightness;
  result.contrast = static_cast<float>(contrast);
  result.sharpness = sharpness;
  result.hue = hue;
  result.autoWhiteBalancing = autoWhiteBalancing;
  result.whiteBalanceTemperature = whiteBalanceTemperature;
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
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(autoExpositionMethod) = "averageY";

  PARAMETER_REGISTER(isActive) = false;

  syncWithConfig();
}
