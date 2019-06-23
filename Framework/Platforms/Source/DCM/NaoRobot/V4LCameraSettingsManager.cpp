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

V4LCameraSettingsManager::V4LCameraSettingsManager()
    : error_count(0)
{
}

int V4LCameraSettingsManager::getSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID)
{
    struct v4l2_queryctrl queryctrl;
    queryctrl.id = parameterID;
    if (int errCode = ioctl(cameraFd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
    {
        std::cerr << LOG << "VIDIOC_QUERYCTRL failed: " << strerror(errCode) << std::endl;
        return -1;
    }
    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
        std::cerr << LOG << "not getting camera parameter since it is not available" << std::endl;
        return -1; // not available
    }
    if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
    {
        std::cerr << LOG << "not getting camera parameter since it is not supported" << std::endl;
        return -1; // not supported
    }

    struct v4l2_control control_g;
    control_g.id = parameterID;

    // max 20 trials
    for (int i = 0; i < 20; i++)
    {
        int errorOccured = ioctl(cameraFd, VIDIOC_G_CTRL, &control_g);

        if (errorOccured < 0)
        {
            switch (errno)
            {
            case EAGAIN:
                usleep(10);
                break;
            case EBUSY:
                usleep(100000);
                break;
            default:
                hasIOError(cameraName, errorOccured, errno, false);
            }
        }
    }

    return -1;
}


bool V4LCameraSettingsManager::setSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID, std::string parameterName, int value)
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
        std::cerr << LOG << "VIDIOC_QUERYCTRL for parameter " << parameterName <<  " failed with code " << errCode << " " << strerror(errCode) << std::endl;
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

    int error = xioctl(cameraFd, VIDIOC_S_CTRL, &control_s);
    return !hasIOError(cameraName, error, errno, false);
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


int32_t V4LCameraSettingsManager::getSingleCameraParameterUVC(int cameraFd, std::string cameraName, 
    int parameterSelector, std::string parameterName, uint16_t parameterDataSize)
{

  if (parameterSelector < 0)
  {
    return -1;
  }


  struct uvc_xu_control_query queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl));

  uint8_t *value_raw = new uint8_t[parameterDataSize];
  queryctrl.unit = 3;
  queryctrl.query = UVC_GET_CUR;
  queryctrl.selector = static_cast<uint8_t>(parameterSelector);

  queryctrl.size = parameterDataSize;
  queryctrl.data = value_raw;

  int error = xioctl(cameraFd, UVCIOC_CTRL_QUERY, &queryctrl);

  int32_t value = (value_raw[3] << 24) | (value_raw[2] << 16) | (value_raw[1] << 8) | (value_raw[0]);
  delete[] value_raw;
  if (hasIOError(cameraName, error, errno, false, "get " + parameterName))
  {
    return -1;
  }
  else
  {
    return value;
  }
}

bool V4LCameraSettingsManager::setSingleCameraParameterUVC(int cameraFd, std::string cameraName, 
    int parameterSelector, std::string parameterName, uint16_t parameterDataSize, int32_t value)
{

  if (parameterSelector)
  {
    return false;
  }


  struct uvc_xu_control_query queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl));

  uint8_t *value_raw = new uint8_t[parameterDataSize];
  memset(value_raw, 0, parameterDataSize);
  value_raw[3] = static_cast<uint8_t>(value >> 24);
  value_raw[2] = static_cast<uint8_t>(value >> 16);
  value_raw[1] = static_cast<uint8_t>(value >> 8);
  value_raw[0] = static_cast<uint8_t>(value);

  queryctrl.unit = 3;
  queryctrl.query = UVC_SET_CUR;
  queryctrl.selector = static_cast<uint8_t>(parameterSelector);
  queryctrl.size = parameterDataSize;
  queryctrl.data = value_raw;

  int error = xioctl(cameraFd, UVCIOC_CTRL_QUERY, &queryctrl);
  return !hasIOError(cameraName, error, errno, false, "set " + parameterName);
}

bool V4LCameraSettingsManager::hasIOError(std::string cameraName, int errOccured, int errNo, bool exitByIOError, std::string paramName) const
{
  if (errOccured < 0 && errNo != EAGAIN)
  {
    std::cout << LOG << paramName << " failed with errno " << errNo << " (" << getErrnoDescription(errNo) << ") >> exiting" << std::endl;
    if (exitByIOError)
    {
      assert(errOccured >= 0);
    }
    return true;
  }
  return false;
}

std::string V4LCameraSettingsManager::getErrnoDescription(int err) const
{
  switch (err)
  {
  case EPERM:
    return "Operation not permitted";
  case ENOENT:
    return "No such file or directory";
  case ENOBUFS:
    return "The specified buffer size is incorrect (too big or too small).";
  case ESRCH:
    return "No such process";
  case EINTR:
    return "Interrupted system call";
  case EIO:
    return "I/O error";
  case ENXIO:
    return "No such device or address";
  case E2BIG:
    return "Argument list too long";
  case ENOEXEC:
    return "Exec format error";
  case EBADF:
    return "Bad file number";
  case ECHILD:
    return "No child processes";
  case EAGAIN:
    return "Try again";
  case ENOMEM:
    return "Out of memory";
  case EACCES:
    return "Permission denied";
  case EFAULT:
    return "Bad address";
  case ENOTBLK:
    return "Block device required";
  case EBUSY:
    return "Device or resource busy";
  case EEXIST:
    return "File exists";
  case EXDEV:
    return "Cross-device link";
  case ENODEV:
    return "No such device";
  case ENOTDIR:
    return "Not a directory";
  case EISDIR:
    return "Is a directory";
  case EINVAL:
    return "Invalid argument";
  case ENFILE:
    return "File table overflow";
  case EMFILE:
    return "Too many open files";
  case ENOTTY:
    return "Not a typewriter";
  case ETXTBSY:
    return "Text file busy";
  case EFBIG:
    return "File too large";
  case ENOSPC:
    return "No space left on device";
  case ESPIPE:
    return "Illegal seek";
  case EROFS:
    return "Read-only file system";
  case EMLINK:
    return "Too many links";
  case EPIPE:
    return "Broken pipe";
  case EDOM:
    return "Math argument out of domain of func";
  case ERANGE:
    return "Math result not representable";
  case EBADRQC:
    return "The given request is not supported by the given control.";
  }
  return "Unknown errorcode";
}