#include "CameraSettingsV6Manager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

using namespace naoth;

CameraSettingsV6Manager::CameraSettingsV6Manager()
: initialized(false)
{
}

void CameraSettingsV6Manager::query(int cameraFd, std::string cameraName, CameraSettings &settings)
{
    settings.autoExposition = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO) == 0 ? true : false;

    settings.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE) / 100;
    settings.saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);

    settings.autoWhiteBalancing = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE) == 0 ? false : true;
    settings.whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

    settings.gain = Math::fromFixPoint<5>(static_cast<std::int32_t>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN)));

    settings.brightness = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BRIGHTNESS);
    settings.contrast = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST));
    settings.sharpness = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SHARPNESS);
    settings.hue = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HUE);

    settings.horizontalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2);
    settings.verticalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2);

    //    settings.backlightCompensation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION) == 0 ? false : true;
}

void CameraSettingsV6Manager::apply(int cameraFd, std::string cameraName, const CameraSettings &settings)
{
    if(!initialized) {
        // set some fixed values
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_FOCUS_AUTO, "FocusAuto", 0);
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_FOCUS_ABSOLUTE, "FocusAbsolute", 0);

        initialized = true;
    }

    if (autoExposition != settings.autoExposition &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "ExposureAuto", settings.autoExposition ? 0 : 1))
    {
        if (settings.autoExposition == false)
        {
            // read back exposure values (and all others) set by the now deactivated auto exposure
            query(cameraFd, cameraName, *this);
        }
        autoExposition = settings.autoExposition;
        return;
    }

    if (autoExposition == false && exposure != settings.exposure &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE, "Exposure", settings.exposure * 100))
    {
        exposure = settings.exposure;
        return;
    }
    if (saturation != settings.saturation &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", settings.saturation))
    {
        saturation = settings.saturation;
        return;
    }

    // TODO, norm brigthness, contrast, sharpness and hue to something that is valid both on V5 and V6 or split up the params
    if (brightness != settings.brightness &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BRIGHTNESS, "Brightness", settings.brightness))
    {
        brightness = settings.brightness;
        return;
    }

    if (contrast != settings.contrast &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST, "Contrast", Math::toFixPoint<5>(settings.contrast)))
    {
        contrast = settings.contrast;
        return;
    }

    if (sharpness != settings.sharpness &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SHARPNESS, "Sharpness", settings.sharpness))
    {
        sharpness = settings.sharpness;
        return;
    }

    if (hue != settings.hue &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HUE, "Hue", settings.hue))
    {
        hue = settings.hue;
        return;
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
        return;
    }

    if (autoWhiteBalancing == false && whiteBalanceTemperature != settings.whiteBalanceTemperature &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", settings.whiteBalanceTemperature))
    {
        whiteBalanceTemperature = settings.whiteBalanceTemperature;
        return;
    }

    if (gain != settings.gain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(settings.gain))))
    {
        gain = settings.gain;
        return;
    }

    // if (backlightCompensation != settings.backlightCompensation &&
    //     setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION, "BacklightCompensation", settings.backlightCompensation ? 0 : 1))
    // {
    //     backlightCompensation = settings.backlightCompensation;
    // }

    if (horizontalFlip != settings.horizontalFlip &&
        setSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2, settings.horizontalFlip))
    {
        horizontalFlip = settings.horizontalFlip;
        return;
    }

    if (verticalFlip != settings.verticalFlip &&
        setSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2, settings.verticalFlip))
    {
        verticalFlip = settings.verticalFlip;
        return;
    }
}
