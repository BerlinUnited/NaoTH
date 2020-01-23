#ifndef _CameraSettingsV6Manager_H_
#define _CameraSettingsV6Manager_H_

#include "V4LCameraSettingsManager.h"
#include <Representations/Infrastructure/CameraSettings.h>

class CameraSettingsV6Manager : public V4LCameraSettingsManager 
{  
public:

  CameraSettingsV6Manager();

  virtual void query(int cameraFd, const std::string& cameraName, naoth::CameraSettings& settings);
  virtual void apply(int cameraFd, const std::string& cameraName, const naoth::CameraSettings& settings, bool force=false);

private:
  naoth::CameraSettings current;
  bool initialized;

  uint16_t getRegister(int cameraFd, uint16_t addr);
  bool setRegister(int cameraFd, uint16_t addr, uint16_t val);
};

#endif // _CameraSettingsV6Manager_H_