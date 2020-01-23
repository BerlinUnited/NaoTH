#include "V4LCameraSettingsManager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <linux/uvcvideo.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/usb/video.h>
}

#define LOG "[CameraHandler:" << __LINE__ << ", Camera: " << cameraName << "] "

int V4LCameraSettingsManager::getSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, int parameterID)
{
  struct v4l2_queryctrl queryctrl;
  queryctrl.id = parameterID;
  
  // NOTE: why not xioctl?
  int errCode = ioctl(cameraFd, VIDIOC_QUERYCTRL, &queryctrl);

  // check if query was successful
  if (errCode < 0) {
    std::cerr << LOG << "VIDIOC_QUERYCTRL failed: " << strerror(errCode) << std::endl;
    return -1; 
  }
  
  // check if parameter is avaliable (enabled)
  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
    std::cerr << LOG << "not getting camera parameter since it is not available" << std::endl;
    return -1; 
  }

  // check if parameter is supported
  // NOTE: we only consider boolean, integer and menu types as supported
  // TODO: is this correct?
  if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU) {
    std::cerr << LOG << "not getting camera parameter since it is not supported" << std::endl;
    return -1;
  }

  /*
  struct v4l2_control {
     __u32  id;      // Identifies the control, set by the application.
    __s32  value;    // New value or current value.
  } 
  */
  struct v4l2_control control_g;
  control_g.id = parameterID;

  // max 20 trials
  int errorOccured = xioctl(cameraFd, VIDIOC_G_CTRL, &control_g);
  if (hasIOError(cameraName, errorOccured, errno, false)) {
    // TODO: some parameter may have -1 as a valid value
    return -1;
  } else {
    return control_g.value;
  }
}

bool V4LCameraSettingsManager::setSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, int parameterID, const std::string& parameterName, int value)
{
  if (parameterID < 0)
  {
    return false;
  }
  struct v4l2_queryctrl queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl));
  queryctrl.id = parameterID;
  
  if (int errCode = xioctl(cameraFd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
  {
    std::cerr << LOG << "VIDIOC_QUERYCTRL for parameter " << parameterName << " failed with code " << errCode << " " << strerror(errCode) << std::endl;
    return false;
  }
  
  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
  {
    std::cerr << LOG << "V4L2_CTRL_FLAG_DISABLED failed" << std::endl;
    return false; // not available
  }

  if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
  {
    std::cerr << LOG << "V4L2_CTRL_FLAG_DISABLED failed" << std::endl;
    return false; // not supported
  }

  // clip value
  if (value < queryctrl.minimum)
  {
    std::cout << LOG << "Clipping control value  " << parameterName << " from " << value << " to " << queryctrl.minimum << std::endl;
    value = queryctrl.minimum;
  }
  if (value > queryctrl.maximum)
  {
    std::cout << LOG << "Clipping control value " << parameterName << " from " << value << " to " << queryctrl.maximum << std::endl;
    value = queryctrl.maximum;
  }

  struct v4l2_control control_s;
  control_s.id = parameterID;
  control_s.value = value;

  std::cout << LOG << "Setting control value " << parameterName << " to " << value << std::endl;
  int error = xioctl(cameraFd, VIDIOC_S_CTRL, &control_s);
  return !hasIOError(cameraName, error, errno, false);
}

bool V4LCameraSettingsManager::setRawIfChanged(int cameraFd, const std::string& cameraName, int parameterID,
                                               const std::string& parameterName, int value, int &bufferedValue, bool force)
{
  if ((force || bufferedValue != value) &&
      setSingleCameraParameterRaw(cameraFd, cameraName, parameterID, parameterName, value))
  {
    bufferedValue = value;
    return true;
  }
  return false;
}

// https://01.org/linuxgraphics/gfx-docs/drm/media/uapi/v4l/capture.c.html
int V4LCameraSettingsManager::xioctl(int fd, int request, void *arg) const
{
  int r;
  // TODO: possibly endless loop?
  do
  {
    r = ioctl(fd, request, arg);
  } while (-1 == r && EINTR == errno); // repeat if the call was interrupted
  return r;
}

/*
int32_t V4LCameraSettingsManager::getSingleCameraParameterUVC(int cameraFd, const std::string& cameraName,
                                                              uint8_t parameterSelector, const std::string& parameterName, uint16_t parameterDataSize)
{
  struct uvc_xu_control_query queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl));

  // HACK: currently only maximum of 4-bytes is supported 
  assert(parameterDataSize <= sizeof(int32_t));
  int32_t value; 
  
  queryctrl.unit = 3;
  queryctrl.query = UVC_GET_CUR;
  queryctrl.selector = parameterSelector;

  queryctrl.size = parameterDataSize;
  queryctrl.data = reinterpret_cast<uint8_t*>(&value);
  
  int error = xioctl(cameraFd, UVCIOC_CTRL_QUERY, &queryctrl);

  //std::cout << cameraName << ": " << parameterName << " = " << ((uint16_t)value) << std::endl;
  if (hasIOError(cameraName, error, errno, false, "get " + parameterName)) {
    return -1;
  } else {
    return value;
  }
}
*/
/*
bool V4LCameraSettingsManager::setSingleCameraParameterUVC(int cameraFd, const std::string& cameraName,
                                                           uint8_t parameterSelector, const std::string& parameterName, uint16_t data_size, int32_t value)
{
  struct uvc_xu_control_query queryctrl;
  
  queryctrl.unit = 3;
  queryctrl.query = UVC_SET_CUR;
  queryctrl.selector = parameterSelector;

  queryctrl.size = data_size;
  queryctrl.data = reinterpret_cast<uint8_t*>(&value);

  //std::cout << LOG << "Setting control value " << parameterName << " to " << value << std::endl;
  int error = xioctl(cameraFd, UVCIOC_CTRL_QUERY, &queryctrl);
  return !hasIOError(cameraName, error, errno, false, "set " + parameterName);
}
*/
/*
// https://linuxtv.org/downloads/v4l-dvb-apis/v4l-drivers/uvcvideo.html
// https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/uvcvideo.h#L61
struct uvc_xu_control_query {
	__u8 unit;          // Extension unit ID
	__u8 selector;      // Control selector
	__u8 query;		      // Request code to send to the device (Video Class-Specific Request Code, defined in linux/usb/video.h A.8.)  
	__u16 size;         // Control data size (in bytes)
	__u8 __user *data;  // Control value
};
*/
int V4LCameraSettingsManager::querySingleCameraParameterUVC(int cameraFd, uint8_t query, uint8_t selector, void* data, uint16_t size)
{
  uvc_xu_control_query queryctrl {
    .unit = 3,
    .selector = selector,
    .query = query,
  
    .size = size,
    .data = static_cast<uint8_t*>(data)
  };

  return xioctl(cameraFd, UVCIOC_CTRL_QUERY, &queryctrl);
}


bool V4LCameraSettingsManager::hasIOError(const std::string& cameraName, int errOccured, int errNo, bool exitByIOError, const std::string& paramName) const
{
  if (errOccured < 0 && errNo != EAGAIN)
  {
    std::cout << LOG << paramName << " failed with errno " << errNo << " (" << strerror(errNo) << ") >> exiting" << std::endl;
    if (exitByIOError) {
      assert(errOccured >= 0);
    }
    return true;
  }
  return false;
}