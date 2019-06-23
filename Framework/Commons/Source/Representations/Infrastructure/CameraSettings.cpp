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
  case Brightness:
    return "Brightness";
    break;
  case Contrast:
    return "Contrast";
    break;
  case Hue:
    return "Hue";
    break;
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
  case BacklightCompensation:
    return "BacklightCompensation";
    break;
  case Sharpness:
    return "Sharpness";
    break;
  case AutoExpositionAlgorithm:
    return "AutoExpositionAlgorithm";
    break;
  case GammaCorrection:
    return "GammaCorrection";
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
  PARAMETER_REGISTER(autoExpositionAlgorithm) = 0;
  PARAMETER_REGISTER(autoWhiteBalancing) = false;
  PARAMETER_REGISTER(backlightCompensation) = false;
  PARAMETER_REGISTER(brightness) = 55;
  PARAMETER_REGISTER(cameraSelection) = 0;
  PARAMETER_REGISTER(contrast) = 1.0;
  PARAMETER_REGISTER(exposure) = 1;
  PARAMETER_REGISTER(v5_fadeToBlack) = false;
  PARAMETER_REGISTER(gain) = 1.0;
  PARAMETER_REGISTER(gammaCorrection) = 220;
  PARAMETER_REGISTER(v5_targetGain) = 5.0;
  PARAMETER_REGISTER(v5_minAnalogGain) = 1.0;
  PARAMETER_REGISTER(v5_maxAnalogGain) = 8.0;
  PARAMETER_REGISTER(horizontalFlip) = 0;
  PARAMETER_REGISTER(hue) = 0;
  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(verticalFlip) = 0;
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(v5_powerlineFrequency) = 50;

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

CameraSettings CameraSettingsRequest::getCameraSettings(bool isV6) const
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
  result.autoWhiteBalancing = autoWhiteBalancing;
  result.whiteBalanceTemperature = whiteBalanceTemperature;
  result.verticalFlip = verticalFlip;
  result.horizontalFlip = horizontalFlip;

  result.v5_targetGain = static_cast<float>(v5_targetGain);
  // make sure min/max gains are compatible with the choosen target gain
  result.v5_minAnalogGain = std::min(static_cast<float>(v5_minAnalogGain), result.v5_targetGain);
  result.v5_maxAnalogGain = std::max(static_cast<float>(v5_maxAnalogGain), result.v5_targetGain);
  result.v5_fadeToBlack = v5_fadeToBlack;
  result.v5_powerlineFrequency = v5_powerlineFrequency;

  // Convert each request to a proper setting and clamp values according to the driver documentation
  // (https://github.com/bhuman/BKernel#information-about-the-camera-driver).
  // The V4L controller might later adjust these values by the ranges reported by driver, but these
  // might be inaccurate or less restricted. Also, for fixed point real numbers the clipping should
  // be performed for the real number range, not the byte-representation.

  result.data[CameraSettings::BacklightCompensation] = backlightCompensation ? 1 : 0;
  // use target brightness for both lightening conditions
  result.data[CameraSettings::Brightness] = Math::clamp(brightness, isV6 ? -255 : 0, 255);
  result.data[CameraSettings::CameraSelection] = cameraSelection;
  if (isV6)
  {
    result.data[CameraSettings::Contrast] = Math::clamp(static_cast<int>(contrast), 0, 255);
  }
  else
  {
    result.data[CameraSettings::Contrast] = Math::toFixPoint<5>(static_cast<float>(Math::clamp(contrast, 0.5, 2.0)));
  }

  if (isV6)
  {
    result.data[CameraSettings::Hue] = Math::clamp(hue, -180, 180);
  }
  else
  {
    result.data[CameraSettings::Hue] = Math::clamp(hue, -22, 22);
  }
  if (isV6)
  {
    result.data[CameraSettings::Sharpness] = Math::clamp(sharpness, 0, 9);
  }
  else
  {
    result.data[CameraSettings::Sharpness] = Math::clamp(sharpness, -7, 7);
  }
  result.data[CameraSettings::GammaCorrection] = Math::clamp(gammaCorrection, 100, 280);


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
  PARAMETER_REGISTER(gammaCorrection) = 220;
  PARAMETER_REGISTER(v5_targetGain) = 5.0;
  PARAMETER_REGISTER(v5_minAnalogGain) = 1.0;
  PARAMETER_REGISTER(v5_maxAnalogGain) = 8.0;

  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(autoExpositionMethod) = "averageY";
  PARAMETER_REGISTER(v5_powerlineFrequency) = 50;

  PARAMETER_REGISTER(isActive) = false;

  syncWithConfig();
}
