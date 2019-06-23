#include "CameraSettingsV6Manager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

using namespace naoth;

CameraSettingsV6Manager::CameraSettingsV6Manager()
{
}

void CameraSettingsV6Manager::query(int cameraFd, std::string cameraName, CameraSettings &settings)
{
    settings.autoExposition = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO) == 0 ? true : false;

    settings.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE);
    settings.saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);
    settings.whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

    std::int32_t gainRaw = static_cast<std::int32_t>(Math::clamp(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN), 0, 255));
    settings.gain = Math::fromFixPoint<5>(gainRaw);

    settings.horizontalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2);
    settings.verticalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2);
}

void CameraSettingsV6Manager::apply(int cameraFd, std::string cameraName, const CameraSettings &settings)
{
    if (autoExposition != settings.autoExposition && setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "autoExposition", settings.autoExposition ? 0 : 1))
    {
        if (settings.autoExposition == false)
        {
            // read back exposure values (and all others) set by the now deactivated auto exposure
            query(cameraFd, cameraName, *this);
        }
        autoExposition = settings.autoExposition;
    }

    if (exposure != settings.exposure &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", Math::clamp(settings.exposure, 0, 1000)))
    {
        exposure = settings.exposure;
    }
    if (saturation != settings.saturation &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", Math::clamp(settings.saturation, 0, 255)))
    {
        saturation = settings.saturation;
    }
    if (whiteBalanceTemperature != settings.whiteBalanceTemperature &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", Math::clamp(settings.whiteBalanceTemperature, 2700, 6500)))
    {
        whiteBalanceTemperature = settings.whiteBalanceTemperature;
    }

    if (gain != settings.gain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(settings.gain))))
    {
        gain = settings.gain;
    }

    setSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2, settings.horizontalFlip);

    setSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2, settings.verticalFlip);
}
