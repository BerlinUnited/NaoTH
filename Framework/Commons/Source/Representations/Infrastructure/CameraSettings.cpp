
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/CameraInfo.h"

using namespace naoth;
using namespace std;

CameraSettings::CameraSettings()
{
  data[CameraSelection] = CameraInfo::Bottom;
  
  for(int i=0;i<numOfCameraSetting;i++) {
    data[i] = 0;
  }
}

CameraSettings::~CameraSettings()
{ }

string CameraSettings::getCameraSettingsName(CameraSettingID id)
{
  switch(id)
  {
    case Brightness: return "Brightness"; break;
    case Contrast: return "Contrast"; break;
    case Saturation: return "Saturation"; break;
    case Hue: return "Hue"; break;
    case Gain: return "Gain"; break;
    case MinGain: return "MinGain"; break;
    case MaxGain: return "MaxGain"; break;
    case TargetGain: return "TargetGain"; break;
    case HorizontalFlip: return "HorizontalFlip"; break;
    case VerticalFlip: return "VerticalFlip"; break;
    case AutoExposition: return "AutoExposition"; break;
    case AutoWhiteBalancing: return "AutoWhiteBalancing"; break;
    case Exposure: return "Exposure"; break;
    case FPS: return "FPS"; break;
    case ResolutionWidth: return "ResolutionWidth"; break;
    case ResolutionHeight: return "ResolutionHeight"; break;
    case CameraSelection: return "CameraSelection"; break;
    case BacklightCompensation: return "BacklightCompensation"; break;
    case WhiteBalance: return "WhiteBalance"; break;
    case Sharpness: return "Sharpness"; break;
    case FadeToBlack: return "FadeToBlack"; break;
    case PowerlineFrequency: return "PowerlineFrequency"; break;
    default: return "Unknown CameraSetting"; break;
  }//end switch
}//end getCameraSettingsName


void CameraSettings::print(ostream& stream) const
{
  for(int i = 0; i < numOfCameraSetting; i++) {
     stream << getCameraSettingsName((CameraSettingID)i) << " = " << data[i] << ";" << endl;
  }
}//end print


CurrentCameraSettings::CurrentCameraSettings()
{}

CurrentCameraSettingsTop::CurrentCameraSettingsTop()
{}

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
  PARAMETER_REGISTER(fadeToBlack) = false;
  PARAMETER_REGISTER(gain) = 32;
  PARAMETER_REGISTER(targetGain) = 4.0;
  PARAMETER_REGISTER(minGain) = 1.0;
  PARAMETER_REGISTER(maxGain) = 8.0;
  PARAMETER_REGISTER(horizontalFlip) = 0;
  PARAMETER_REGISTER(hue) = 0;
  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(verticalFlip) = 0;
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;

  syncWithConfig();
}

CameraSettings CameraSettingsRequest::getCameraSettings() const {
  CameraSettings result;

  // set some default/constant values
  result.data[CameraSettings::FPS] = 30;
  result.data[CameraSettings::ResolutionHeight] = IMAGE_HEIGHT;
  result.data[CameraSettings::ResolutionWidth] = IMAGE_WIDTH;
  
  // Convert each request to a proper setting and clamp values according to the driver documentation
  // (https://github.com/bhuman/BKernel#information-about-the-camera-driver).
  // The V4L controller might later adjust these values by the ranges reported by driver, but these
  // might be inaccurate or less restricted. Also, for fixed point real numbers the clipping should
  // be performed for the real number range, not the byte-representation.

  result.data[CameraSettings::AutoExposition] = autoExposition ? 1 : 0;
  result.data[CameraSettings::AutoWhiteBalancing] = autoWhiteBalancing ? 1 : 0;
  result.data[CameraSettings::BacklightCompensation] = backlightCompensation ? 1 : 0;
  // use target brightness for both lightening conditions
  result.data[CameraSettings::Brightness] = Math::clamp(brightness, 0, 255);
  result.data[CameraSettings::BrightnessDark] = Math::clamp(brightness, 0, 255);
  result.data[CameraSettings::CameraSelection] = cameraSelection;
  result.data[CameraSettings::Contrast] = Math::toFixPoint<5>(static_cast<float>(Math::clamp(contrast, 0.5, 2.0)));
  result.data[CameraSettings::Exposure] = Math::clamp(exposure, 1, 1000);
  result.data[CameraSettings::FadeToBlack] = fadeToBlack ? 1 : 0;
  result.data[CameraSettings::Gain] = Math::clamp(gain, 0, 255);
  result.data[CameraSettings::TargetGain] = Math::toFixPoint<5>(static_cast<float>(targetGain));
  result.data[CameraSettings::MinGain] = Math::toFixPoint<5>(static_cast<float>(minGain));
  result.data[CameraSettings::MaxGain] = Math::toFixPoint<5>(static_cast<float>(maxGain));
  result.data[CameraSettings::HorizontalFlip] = horizontalFlip ? 1 : 0;
  result.data[CameraSettings::Hue] = Math::clamp(hue, -22, 22);
  result.data[CameraSettings::Saturation] = Math::clamp(saturation, 0, 255);
  result.data[CameraSettings::Sharpness] = Math::clamp(sharpness, -7, 7);
  result.data[CameraSettings::VerticalFlip] = verticalFlip ? 1 : 0;
  result.data[CameraSettings::WhiteBalance] = Math::clamp(whiteBalanceTemperature, 2700, 6500);

  result.data[CameraSettings::PowerlineFrequency] = 1;

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
  PARAMETER_REGISTER(gain) = 32;
  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(isActive) = false;

  syncWithConfig();
}


