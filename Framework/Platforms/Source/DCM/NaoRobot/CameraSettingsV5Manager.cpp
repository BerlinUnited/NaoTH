#include "CameraSettingsV5Manager.h"

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

//Custom V4L control variables
#define V4L2_CID_EXPOSURE_ALGORITHM (V4L2_CID_CAMERA_CLASS_BASE + 17)
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

    settings.verticalFlip = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_VFLIP) == 0 ? false : true;
    settings.horizontalFlip = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HFLIP) == 0 ? false : true;

    settings.brightness = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BRIGHTNESS);
    settings.contrast = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST));
    settings.sharpness = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SHARPNESS);
    settings.hue = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HUE);

    settings.backlightCompensation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION) == 0 ? false : true;

    settings.v5.targetGain = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_TARGET_GAIN));
    settings.v5.minAnalogGain = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MIN_VIRT_AGAIN));
    settings.v5.maxAnalogGain = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MAX_VIRT_AGAIN));
    settings.v5.fadeToBlack = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_FADE_TO_BLACK) == 0 ? false : true;
    settings.v5.powerlineFrequency = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_POWER_LINE_FREQUENCY) == 2 ? 60 : 50;
    settings.v5.gammaCorrection = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAMMA);

    // get the autoexposure grid parameters
    for (std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++)
    {
        for (std::size_t j = 0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++)
        {
            autoExposureWeights[i][j] = static_cast<uint8_t>(getSingleCameraParameterRaw(cameraFd, cameraName, getAutoExposureGridID(i, j)));
        }
    }

    settings.v5.autoExpositionAlgorithm = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ALGORITHM);
}

void CameraSettingsV5Manager::apply(int cameraFd, std::string cameraName, const naoth::CameraSettings &settings)
{
    // Convert each paramter to a raw setting and clamp values according to the driver documentation
    // (https://github.com/bhuman/BKernel#information-about-the-camera-driver).
    // The V4L controller might later adjust these values by the ranges reported by driver, but these
    // might be inaccurate or less restricted. Also, for fixed point real numbers the clipping should
    // be performed for the real number range, not the byte-representation.
    bool requeryParams = false;
    if (autoExposition != settings.autoExposition &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "AutoExposure", settings.autoExposition ? 1 : 0))
    {
        if (settings.autoExposition == false)
        {
            requeryParams = true;
        }
        autoExposition = settings.autoExposition;
    }

    if (autoWhiteBalancing != settings.autoWhiteBalancing &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE, "AutoWhiteBalance",
                                    settings.autoWhiteBalancing ? 1 : 0))
    {
        if (settings.autoWhiteBalancing == false)
        {
            requeryParams = true;
        }

        autoWhiteBalancing = settings.autoWhiteBalancing;
    }

    if(requeryParams) {
        // read back white balanche values (and all others) set by the now deactivated auto exposure
        query(cameraFd, cameraName, *this);
        return;
    }


    if (autoExposition == false && exposure != settings.exposure &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", Math::clamp(settings.exposure, 0, 1000)))
    {
        exposure = settings.exposure;
        return;
    }

    if (saturation != settings.saturation &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", Math::clamp(settings.saturation, 0, 255)))
    {
        saturation = settings.saturation;
        return;
    }

    if (autoWhiteBalancing == false &&
        whiteBalanceTemperature != settings.whiteBalanceTemperature &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", Math::clamp(settings.whiteBalanceTemperature, 2700, 6500)))
    {
        whiteBalanceTemperature = settings.whiteBalanceTemperature;
        return;
    }

    if (!autoExposition && 
        gain != settings.gain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(settings.gain))))
    {
        gain = settings.gain;
        return;
    }

    if (brightness != settings.brightness &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BRIGHTNESS, "Brightness", Math::clamp(settings.brightness, 0, 255)))
    {
        brightness = settings.brightness;
        return;
    }

    if (contrast != settings.contrast &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST, "Contrast", Math::toFixPoint<5>(Math::clamp(static_cast<float>(settings.contrast), 0.5f, 2.0f))))
    {
        contrast = settings.contrast;
        return;
    }

    if (sharpness != settings.sharpness &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SHARPNESS, "Sharpness", Math::clamp(settings.sharpness, -7, 7)))
    {
        sharpness = settings.sharpness;
        return;
    }

    if (hue != settings.hue &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HUE, "Hue", Math::clamp(settings.hue, -22, 22)))
    {
        hue = settings.hue;
        return;
    }

    if (verticalFlip != settings.verticalFlip &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_VFLIP, "VerticalFlip", settings.verticalFlip ? 1 : 0))
    {
        verticalFlip = settings.verticalFlip;
        return;
    }

    if (horizontalFlip != settings.horizontalFlip && setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HFLIP, "HorizontalFlip", settings.horizontalFlip ? 1 : 0))
    {
        horizontalFlip = settings.horizontalFlip;
        return;
    }

    if (backlightCompensation != settings.backlightCompensation &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION, "BacklightCompensation", settings.backlightCompensation ? 0 : 1))
    {
        backlightCompensation = settings.backlightCompensation;
        return;
    }

    if (v5.minAnalogGain != settings.v5.minAnalogGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MIN_VIRT_AGAIN, "MinAnalogGain", 
            Math::clamp(Math::toFixPoint<5>(static_cast<float>(settings.v5.minAnalogGain)), 0, 65535)))
    {
        v5.minAnalogGain = settings.v5.minAnalogGain;
    }

    if (v5.maxAnalogGain != settings.v5.maxAnalogGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MAX_VIRT_AGAIN, "MaxAnalogGain", 
            Math::clamp(Math::toFixPoint<5>(static_cast<float>(settings.v5.maxAnalogGain)), 0, 65535)))
    {
        v5.maxAnalogGain = settings.v5.maxAnalogGain;
        return;
    }

    if (v5.targetGain != settings.v5.targetGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_TARGET_GAIN, "TargetGain", 
            Math::clamp(Math::toFixPoint<5>(static_cast<float>(settings.v5.targetGain)), 0, 65535)))
    {
        v5.targetGain = settings.v5.targetGain;
        return;
    }

    if (v5.fadeToBlack != settings.v5.fadeToBlack &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_FADE_TO_BLACK, "FadeToBlack", settings.v5.fadeToBlack ? 1 : 0))
    {
        v5.fadeToBlack = settings.v5.fadeToBlack;
        return;
    }

    // use 50 Hz (val = 1) if 60 Hz (val = 2) is not explicitly requested
    if (v5.powerlineFrequency != settings.v5.powerlineFrequency &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_POWER_LINE_FREQUENCY, "PowerlineFrequency", settings.v5.powerlineFrequency == 60 ? 2 : 1))
    {
        v5.powerlineFrequency = settings.v5.powerlineFrequency;
        return;
    }

    if (v5.gammaCorrection != settings.v5.gammaCorrection &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAMMA, "GammaCorrection", Math::clamp(settings.v5.gammaCorrection, 100, 280)))
    {
        v5.gammaCorrection = settings.v5.gammaCorrection;
        return;
    }

    if (v5.autoExpositionAlgorithm != settings.v5.autoExpositionAlgorithm && setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ALGORITHM, "AutoExposureAlgorithm", Math::clamp(settings.v5.autoExpositionAlgorithm, 0, 3)))
    {
        v5.autoExpositionAlgorithm = settings.v5.autoExpositionAlgorithm;
        return;
    }

    // set the autoexposure grid parameters
    for (std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++)
    {
        for (std::size_t j = 0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++)
        {
            if (autoExposureWeights[i][j] != settings.autoExposureWeights[i][j])
            {
                std::stringstream paramName;
                paramName << "autoExposureWeights (" << i << "," << j << ")";
                if (setSingleCameraParameterRaw(cameraFd, cameraName, getAutoExposureGridID(i, j), paramName.str(), settings.autoExposureWeights[i][j]))
                {
                    autoExposureWeights[i][j] = settings.autoExposureWeights[i][j];
                    return;
                }
            }
        }
    }
}

int CameraSettingsV5Manager::getAutoExposureGridID(size_t i, size_t j)
{
    return V4L2_CID_PRIVATE_BASE + 7 + (i * CameraSettings::AUTOEXPOSURE_GRID_SIZE) + j;
}
