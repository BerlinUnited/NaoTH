#include "V4LCameraSettingsManager.h"

/**
Based on example from 
https://www.kernel.org/doc/html/v4.14/media/uapi/v4l/control.html
*/
void V4LCameraSettingsManager::enumerate_menu(int fd, v4l2_queryctrl& queryctrl)
{
  printf("  Menu items:\n");
  
  struct v4l2_querymenu querymenu;
  memset(&querymenu, 0, sizeof(querymenu));
  querymenu.id = queryctrl.id;

  for (querymenu.index = queryctrl.minimum; (int)querymenu.index <= queryctrl.maximum; querymenu.index++) {
    if (0 == xioctl(fd, VIDIOC_QUERYMENU, &querymenu)) {
      printf("   +- [%i] %s\n", querymenu.index, querymenu.name);
    } else {
      printf("   +- [%i] (%i) %s\n", querymenu.index, errno, strerror(errno));
    }
  }
}

void V4LCameraSettingsManager::enumerate_controls(int fd)
{
  struct v4l2_queryctrl queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl));

  printf("=== Camera Controls BEGIN ===\n");
  
  queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
  while (0 == xioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) 
  {
    if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) 
    {    
      printf(" %i: %i, %s (min: %i, max: %i, step: %i) \n", queryctrl.id, queryctrl.type, queryctrl.name, queryctrl.minimum, queryctrl.maximum, queryctrl.step);

      if (queryctrl.type == V4L2_CTRL_TYPE_MENU) {
          enumerate_menu(fd, queryctrl);
      }
    }
    queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
  }
  
  if (errno != EINVAL) {
    perror("VIDIOC_QUERYCTRL");
    exit(EXIT_FAILURE);
  }
  printf("=== Camera Controls END ===\n");
}

int V4LCameraSettingsManager::getSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, uint32_t parameterID)
{
  // TODO: make it more general and maybe do it only once at the beginning
  // NOTE: first query information regarding the parameter to verify it is avaliable and enabled
  struct v4l2_queryctrl queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl)); // is it necessary?
  queryctrl.id = parameterID;
  
  // check if query was successful
  if (xioctl(cameraFd, VIDIOC_QUERYCTRL, &queryctrl) < 0) {
    std::cerr << LOG << "VIDIOC_QUERYCTRL for parameter " << parameterID << " failed with code " << errno << " " << strerror(errno) << std::endl;
    return -1; 
  }
  
  // check if parameter is avaliable (enabled)
  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
    std::cerr << LOG << "V4L2_CTRL_FLAG_DISABLED failed. Parameter " << parameterID << " seems to be not avaliable." << std::endl;
    return -1; 
  }

  // check if parameter is supported
  // NOTE: we only consider boolean, integer and menu types as supported
  if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU) {
    std::cerr << LOG << "ERROR: type " << queryctrl.type << " of the parameter " << parameterID << " is not supported." << std::endl;
    return -1;
  }

  /*
  struct v4l2_control {
    __u32  id;      // Identifies the control, set by the application.
    __s32  value;   // New value or current value.
  } 
  */
  struct v4l2_control control;
  control.id = parameterID;

  int errorOccured = xioctl(cameraFd, VIDIOC_G_CTRL, &control);
  if (hasIOError(cameraName, errorOccured, errno, false)) {
    // TODO: some parameter may have -1 as a valid value
    return -1;
  } else {
    return control.value;
  }
}

bool V4LCameraSettingsManager::setSingleCameraParameterRaw(int cameraFd, const std::string& cameraName, uint32_t parameterID, const std::string& parameterName, int value)
{
  // TODO: make it more general and maybe do it only once at the beginning
  // NOTE: first query information regarding the parameter to verify it is avaliable and enabled
  struct v4l2_queryctrl queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl)); // is it necessary?
  queryctrl.id = parameterID;
  
  // check if query was successful
  if (xioctl(cameraFd, VIDIOC_QUERYCTRL, &queryctrl) < 0) {
    std::cerr << LOG << "VIDIOC_QUERYCTRL for parameter " << parameterName << " failed with code " << errno << " " << strerror(errno) << std::endl;
    return false; 
  }
  
  // check if parameter is avaliable (enabled)
  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
    std::cerr << LOG << "V4L2_CTRL_FLAG_DISABLED failed. Parameter " << parameterName << " seems to be not avaliable." << std::endl;
    return false; 
  }

  // check if parameter is supported
  // NOTE: we only consider boolean, integer and menu types as supported
  if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU) {
    std::cerr << LOG << "ERROR: type " << queryctrl.type << " of the parameter " << parameterName << " is not supported." << std::endl;
    return false;
  }

  // clip value
  if (value < queryctrl.minimum) {
    std::cout << LOG << "Clipping control value " << parameterName << " from " << value << " to " << queryctrl.minimum << std::endl;
    value = queryctrl.minimum;
  } else if (value > queryctrl.maximum) {
    std::cout << LOG << "Clipping control value " << parameterName << " from " << value << " to " << queryctrl.maximum << std::endl;
    value = queryctrl.maximum;
  }

  /*
  struct v4l2_control {
    __u32  id;      // Identifies the control, set by the application.
    __s32  value;   // New value or current value.
  } 
  */
  struct v4l2_control control {
    .id = parameterID,
    .value = value
  };

  std::cout << LOG << "Setting control value " << parameterName << " to " << value << std::endl;
  int error = xioctl(cameraFd, VIDIOC_S_CTRL, &control);
  return !hasIOError(cameraName, error, errno, false);
}

bool V4LCameraSettingsManager::setRawIfChanged(int cameraFd, const std::string& cameraName, uint32_t parameterID,
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
int V4LCameraSettingsManager::xioctl(int fd, int request, void *arg)
{
  int r;
  // TODO: possibly endless loop?
  do {
    r = ioctl(fd, request, arg);
  } while (-1 == r && EINTR == errno); // repeat if the call was interrupted
  return r;
}


/**
// https://linuxtv.org/downloads/v4l-dvb-apis/v4l-drivers/uvcvideo.html
// https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/uvcvideo.h#L61
struct uvc_xu_control_query {
  __u8 unit;          // Extension unit ID
  __u8 selector;      // Control selector
  __u8 query;          // Request code to send to the device (Video Class-Specific Request Code, defined in linux/usb/video.h A.8.)  
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

bool V4LCameraSettingsManager::hasIOErrorPrint(int lineNumber, const std::string& cameraName, int errOccured, int errNo, bool exitByIOError, const std::string& paramName)
{
  if (errOccured < 0 && errNo != EAGAIN)
  {
    std::cout << LOG << paramName << " [hasIOError:" << lineNumber << "]"
              << " failed with errno " << errNo << " (" << strerror(errNo) << ")" 
              << (exitByIOError?" >> exiting":"") << std::endl;
    if (exitByIOError) {
      assert(errOccured >= 0);
    }
    return true;
  }
  return false;
}
