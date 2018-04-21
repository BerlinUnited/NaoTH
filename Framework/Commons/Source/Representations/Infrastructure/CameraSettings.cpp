
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
  for(int i=0;i<numOfCameraSetting;i++) {
    registerParameter(getCameraSettingsName((CameraSettingID) i), data[i]);
  }
  syncWithConfig();
}

CameraSettings CameraSettingsRequest::getCameraSettings() {
  CameraSettings result;

  // set some default/constant values
  result.data[CameraSettings::FPS] = 30;
  result.data[CameraSettings::ResolutionHeight] = IMAGE_HEIGHT;
  result.data[CameraSettings::ResolutionWidth] = IMAGE_WIDTH;
  
  // Convert each request to a proper setting and clamp values according to the driver documentation
  // (https://github.com/bhuman/BKernel#information-about-the-camera-driver).
  // The V4L controller might later adjust these values by the ranges reported by driver, but these
  // might be inaccurate or less restricted.

  result.data[CameraSettings::AutoExposition] = autoExposition ? 1 : 0;
  result.data[CameraSettings::AutoWhiteBalancing] = autoWhiteBalancing ? 1 : 0;
  result.data[CameraSettings::BacklightCompensation] = backlightCompensation ? 1 : 0;
  result.data[CameraSettings::Brightness] = Math::clamp(brightness, 0, 255);
  result.data[CameraSettings::CameraSelection] = cameraSelection;
  // TODO: contrast is actually a fixed point floating number
  result.data[CameraSettings::Contrast] = Math::clamp(contrast, 16, 64);
  result.data[CameraSettings::Exposure] = Math::clamp(exposure, 1, 1000);
  result.data[CameraSettings::FadeToBlack] = fadeToBlack ? 1 : 0;
  result.data[CameraSettings::Gain] = Math::clamp(gain, 0, 255);
  result.data[CameraSettings::HorizontalFlip] = horizontalFlip ? 1 : 0;
  result.data[CameraSettings::Hue] = Math::clamp(hue, -22, 22);
  result.data[CameraSettings::Saturation] = Math::clamp(saturation, 0, 255);
  result.data[CameraSettings::Sharpness] = Math::clamp(sharpness, -7, 7);
  result.data[CameraSettings::VerticalFlip] = verticalFlip ? 1 : 0;
  result.data[CameraSettings::WhiteBalance] = Math::clamp(whiteBalanceTemperature, 2700, 6500);

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
  for(int i=0;i<numOfCameraSetting;i++) 
  {
    CameraSettings::CameraSettingID id = (CameraSettings::CameraSettingID) i;
    if
    (
      id == CameraSettings::Gain || 
      id == CameraSettings::Exposure || 
      id == CameraSettings::Saturation || 
      id == CameraSettings::Sharpness ||
      id == CameraSettings::WhiteBalance
    )
    {
      registerParameter(getCameraSettingsName(id), data[i]);
    }
  }

  PARAMETER_REGISTER(isActive) = false;

  syncWithConfig();
}


