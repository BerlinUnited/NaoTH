
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
    case BrightnessDark: return "BrightnessDark"; break;
    case Contrast: return "Contrast"; break;
    case Saturation: return "Saturation"; break;
    case Hue: return "Hue"; break;
    case Gain: return "Gain"; break;
    case MinAnalogGain: return "MinAnalogGain"; break;
    case MaxAnalogGain: return "MaxAnalogGain"; break;
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
  PARAMETER_REGISTER(gain) = 1.0;
  PARAMETER_REGISTER(targetGain) = 100.0;
  PARAMETER_REGISTER(minAnalogGain) = 1.0;
  PARAMETER_REGISTER(maxAnalogGain) = 8.0;
  PARAMETER_REGISTER(horizontalFlip) = 0;
  PARAMETER_REGISTER(hue) = 0;
  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(verticalFlip) = 0;
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(powerlineFrequency) = 50;

  setAutoExposureWeights(100);

  syncWithConfig();
}

void CameraSettingsRequest::reset() {
  setAutoExposureWeights(0);
}

void CameraSettingsRequest::setAutoExposureWeights(std::uint8_t w) {
  for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
      for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
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
  result.data[CameraSettings::MinAnalogGain] = Math::clamp(Math::toFixPoint<5>(static_cast<float>(minAnalogGain)), 0, 32767);
  result.data[CameraSettings::MaxAnalogGain] = Math::clamp(Math::toFixPoint<5>(static_cast<float>(maxAnalogGain)), 0, 32767);

  // manual gain must be inside the min/max given in the other parameters and also can't be larger than 255 (as integer)
  result.data[CameraSettings::Gain] = Math::clamp(
    Math::toFixPoint<5>(static_cast<float>(Math::clamp(gain, minAnalogGain, maxAnalogGain))), 0, 255);
  
  result.data[CameraSettings::TargetGain] = Math::toFixPoint<5>(static_cast<float>(Math::clamp(targetGain, minAnalogGain, maxAnalogGain)));
  

  result.data[CameraSettings::HorizontalFlip] = horizontalFlip ? 1 : 0;
  result.data[CameraSettings::Hue] = Math::clamp(hue, -22, 22);
  result.data[CameraSettings::Saturation] = Math::clamp(saturation, 0, 255);
  result.data[CameraSettings::Sharpness] = Math::clamp(sharpness, -7, 7);
  result.data[CameraSettings::VerticalFlip] = verticalFlip ? 1 : 0;
  result.data[CameraSettings::WhiteBalance] = Math::clamp(whiteBalanceTemperature, 2700, 6500);

  // use 50 Hz (val = 1) if 60 Hz (val = 2) is not explicitly requested
  result.data[CameraSettings::PowerlineFrequency] = powerlineFrequency == 60 ? 2 : 1;

  
  for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
      for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
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
  PARAMETER_REGISTER(targetGain) = 100.0;
  PARAMETER_REGISTER(minAnalogGain) = 1.0;
  PARAMETER_REGISTER(maxAnalogGain) = 8.0;

  PARAMETER_REGISTER(saturation) = 128;
  PARAMETER_REGISTER(sharpness) = 128;
  PARAMETER_REGISTER(whiteBalanceTemperature) = 6500;
  PARAMETER_REGISTER(autoExpositionMethod) = "averageY";
  PARAMETER_REGISTER(powerlineFrequency) = 50;

  PARAMETER_REGISTER(isActive) = false;

  syncWithConfig();
}


