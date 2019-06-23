#ifndef _CameraSettingsV5Manager_H_
#define _CameraSettingsV5Manager_H_

#include <Representations/Infrastructure/CameraSettings.h>


class CameraSettingsV5Manager : public naoth::CameraSettingsManager {
  public:

  CameraSettingsV5Manager();

  virtual void query(int cameraFd, std::string cameraName);

  virtual void apply(int cameraFd, std::string cameraName);

  private:
    int error_count;

  private:
    int getSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID);
    bool setSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID, std::string parameterName, int value);

    int xioctl(int fd, int request, void* arg) const;

    bool hasIOErrorPrint(int lineNumber, std::string cameraName, int errOccured, int errNo, bool exitByIOError = true);

};

#endif // _CameraSettingsV5Manager_H_