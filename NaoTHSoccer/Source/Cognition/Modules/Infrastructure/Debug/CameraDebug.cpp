/**
* @file CameraDebug.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraDebug
*/

#include "CameraDebug.h"
#include <DebugCommunication/DebugCommandManager.h>
#include <Representations/Infrastructure/Configuration.h>
#include <PlatformInterface/Platform.h>

using namespace std;

CameraDebug::CameraDebug(): CameraDebugBase(),
  isAutoCalibratingCamera(false),
  afterAutoCalibratingCamera(false),
  afterQueryCameraSettings(false)
{  
  REGISTER_DEBUG_COMMAND("camera:switch_auto_parameters", "switch Automation for camera parameters", this);
  REGISTER_DEBUG_COMMAND("camera:force_reload", "force reading all parameters from camera", this);
  
  // register the CameraSettingsRequest as a parameter list
  getDebugParameterList().add(&(getCameraSettingsRequest()));
  getDebugParameterList().add(&(getCameraSettingsRequestTop()));
  getDebugParameterList().add(&(getCommonCameraSettingsRequest()));
}

CameraDebug::~CameraDebug()
{
  getDebugParameterList().remove(&(getCameraSettingsRequest()));
  getDebugParameterList().remove(&(getCameraSettingsRequestTop()));
  getDebugParameterList().remove(&(getCommonCameraSettingsRequest()));
}

void CameraDebug::execute()
{
  if(!getCommonCameraSettingsRequest().isActive)
  {
    // HACK: select camera by head motion request (of course in the representation itself...)
    getCameraSettingsRequest().data[CameraSettings::CameraSelection] = getHeadMotionRequest().cameraID;

    getCameraSettingsRequest().queryCameraSettings = false;

    if(isAutoCalibratingCamera)
    {
      //disabled auto exposure time setting
      getCameraSettingsRequest().data[CameraSettings::AutoExposition] = 1;
      getCameraSettingsRequest().data[CameraSettings::AutoWhiteBalancing] = 1;

      timeWhenCameraCalibrationStopped = getFrameInfo();
    }
    else if(afterAutoCalibratingCamera)
    {
      getCameraSettingsRequest().queryCameraSettings = true;
  
      // wait at least some frames
      if((getFrameInfo().getFrameNumber() - timeWhenCameraCalibrationStopped.getFrameNumber()) > 3)
      {
        afterAutoCalibratingCamera = false;
        afterQueryCameraSettings = true;
      }
    }
    else if(afterQueryCameraSettings)
    {
      afterQueryCameraSettings = false;
    
      // sync to the current settings
      for(int i=0; i < CameraSettings::numOfCameraSetting; i++) 
      {
        getCameraSettingsRequest().data[i] = getCurrentCameraSettings().data[i];
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
          getCommonCameraSettingsRequest().data[i] = getCurrentCameraSettings().data[i];
        }
      }

      // deactivate the auto stuff
      //disabled auto exposure time setting
      getCameraSettingsRequest().data[CameraSettings::AutoExposition] = 0;
      getCameraSettingsRequest().data[CameraSettings::AutoWhiteBalancing] = 0;

      naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
      getCameraSettingsRequest().saveToConfig();
      config.save();
      std::cout << "Camera parameter were saved after auto-calibration" << endl;
    }
  }
  else
  {
    getCameraSettingsRequest().data[CameraSettings::AutoExposition] = 0;
    getCameraSettingsRequest().data[CameraSettings::AutoWhiteBalancing] = 0;
    getCameraSettingsRequest().data[CameraSettings::Exposure] = getCommonCameraSettingsRequest().data[CameraSettings::Exposure];
    getCameraSettingsRequest().data[CameraSettings::Gain] = getCommonCameraSettingsRequest().data[CameraSettings::Gain];
    getCameraSettingsRequest().data[CameraSettings::Saturation] = getCommonCameraSettingsRequest().data[CameraSettings::Saturation];
    getCameraSettingsRequest().data[CameraSettings::Sharpness] = getCommonCameraSettingsRequest().data[CameraSettings::Sharpness];
    getCameraSettingsRequest().data[CameraSettings::WhiteBalance] = getCommonCameraSettingsRequest().data[CameraSettings::WhiteBalance];
    getCameraSettingsRequest().saveToConfig();
    getCameraSettingsRequestTop().data[CameraSettings::AutoExposition] = 0;
    getCameraSettingsRequestTop().data[CameraSettings::AutoWhiteBalancing] = 0;
    getCameraSettingsRequestTop().data[CameraSettings::Exposure] = getCommonCameraSettingsRequest().data[CameraSettings::Exposure];
    getCameraSettingsRequestTop().data[CameraSettings::Gain] = getCommonCameraSettingsRequest().data[CameraSettings::Gain];
    getCameraSettingsRequestTop().data[CameraSettings::Saturation] = getCommonCameraSettingsRequest().data[CameraSettings::Saturation];
    getCameraSettingsRequestTop().data[CameraSettings::Sharpness] = getCommonCameraSettingsRequest().data[CameraSettings::Sharpness];
    getCameraSettingsRequestTop().data[CameraSettings::WhiteBalance] = getCommonCameraSettingsRequest().data[CameraSettings::WhiteBalance];
    getCameraSettingsRequestTop().saveToConfig();
  }
  
}//end execute


void CameraDebug::executeDebugCommand(
  const std::string& command, 
  const std::map<std::string, std::string>& arguments,
  std::ostream & outstream)
{
  if (command == "camera:switch_auto_parameters")
  {
    if (!arguments.empty())
    {
      if (arguments.begin()->first == "on")
      {
        outstream << "switching auto parameters on" << endl;
        isAutoCalibratingCamera = true;
      }
      else if (arguments.begin()->first == "off")
      {
        outstream << "switching auto parameters off and saved" << endl;

        isAutoCalibratingCamera = false;
        afterAutoCalibratingCamera = true;
      }
      else
      {
        outstream << "don't know this parameter, choose either \"on\" or \"off\"" << endl;
      }
    }
  }
  else if(command == "camera:force_reload")
  {
    timeWhenCameraCalibrationStopped = getFrameInfo();
    afterAutoCalibratingCamera = true;
  }
}//end executeDebugCommand
