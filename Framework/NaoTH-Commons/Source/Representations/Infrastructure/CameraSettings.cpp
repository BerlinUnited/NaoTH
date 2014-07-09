
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
{ 
}

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


CameraSettingsRequest::CameraSettingsRequest(string configName)
: ParameterList(configName),
  queryCameraSettings(false)
{
  for(int i=0;i<numOfCameraSetting;i++) {
    registerParameter(getCameraSettingsName((CameraSettingID) i), data[i]);
  }

  syncWithConfig();
}

CurrentCameraSettings::CurrentCameraSettings()
{

}

CurrentCameraSettingsTop::CurrentCameraSettingsTop()
{

}

CameraSettingsRequestTop::CameraSettingsRequestTop()
  : CameraSettingsRequest("CameraSettingsTop")
{
}

