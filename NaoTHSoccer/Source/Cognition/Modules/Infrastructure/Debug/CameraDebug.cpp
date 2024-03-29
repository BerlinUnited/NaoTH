/**
* @file CameraDebug.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraDebug
*/

#include "CameraDebug.h"

using namespace std;

CameraDebug::CameraDebug(): CameraDebugBase()
{  
  getDebugParameterList().add(&getCameraInfo().params);
  getDebugParameterList().add(&getCameraInfoTop().params);

  // register the CameraSettingsRequest as a parameter list
  getDebugParameterList().add(&getCameraSettingsRequest());
  getDebugParameterList().add(&getCameraSettingsRequestTop());
  getDebugParameterList().add(&getCommonCameraSettingsRequest());
}

CameraDebug::~CameraDebug()
{
  getDebugParameterList().remove(&getCameraInfo().params);
  getDebugParameterList().remove(&getCameraInfoTop().params);

  getDebugParameterList().remove(&getCameraSettingsRequest());
  getDebugParameterList().remove(&getCameraSettingsRequestTop());
  getDebugParameterList().remove(&getCommonCameraSettingsRequest());
}

void CameraDebug::execute()
{
  if(getCommonCameraSettingsRequest().isActive)
  {
    bool changed = false;
    const CommonCameraSettingsRequest& common = getCommonCameraSettingsRequest();
    CameraSettingsRequest& bottom = getCameraSettingsRequest();
    CameraSettingsRequestTop& top = getCameraSettingsRequestTop();

    // if (getFrameInfo().getFrameNumber() < 100) {
    //   bottom.autoExposition = false;
    //   top.autoExposition = false;
    //   bottom.autoWhiteBalancing = false;
    //   top.autoWhiteBalancing = false;
    // } 
    // else
    //{
      setCommonCameraSetting(common.autoExposition, bottom.autoExposition, top.autoExposition, changed);
      setCommonCameraSetting(common.autoWhiteBalancing, bottom.autoWhiteBalancing, top.autoWhiteBalancing, changed);
    //}
    
    setCommonCameraSetting(common.brightness, bottom.brightness, top.brightness, changed);
    setCommonCameraSetting(common.exposure, bottom.exposure, top.exposure, changed);

    setCommonCameraSetting(common.gain, bottom.gain, top.gain, changed);
    setCommonCameraSetting(common.saturation, bottom.saturation, top.saturation, changed);
    setCommonCameraSetting(common.sharpness, bottom.sharpness, top.sharpness, changed);
    setCommonCameraSetting(common.v5.whiteBalanceTemperature, bottom.v5.whiteBalanceTemperature, top.v5.whiteBalanceTemperature, changed);
    
            
    if(changed)
    {
      getCameraSettingsRequest().saveToConfig();
      getCameraSettingsRequestTop().saveToConfig();
    }
  }
  
  // A sanity check, just in case ... 
  if(getCameraInfo().resolutionWidth != naoth::IMAGE_WIDTH || getCameraInfo().resolutionHeight != naoth::IMAGE_HEIGHT) {
    THROW("Image size of the CameraInfo doesn't correspond to the static values IMAGE_WIDTH and IMAGE_HEIGHT.");
  }
}//end execute

