#ifndef _CameraSettingsV5Manager_H_
#define _CameraSettingsV5Manager_H_

#include "V4LCameraSettingsManager.h"
#include <Representations/Infrastructure/CameraSettings.h>

class CameraSettingsV5Manager : public V4LCameraSettingsManager
{
public:
  CameraSettingsV5Manager(int cameraFd, const std::string& cameraName);

  virtual void query(naoth::CameraSettings &settings);
  virtual void apply(const naoth::CameraSettings &settings, bool force = false);

private:
  naoth::CameraSettings current;
  bool initialized;

  int cameraFd;
  std::string cameraName;

private:
  int getAutoExposureGridID(size_t i, size_t j);
};

#endif // _CameraSettingsV5Manager_H_