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

    settings.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE);
    settings.saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);

    settings.autoWhiteBalancing = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE) == 0 ? false : true;
    settings.whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

    settings.gain = Math::fromFixPoint<5>(static_cast<std::int32_t>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN)));

    settings.horizontalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2);
    settings.verticalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2);

    settings.backlightCompensation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION) == 0 ? false : true;
}

void CameraSettingsV6Manager::apply(int cameraFd, std::string cameraName, const CameraSettings &settings)
{
    if (autoExposition != settings.autoExposition &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "autoExposition", settings.autoExposition ? 0 : 1))
    {
        if (settings.autoExposition == false)
        {
            // read back exposure values (and all others) set by the now deactivated auto exposure
            query(cameraFd, cameraName, *this);
        }
        autoExposition = settings.autoExposition;
    }

    if (autoExposition == false && exposure != settings.exposure &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE, "Exposure", Math::clamp(settings.exposure, 0, 1000)))
    {
        exposure = settings.exposure;
    }
    if (saturation != settings.saturation &&
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

    if (autoWhiteBalancing == false && whiteBalanceTemperature != settings.whiteBalanceTemperature &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", Math::clamp(settings.whiteBalanceTemperature, 2500, 6500)))
    {
        whiteBalanceTemperature = settings.whiteBalanceTemperature;
    }

    if (gain != settings.gain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(settings.gain))))
    {
        gain = settings.gain;
    }

    if (backlightCompensation != settings.backlightCompensation &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION, "BacklightCompensation", settings.backlightCompensation ? 0 : 1))
    {
        backlightCompensation = settings.backlightCompensation;
    }

    setSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2, settings.horizontalFlip);

    setSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2, settings.verticalFlip);
}
