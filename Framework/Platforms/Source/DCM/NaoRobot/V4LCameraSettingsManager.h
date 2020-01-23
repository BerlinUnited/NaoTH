#ifndef _V4LCameraSettingsManager_H_
#define _V4LCameraSettingsManager_H_

#include <Representations/Infrastructure/CameraSettings.h>
#include <linux/usb/video.h>

class V4LCameraSettingsManager
{
public:
    V4LCameraSettingsManager(){}

protected:

    int getSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, int parameterID);
    bool setSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, int parameterID, const std::string& parameterName, int value);

    bool setRawIfChanged(int cameraFd, const std::string& cameraName, int parameterID, const std::string& parameterName, int value, int& bufferedValue, bool force=false);

    // NOTE: deprecated and will be removed soon
    //int32_t getSingleCameraParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName, uint16_t parameterDataSize);
    //bool setSingleCameraParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName, uint16_t parameterDataSize, int32_t value);
    
    template<typename T>
    T getParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName) {
      T data;
      int error = querySingleCameraParameterUVC(cameraFd, UVC_GET_CUR, parameterSelector, &data, sizeof(T));
      
      //TODO: this is a mixture between c-style error handling and a cpp style function
      hasIOError(cameraName, error, errno, false, "get " + parameterName);
      return data;
    }
    
    template<typename T>
    bool setParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName, T value) {
      int error = querySingleCameraParameterUVC(cameraFd, UVC_SET_CUR, parameterSelector, &value, sizeof(T));
      return !hasIOError(cameraName, error, errno, false, "set " + parameterName);
    }
 
public:
  /** Queries all values from the actual camera */
  virtual void query(int cameraFd, const std::string& cameraName, naoth::CameraSettings &settings) = 0;

  /** Apply all changed values on the actual camera */
  virtual void apply(int cameraFd, const std::string& cameraName, const naoth::CameraSettings &settings, bool force=false) = 0;

private:
    int querySingleCameraParameterUVC(int cameraFd, uint8_t query, uint8_t selector, void* data, uint16_t size);

    int xioctl(int fd, int request, void *arg) const;
    bool hasIOError(const std::string& cameraName, int errOccured, int errNo, bool exitByIOError = true, const std::string& paramName = "") const;
};

#endif //_V4LCameraSettingsManager_H_