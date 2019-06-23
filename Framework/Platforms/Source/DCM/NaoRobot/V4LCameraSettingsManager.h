#ifndef _V4LCameraSettingsManager_H_
#define _V4LCameraSettingsManager_H_

#include <Representations/Infrastructure/CameraSettings.h>


class V4LCameraSettingsManager : public naoth::CameraSettingsManager {
  public:

  V4LCameraSettingsManager();


  protected:
    int error_count;

  protected:
    int getSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID);
    bool setSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID, std::string parameterName, int value);

    int xioctl(int fd, int request, void* arg) const;

    bool hasIOErrorPrint(int lineNumber, std::string cameraName, int errOccured, int errNo, bool exitByIOError = true);

};

#endif //_V4LCameraSettingsManager_H_