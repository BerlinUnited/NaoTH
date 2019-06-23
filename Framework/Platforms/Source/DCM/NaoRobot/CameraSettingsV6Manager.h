#ifndef _CameraSettingsV6Manager_H_
#define _CameraSettingsV6Manager_H_


#include "V4LCameraSettingsManager.h"
#include <Representations/Infrastructure/CameraSettings.h>


class CameraSettingsV6Manager : public V4LCameraSettingsManager {
  public:

  CameraSettingsV6Manager();

  virtual void query(int cameraFd, std::string cameraName);

  virtual void apply(int cameraFd, std::string cameraName);

};

#endif // _CameraSettingsV6Manager_H_