#include "CameraSettingsV5Manager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

CameraSettingsV5Manager::CameraSettingsV5Manager()
{
}

void CameraSettingsV5Manager::query(int cameraFd, std::string cameraName, naoth::CameraSettings& settings)
{
    settings.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE);
    settings.saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);
    settings.whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

    std::int32_t gainRaw = static_cast<std::int32_t>(Math::clamp(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN), 0, 255));
    settings.gain = Math::fromFixPoint<5>(gainRaw);
}

void CameraSettingsV5Manager::apply(int cameraFd, std::string cameraName, const naoth::CameraSettings& settings)
{
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", Math::clamp(settings.exposure, 0, 1000));
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", Math::clamp(settings.saturation, 0, 255));
    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", Math::clamp(settings.whiteBalanceTemperature, 2700, 6500));

    setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(settings.gain)));    
}
