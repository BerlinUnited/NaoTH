#ifndef _V4LCameraSettingsManager_H_
#define _V4LCameraSettingsManager_H_

#include <Representations/Infrastructure/CameraSettings.h>

class V4LCameraSettingsManager : public naoth::CameraSettingsManager
{
public:
    V4LCameraSettingsManager();

protected:
    int error_count;
    naoth::CameraSettings current;

protected:

    int getSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID);
    bool setSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID, std::string parameterName, int value);

    bool setRawIfChanged(int cameraFd, std::string cameraName, int parameterID, std::string parameterName, int value, int& bufferedValue, bool force=false);

    int32_t getSingleCameraParameterUVC(int cameraFd, std::string cameraName, int parameterSelector, 
        std::string parameterName, uint16_t parameterDataSize);
    bool setSingleCameraParameterUVC(int cameraFd, std::string cameraName, 
        uint8_t parameterSelector, std::string parameterName, uint16_t parameterDataSize, int32_t value);

    int xioctl(int fd, int request, void *arg) const;

    bool hasIOError(std::string cameraName, int errOccured, int errNo, bool exitByIOError = true, std::string paramName = "") const;
    std::string getErrnoDescription(int err) const;
};

#endif //_V4LCameraSettingsManager_H_