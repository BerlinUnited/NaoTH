#ifndef _CameraSettingsV5Manager_H_
#define _CameraSettingsV5Manager_H_


#include "V4LCameraSettingsManager.h"
#include <Representations/Infrastructure/CameraSettings.h>


class CameraSettingsV5Manager : public V4LCameraSettingsManager {
  public:

  CameraSettingsV5Manager();

  virtual void query(int cameraFd, std::string cameraName, naoth::CameraSettings& settings);

  virtual void apply(int cameraFd, std::string cameraName, const naoth::CameraSettings& settings);

};

#endif // _CameraSettingsV5Manager_H_