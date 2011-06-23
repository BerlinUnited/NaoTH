#include "Representations/Infrastructure/CameraSettings.h"
#include "PlatformInterface/Platform.h"

using namespace naoth;

CameraSettings::CameraSettings()
{
  data[CameraSelection] = CameraInfo::Bottom;
  
  for(int i=0;i<numOfCameraSetting;i++)
  {
    data[i] = 0;
  }//end for
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
    case RedChroma: return "RedChroma"; break;
    case BlueChroma: return "BlueChroma"; break;
    case Gain: return "Gain"; break;
    case HorizontalFlip: return "HorizontalFlip"; break;
    case VerticalFlip: return "VerticalFlip"; break;
    case CorrectionLensX: return "CorrectionLensX"; break;
    case CorrectionLensY: return "CorrectionLensY"; break;
    case AutoExposition: return "AutoExposition"; break;
    case AutoWhiteBalancing: return "AutoWhiteBalancing"; break;
    case AutoGain: return "AutoGain"; break;
    case Exposure: return "Exposure"; break;
    case ExposureCorrection: return "ExposureCorrection"; break;
    case FPS: return "FPS"; break;
    case Resolution: return "Resolution"; break;
    case CameraSelection: return "CameraSelection"; break;
    default: return "Unknown CameraSetting"; break;
  }//end switch
}//end getCameraSettingsName


void CameraSettings::print(ostream& stream) const
{
  for(int i = 0; i < numOfCameraSetting; i++)
  {
     stream << getCameraSettingsName((CameraSettingID)i) << " = " << data[i] << ";" << endl;
  }//end for
}//end print


CameraSettingsRequest::CameraSettingsRequest()
: ParameterList("CameraSettings"),
  queryCameraSettings(false)
{
  for(int i=0;i<numOfCameraSetting;i++)
  {
    registerParameter(getCameraSettingsName((CameraSettingID) i), data[i]);
  }//end for
  
  loadFromConfig();
}

CurrentCameraSettings::CurrentCameraSettings()
{

}

