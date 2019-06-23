#include "CameraSettingsV6Manager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
}


CameraSettingsV6Manager::CameraSettingsV6Manager()
{
}

void CameraSettingsV6Manager::query(int cameraFd, std::string cameraName)
{
    exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE);
    saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);
    whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

    std::int32_t gainRaw = static_cast<std::int32_t>(Math::clamp(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN), 0, 255));
    gain = Math::fromFixPoint<5>(gainRaw);
}

void CameraSettingsV6Manager::apply(int cameraFd, std::string cameraName)
{
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", Math::clamp(exposure, 0, 1000));
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", Math::clamp(saturation, 0, 255));
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", Math::clamp(whiteBalanceTemperature, 2700, 6500));

    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(gain)));    
}


