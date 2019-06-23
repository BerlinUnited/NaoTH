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

void CameraSettingsV5Manager::query(int cameraFd, std::string cameraName, naoth::CameraSettings &settings)
{
    settings.autoExposition = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO) == 0 ? false : true;

    settings.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE);
    settings.saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);
    settings.autoWhiteBalancing = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE) == 0 ? false : true;
    settings.whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

    std::int32_t gainRaw = static_cast<std::int32_t>(Math::clamp(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN), 0, 255));
    settings.gain = Math::fromFixPoint<5>(gainRaw);

    settings.verticalFlip = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_VFLIP) == 0 ? false : true;
    settings.horizontalFlip = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HFLIP) == 0 ? false : true;
}

void CameraSettingsV5Manager::apply(int cameraFd, std::string cameraName, const naoth::CameraSettings &settings)
{
    // Convert each paramter to a raw setting and clamp values according to the driver documentation
    // (https://github.com/bhuman/BKernel#information-about-the-camera-driver).
    // The V4L controller might later adjust these values by the ranges reported by driver, but these
    // might be inaccurate or less restricted. Also, for fixed point real numbers the clipping should
    // be performed for the real number range, not the byte-representation.

    if (autoExposition != settings.autoExposition &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "autoExposition", settings.autoExposition ? 1 : 0))
    {
        if (settings.autoExposition)
        {
            // read back exposure values (and all others) set by the now deactivated auto exposure
            query(cameraFd, cameraName, *this);
        }
        autoExposition = settings.autoExposition;
    }

    if (autoExposition == false && exposure != settings.exposure &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", Math::clamp(settings.exposure, 0, 1000)))
    {
        exposure = settings.exposure;
    }

    if (saturation != settings.exposure &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", Math::clamp(settings.saturation, 0, 255)))
    {
        saturation = settings.saturation;
    }

    if (autoWhiteBalancing != settings.autoWhiteBalancing &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE, "AutoWhiteBalance",
                                    settings.autoWhiteBalancing ? 1 : 0))
    {
        if (settings.autoWhiteBalancing == false)
        {
            // read back white balanche values (and all others) set by the now deactivated auto exposure
            query(cameraFd, cameraName, *this);
        }
        
        autoWhiteBalancing = settings.autoWhiteBalancing;
    }

    if (autoWhiteBalancing == false &&
        whiteBalanceTemperature != settings.whiteBalanceTemperature &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", Math::clamp(settings.whiteBalanceTemperature, 2700, 6500)))
    {
        whiteBalanceTemperature = settings.whiteBalanceTemperature;
    }

    if (gain != settings.gain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(settings.gain))))
    {
        gain = settings.gain;
    }

    if (verticalFlip != settings.verticalFlip &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_VFLIP, "VerticalFlip", settings.verticalFlip ? 1 : 0))
    {
        verticalFlip = settings.verticalFlip;
    }

    if (horizontalFlip != settings.horizontalFlip && setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HFLIP, "HorizontalFlip", settings.horizontalFlip ? 1 : 0))
    {
        horizontalFlip = settings.horizontalFlip;
    }
}
