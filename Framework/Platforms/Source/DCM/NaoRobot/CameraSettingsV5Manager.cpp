#include "CameraSettingsV5Manager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

#define LOG "[CameraHandler:" << __LINE__ << ", Camera: " << cameraName << "] "

#define hasIOError(...) hasIOErrorPrint(__LINE__, __VA_ARGS__)

CameraSettingsV5Manager::CameraSettingsV5Manager()
    : error_count(0)
{
}

void CameraSettingsV5Manager::query(int cameraFd, std::string cameraName)
{
    exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE);
    saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);
    whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

    std::int32_t gainRaw = static_cast<std::int32_t>(Math::clamp(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN), 0, 255));
    gain = Math::fromFixPoint<5>(gainRaw);
}

void CameraSettingsV5Manager::apply(int cameraFd, std::string cameraName)
{
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", exposure);
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", saturation);
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", whiteBalanceTemperature);

    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(gain)));    
}

int CameraSettingsV5Manager::getSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID)
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

bool CameraSettingsV5Manager::setSingleCameraParameterRaw(int cameraFd, std::string cameraName, int parameterID, std::string parameterName, int value)
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
        std::cerr << LOG << "VIDIOC_QUERYCTRL failed with code " << errCode << " " << strerror(errCode) << std::endl;
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
int CameraSettingsV5Manager::xioctl(int fd, int request, void *arg) const
{
    int r;
    // TODO: possibly endless loop?
    do
    {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno); // repeat if the call was interrupted
    return r;
}

bool CameraSettingsV5Manager::hasIOErrorPrint(int lineNumber, std::string cameraName, int errOccured, int errNo, bool exitByIOError)
{
    if (errOccured < 0 && errNo != EAGAIN)
    {
        std::cout << LOG << "[hasIOError:" << lineNumber << "]"
                  << " failed with errno " << errNo << " (" << strerror(errNo) << ") >> exiting" << std::endl;
        if (exitByIOError && error_count > 10)
        {
            assert(errOccured >= 0);
        }
        error_count++;
        return true;
    }
    return false;
}