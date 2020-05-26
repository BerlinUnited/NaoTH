#ifndef _CameraSettingsV5Manager_H_
#define _CameraSettingsV5Manager_H_

#include "V4LCameraSettingsManager.h"
#include <Representations/Infrastructure/CameraSettings.h>

class CameraSettingsV5Manager : public V4LCameraSettingsManager
{
public:
  CameraSettingsV5Manager();

  virtual void query(int cameraFd, const std::string& cameraName, naoth::CameraSettings &settings);
  virtual void apply(int cameraFd, const std::string& cameraName, const naoth::CameraSettings &settings, bool force = false);

private:
  naoth::CameraSettings current;
  bool initialized;

private:
  int getAutoExposureGridID(size_t i, size_t j);
};

#endif // _CameraSettingsV5Manager_H_