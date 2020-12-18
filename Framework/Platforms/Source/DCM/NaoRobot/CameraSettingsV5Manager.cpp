#include "CameraSettingsV5Manager.h"

#include <thread>
#include <chrono>

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
    : initialized(false)
{
}

void CameraSettingsV5Manager::query(int cameraFd, const std::string& cameraName, naoth::CameraSettings &settings)
{
    settings.autoExposition = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO) == 0 ? false : true;

    settings.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE);

    settings.saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);
    settings.autoWhiteBalancing = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE) == 0 ? false : true;
    settings.v5.whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);

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
    for (std::size_t i = 0; i < naoth::CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++)
    {
        for (std::size_t j = 0; j < naoth::CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++)
        {
            settings.autoExposureWeights[i][j] = static_cast<uint8_t>(getSingleCameraParameterRaw(cameraFd, cameraName, getAutoExposureGridID(i, j)));
        }
    }

    settings.v5.autoExpositionAlgorithm = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ALGORITHM);

    current = settings;
}

void CameraSettingsV5Manager::apply(int cameraFd, const std::string& cameraName, const naoth::CameraSettings &settings, bool force)
{
    if (!initialized)
    {
        // HACK: disable autoexposure to make sure that the manual exposure is set correctly
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "AutoExposure", 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        current.autoExposition = false;

        // HACK: change exposure to some arbitrary value, to make sure that the actual value is actually applied later
        current.exposure = (current.exposure == 40) ? 41 : 40;
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", current.exposure);
         
        initialized = true;
        return;
    }

    // Convert each paramter to a raw setting and clamp values according to the driver documentation
    // (https://github.com/bhuman/BKernel#information-about-the-camera-driver).
    // The V4L controller might later adjust these values by the ranges reported by driver, but these
    // might be inaccurate or less restricted. Also, for fixed point real numbers the clipping should
    // be performed for the real number range, not the byte-representation.

    if ((force || current.verticalFlip != settings.verticalFlip) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_VFLIP, "VerticalFlip", settings.verticalFlip ? 1 : 0))
    {
        current.verticalFlip = settings.verticalFlip;
        return;
    }

    if ((force || current.horizontalFlip != settings.horizontalFlip) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HFLIP, "HorizontalFlip", settings.horizontalFlip ? 1 : 0))
    {
        current.horizontalFlip = settings.horizontalFlip;
        return;
    }

    if ((force || current.autoExposition != settings.autoExposition) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "AutoExposure", settings.autoExposition ? 1 : 0))
    {
        // allways wait a little bit after setting the auto exposure
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (settings.autoExposition == false)
        {
            // read back values set by the now deactivated auto exposure
            current.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE);
            current.gain = Math::fromFixPoint<5>(static_cast<std::int32_t>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN)));
        }
        current.autoExposition = settings.autoExposition;
        return;
    }

    if (setRawIfChanged(cameraFd, cameraName, V4L2_CID_EXPOSURE_ALGORITHM, "AutoExposureAlgorithm",
                        Math::clamp(settings.v5.autoExpositionAlgorithm, 0, 3), current.v5.autoExpositionAlgorithm, force))
    {
        return;
    }

    if (current.brightness != settings.brightness &&
        setRawIfChanged(cameraFd, cameraName, V4L2_CID_BRIGHTNESS, "Brightness", Math::clamp(settings.brightness, 0, 255), current.brightness, force))
    {
        return;
    }

    if (current.v5.minAnalogGain != settings.v5.minAnalogGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MIN_VIRT_AGAIN, "MinAnalogGain",
                                    Math::clamp(Math::toFixPoint<5>(static_cast<float>(settings.v5.minAnalogGain)), 0, 65535)))
    {
        current.v5.minAnalogGain = settings.v5.minAnalogGain;
        return;
    }

    if (current.v5.maxAnalogGain != settings.v5.maxAnalogGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_MAX_VIRT_AGAIN, "MaxAnalogGain",
                                    Math::clamp(Math::toFixPoint<5>(static_cast<float>(settings.v5.maxAnalogGain)), 0, 65535)))
    {
        current.v5.maxAnalogGain = settings.v5.maxAnalogGain;
        return;
    }

    if (current.v5.targetGain != settings.v5.targetGain &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_AE_TARGET_GAIN, "TargetGain",
                                    Math::clamp(Math::toFixPoint<5>(static_cast<float>(settings.v5.targetGain)), 0, 65535)))
    {
        current.v5.targetGain = settings.v5.targetGain;
        return;
    }

    if (current.autoWhiteBalancing != settings.autoWhiteBalancing &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE, "AutoWhiteBalance",
                                    settings.autoWhiteBalancing ? 1 : 0))
    {
        if (settings.autoWhiteBalancing == false)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // read back white balanche values set by the now deactivated auto exposure
            current.v5.whiteBalanceTemperature = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE);
        }

        current.autoWhiteBalancing = settings.autoWhiteBalancing;
        return;
    }

    // use 50 Hz (val = 1) if 60 Hz (val = 2) is not explicitly requested
    if (current.v5.powerlineFrequency != settings.v5.powerlineFrequency &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_POWER_LINE_FREQUENCY, "PowerlineFrequency", settings.v5.powerlineFrequency == 60 ? 2 : 1))
    {
        current.v5.powerlineFrequency = settings.v5.powerlineFrequency;
        return;
    }

    if (current.contrast != settings.contrast &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST, "Contrast", Math::toFixPoint<5>(Math::clamp(static_cast<float>(settings.contrast), 0.5f, 2.0f))))
    {
        current.contrast = settings.contrast;
        return;
    }

    if (current.saturation != settings.saturation &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", Math::clamp(settings.saturation, 0, 255)))
    {
        current.saturation = settings.saturation;
        return;
    }

    if (current.hue != settings.hue &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HUE, "Hue", Math::clamp(settings.hue, -22, 22)))
    {
        current.hue = settings.hue;
        return;
    }

    if (current.sharpness != settings.sharpness &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SHARPNESS, "Sharpness", Math::clamp(settings.sharpness, -7, 7)))
    {
        current.sharpness = settings.sharpness;
        return;
    }

    if (!current.autoExposition && (force || current.exposure != settings.exposure) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE, "Exposure", Math::clamp(settings.exposure, 0, 1000)))
    {
        current.exposure = settings.exposure;
        return;
    }

    if (!current.autoExposition &&
        (force || current.gain != settings.gain) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(Math::clamp(settings.gain, 1.0, 7.9)))))
    {
        current.gain = settings.gain;
        return;
    }

    if (current.v5.gammaCorrection != settings.v5.gammaCorrection &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAMMA, "GammaCorrection", Math::clamp(settings.v5.gammaCorrection, 100, 280)))
    {
        current.v5.gammaCorrection = settings.v5.gammaCorrection;
        return;
    }

    if (current.autoWhiteBalancing == false &&
        current.v5.whiteBalanceTemperature != settings.v5.whiteBalanceTemperature &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance", Math::clamp(settings.v5.whiteBalanceTemperature, 2700, 6500)))
    {
        current.v5.whiteBalanceTemperature = settings.v5.whiteBalanceTemperature;
        return;
    }

    // this throws errors sometimes and slows down the robot, check whats wrong before activating it
    // if (
    //     autoExposition &&
    //     backlightCompensation != settings.backlightCompensation &&
    //      setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION, "BacklightCompensation", settings.backlightCompensation ? 0 : 1))
    // {
    //      backlightCompensation = settings.backlightCompensation;
    //      return;
    // }

    if (current.v5.fadeToBlack != settings.v5.fadeToBlack &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_MT9M114_FADE_TO_BLACK, "FadeToBlack", settings.v5.fadeToBlack ? 1 : 0))
    {
        current.v5.fadeToBlack = settings.v5.fadeToBlack;
        return;
    }

    // set the autoexposure grid parameters
    for (std::size_t i = 0; i < naoth::CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++)
    {
        for (std::size_t j = 0; j < naoth::CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++)
        {
            if (current.autoExposureWeights[i][j] != settings.autoExposureWeights[i][j])
            {
                std::stringstream paramName;
                paramName << "autoExposureWeights (" << i << "," << j << ")";
                if (setSingleCameraParameterRaw(cameraFd, cameraName, getAutoExposureGridID(i, j), paramName.str(), settings.autoExposureWeights[i][j]))
                {
                    current.autoExposureWeights[i][j] = settings.autoExposureWeights[i][j];
                    return;
                }
            }
        }
    }
}

int CameraSettingsV5Manager::getAutoExposureGridID(size_t i, size_t j)
{
    return V4L2_CID_PRIVATE_BASE + 7 + (i * naoth::CameraSettings::AUTOEXPOSURE_GRID_SIZE) + j;
}
