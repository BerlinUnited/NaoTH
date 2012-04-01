/**
* @file CameraDebug.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraDebug
*/

#include "CameraDebug.h"
#include <DebugCommunication/DebugCommandManager.h>

#include "Tools/Debug/DebugParameterList.h"
#include "PlatformInterface/Platform.h"

CameraDebug::CameraDebug(): CameraDebugBase(),
  isAutoCalibratingCamera(false),
  afterAutoCalibratingCamera(false),
  afterQueryCameraSettings(false)
{  
  REGISTER_DEBUG_COMMAND("camera:switch_auto_parameters",
    "switch Automation for camera parameters", this);
  //DebugParameterList::getInstance().add(&getCameraSettingsRequest());
}

void CameraDebug::execute()
{

  // HACK: select camera by head motion request (of course in the representation itself...)
  getCameraSettingsRequest().data[CameraSettings::CameraSelection] = getHeadMotionRequest().cameraID;

  getCameraSettingsRequest().queryCameraSettings = false;

  if(isAutoCalibratingCamera)
  {
    //disabled auto exposure time setting
    //getCameraSettingsRequest().data[CameraSettings::AutoExposition] = 1;
    getCameraSettingsRequest().data[CameraSettings::AutoGain] = 1;
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
    }

    // hack again
    getCameraSettingsRequest().data[CameraSettings::VerticalFlip] = 0;
    getCameraSettingsRequest().data[CameraSettings::HorizontalFlip] = 0;

    // deactivate the auto stuff
    //disabled auto exposure time setting
    //getCameraSettingsRequest().data[CameraSettings::AutoExposition] = 0;
    getCameraSettingsRequest().data[CameraSettings::AutoGain] = 0;
    getCameraSettingsRequest().data[CameraSettings::AutoWhiteBalancing] = 0;


    naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
    getCameraSettingsRequest().saveToConfig();
    config.save();
    std::cout << "Camera parameter were saved after auto-calibration" << endl;
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
  }//end if command
}//end executeDebugCommand
