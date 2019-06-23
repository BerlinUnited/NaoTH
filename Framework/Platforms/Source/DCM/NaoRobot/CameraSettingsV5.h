#ifndef _CameraSettingsV5_H_
#define _CameraSettingsV5_H_

#include <Representations/Infrastructure/CameraSettings.h>


class CameraSettingsV5 : public naoth::GenericCameraSettings {
  public:

  CameraSettingsV5();

  virtual void query(int cameraFd);

  virtual void apply(int cameraFd);

  private:
    int error_count;

  private:
    int getSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID);
    bool setSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID, std::string parameterName, int value);

    int xioctl(int fd, int request, void* arg) const;

    bool hasIOErrorPrint(int lineNumber, std::string cameraName, int errOccured, int errNo, bool exitByIOError = true);

};

#endif // _CameraSettingsV5_H_