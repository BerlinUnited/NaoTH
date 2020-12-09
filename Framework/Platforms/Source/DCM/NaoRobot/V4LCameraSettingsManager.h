#ifndef _V4LCameraSettingsManager_H_
#define _V4LCameraSettingsManager_H_

#include <Representations/Infrastructure/CameraSettings.h>

extern "C"
{
#include <linux/videodev2.h>
#include <linux/uvcvideo.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/usb/video.h>
}

#define LOG "[CameraHandler:" << __LINE__ << ", Camera: " << cameraName << "] "
#define hasIOError(...) hasIOErrorPrint(__LINE__, __VA_ARGS__)

/**

NOTE: there are two ways to access V4L controls: v4l2_control and uvc_xu_control_query

* The Linux USB Video Class (UVC) driver:
https://linuxtv.org/downloads/v4l-dvb-apis/v4l-drivers/uvcvideo.html#

*/

class V4LCameraSettingsManager
{
public:
  V4LCameraSettingsManager(){}
  // this class is meant to be derived from and contains virtual methods, 
  // so make also the destructor virtual just in case to prevent future memory leaks
  virtual ~V4LCameraSettingsManager(){}

private: 
  /** used by enumerate_controls */
  static void enumerate_menu(int fd, v4l2_queryctrl& queryctrl);
public: 
  /** print all controlls and their parameters */
  static void enumerate_controls(int fd);

protected:

  int getSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, uint32_t parameterID);
  bool setSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, uint32_t parameterID, const std::string& parameterName, int value);

  bool setRawIfChanged(int cameraFd, const std::string& cameraName, uint32_t parameterID, const std::string& parameterName, int value, int& bufferedValue, bool force=false);

  template<typename T>
  T getParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName) {
    T data;
    int error = querySingleCameraParameterUVC(cameraFd, UVC_GET_CUR, parameterSelector, &data, sizeof(T));
    
    //TODO: this is a mixture between c-style error handling and a cpp style function
    hasIOError(cameraName, error, errno, false, "get " + parameterName);
    return data;
  }
  
  template<typename T>
  bool getParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName, T& data) {
    int error = querySingleCameraParameterUVC(cameraFd, UVC_GET_CUR, parameterSelector, &data, sizeof(T));
    return !hasIOError(cameraName, error, errno, false, "get " + parameterName);
  }
  
  template<typename T>
  bool setParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName, T value) {
    int error = querySingleCameraParameterUVC(cameraFd, UVC_SET_CUR, parameterSelector, &value, sizeof(T));
    return !hasIOError(cameraName, error, errno, false, "set " + parameterName);
  }
  
  bool setParameterUVC(int cameraFd, const std::string& cameraName, uint8_t parameterSelector, const std::string& parameterName, void* data, uint16_t size) {
    int error = querySingleCameraParameterUVC(cameraFd, UVC_SET_CUR, parameterSelector, data, size);
    return !hasIOError(cameraName, error, errno, false, "set " + parameterName);
  }
 
public:
  /** Queries all values from the actual camera */
  virtual void query(int cameraFd, const std::string& cameraName, naoth::CameraSettings &settings) = 0;

  /** Apply all changed values on the actual camera */
  virtual void apply(int cameraFd, const std::string& cameraName, const naoth::CameraSettings &settings, bool force=false) = 0;

private:
  static int querySingleCameraParameterUVC(int cameraFd, uint8_t query, uint8_t selector, void* data, uint16_t size);

  static int xioctl(int fd, int request, void *arg);
  
  static bool hasIOErrorPrint(int lineNumber, const std::string& cameraName, int errOccured, int errNo, bool exitByIOError, const std::string& paramName = "");
};

#endif //_V4LCameraSettingsManager_H_