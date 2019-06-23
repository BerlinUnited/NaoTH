#ifndef _CameraSettingsV6Manager_H_
#define _CameraSettingsV6Manager_H_


#include "V4LCameraSettingsManager.h"
#include <Representations/Infrastructure/CameraSettings.h>


class CameraSettingsV6Manager : public V4LCameraSettingsManager {
  public:

  CameraSettingsV6Manager();

  virtual void query(int cameraFd, std::string cameraName, naoth::CameraSettings& settings);

  virtual void apply(int cameraFd, std::string cameraName, const naoth::CameraSettings& settings);

};

#endif // _CameraSettingsV6Manager_H_