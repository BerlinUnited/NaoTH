#include "CameraSettingsV5Manager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

//Custom V4L control variables
#define V4L2_MT9M114_FADE_TO_BLACK (V4L2_CID_PRIVATE_BASE) //boolean, enable or disable fade to black feature
#define V4L2_MT9M114_BRIGHTNESS_DARK (V4L2_CID_PRIVATE_BASE + 1)
#define V4L2_MT9M114_AE_TARGET_GAIN (V4L2_CID_PRIVATE_BASE + 2)
#define V4L2_MT9M114_AE_MIN_VIRT_AGAIN (V4L2_CID_PRIVATE_BASE + 3)
#define V4L2_MT9M114_AE_MAX_VIRT_AGAIN (V4L2_CID_PRIVATE_BASE + 4)
#define V4L2_MT9M114_AE_MIN_VIRT_DGAIN (V4L2_CID_PRIVATE_BASE + 5)
#define V4L2_MT9M114_AE_MAX_VIRT_DGAIN (V4L2_CID_PRIVATE_BASE + 6)

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

    settings.gain = Math::fromFixPoint<5>(static_cast<std::int32_t>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN)));

    settings.verticalFlip = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_VFLIP) == 0? false : true;
    settings.horizontalFlip = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HFLIP) == 0 ? false : true;

    settings.v5_targetGain = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_TARGET_GAIN));
    settings.v5_minAnalogGain = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MIN_VIRT_AGAIN));
    settings.v5_maxAnalogGain = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MAX_VIRT_AGAIN));
    settings.v5_fadeToBlack = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_FADE_TO_BLACK) == 0 ? false : true;
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

    if (v5_minAnalogGain != settings.v5_minAnalogGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MIN_VIRT_AGAIN, "MinAnalogGain", Math::clamp(Math::toFixPoint<5>(settings.v5_minAnalogGain), 0, 65535)))
    {
        v5_minAnalogGain = settings.v5_minAnalogGain;
    }

    if (v5_maxAnalogGain != settings.v5_maxAnalogGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MAX_VIRT_AGAIN, "MaxAnalogGain", Math::clamp(Math::toFixPoint<5>(settings.v5_maxAnalogGain), 0, 65535)))
    {
        v5_maxAnalogGain = settings.v5_maxAnalogGain;
    }

    if (v5_targetGain != settings.v5_targetGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_TARGET_GAIN, "TargetGain", Math::clamp(Math::toFixPoint<5>(settings.v5_targetGain), 0, 65535)))
    {
        v5_targetGain = settings.v5_targetGain;
    }

    if (v5_fadeToBlack != settings.v5_fadeToBlack && 
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_FADE_TO_BLACK, "FadeToBlack", settings.v5_fadeToBlack ? 1 : 0))
    {
        v5_fadeToBlack = settings.v5_fadeToBlack;
    }
}
