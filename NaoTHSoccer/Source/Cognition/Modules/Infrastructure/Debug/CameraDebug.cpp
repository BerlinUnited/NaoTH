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

CameraDebug::CameraDebug(): CameraDebugBase()
{  
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
  if(getCommonCameraSettingsRequest().isActive)
  {
    bool settingsChanged = false;
    const CommonCameraSettingsRequest& commonRequest = getCommonCameraSettingsRequest();
    CameraSettingsRequest& bottomRequest = getCameraSettingsRequest();
    CameraSettingsRequestTop& topRequest = getCameraSettingsRequestTop();

    // bottom
    if(commonRequest.autoExposition != bottomRequest.autoExposition) {
      settingsChanged = true;
      bottomRequest.autoExposition = commonRequest.autoExposition;
    }
    if(commonRequest.autoWhiteBalancing != bottomRequest.autoWhiteBalancing) {
      settingsChanged = true;
      bottomRequest.autoWhiteBalancing = commonRequest.autoWhiteBalancing;
    }
    if(commonRequest.exposure != bottomRequest.exposure) {
      settingsChanged = true;
      bottomRequest.exposure = commonRequest.exposure;
    }
    if(commonRequest.gain != bottomRequest.gain) {
      settingsChanged = true;
      bottomRequest.gain = commonRequest.gain;
    }
    if(commonRequest.saturation != bottomRequest.saturation) {
      settingsChanged = true;
      bottomRequest.saturation = commonRequest.saturation;
    }
    if(commonRequest.sharpness != bottomRequest.sharpness) {
      settingsChanged = true;
      bottomRequest.sharpness = commonRequest.sharpness;
    }
    if(commonRequest.whiteBalanceTemperature != bottomRequest.whiteBalanceTemperature) {
      settingsChanged = true;
      bottomRequest.whiteBalanceTemperature = commonRequest.whiteBalanceTemperature;
    }

    // top
    if(commonRequest.autoExposition != topRequest.autoExposition) {
      settingsChanged = true;
      topRequest.autoExposition = commonRequest.autoExposition;
    }
    if(commonRequest.autoWhiteBalancing != topRequest.autoWhiteBalancing) {
      settingsChanged = true;
      topRequest.autoWhiteBalancing = commonRequest.autoWhiteBalancing;
    }
    if(commonRequest.exposure != topRequest.exposure) {
      settingsChanged = true;
      topRequest.exposure = commonRequest.exposure;
    }
    if(commonRequest.gain != topRequest.gain) {
      settingsChanged = true;
      topRequest.gain = commonRequest.gain;
    }
    if(commonRequest.saturation != topRequest.saturation) {
      settingsChanged = true;
      topRequest.saturation = commonRequest.saturation;
    }
    if(commonRequest.sharpness != topRequest.sharpness) {
      settingsChanged = true;
      topRequest.sharpness = commonRequest.sharpness;
    }
    if(commonRequest.whiteBalanceTemperature != topRequest.whiteBalanceTemperature) {
      settingsChanged = true;
      topRequest.whiteBalanceTemperature = commonRequest.whiteBalanceTemperature;
    }
    


    if(settingsChanged)
    {
      getCameraSettingsRequest().saveToConfig();
      getCameraSettingsRequestTop().saveToConfig();
    }
  }
  
}//end execute

