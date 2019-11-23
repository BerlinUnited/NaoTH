#include "CameraSettingsV6Manager.h"

#include <chrono>
#include <thread>
#include <bitset>

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/uvcvideo.h>
#include <linux/usb/video.h>
}

using namespace naoth;

CameraSettingsV6Manager::CameraSettingsV6Manager()
    : initialized(false)
{
}

void CameraSettingsV6Manager::query(int cameraFd, const std::string& cameraName, CameraSettings &settings)
{
    settings.autoExposition = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO) == 0 ? true : false;

    settings.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE) / 100;
    settings.saturation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);

    settings.autoWhiteBalancing = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE) == 0 ? false : true;

    settings.gain = Math::fromFixPoint<5>(static_cast<std::int32_t>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN)));

    settings.brightness = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BRIGHTNESS);
    settings.contrast = Math::fromFixPoint<5>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST));
    settings.sharpness = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SHARPNESS);
    settings.hue = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HUE);

    settings.horizontalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2);
    settings.verticalFlip = getSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2);

    //    settings.backlightCompensation = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION) == 0 ? false : true;

    current = settings;
}

void CameraSettingsV6Manager::apply(int cameraFd, const std::string& cameraName, const CameraSettings &settings, bool force)
{
    if (!initialized)
    {
        // set some fixed values
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_FOCUS_AUTO, "FocusAuto", 0);
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_FOCUS_ABSOLUTE, "FocusAbsolute", 0);

        // enable test pattern
        //setRegister(cameraFd, 0x503D, 128);

        // HACK: make less greenish
        uint16_t regVal = getRegister(cameraFd, 0x5005);
        std::cout << "REG VAL " << std::bitset<16>(regVal) << std::endl;
        std::cout << "REG VAL (INT) " << regVal << std::endl;
        
        regVal ^= (1 << 5);
        regVal |= (1 << 6);
        std::cout << "REG VAL " << std::bitset<16>(regVal) << std::endl;
        std::cout << "REG VAL (INT) " << regVal << std::endl;
        
        setRegister(cameraFd, 0x5005, static_cast<uint16_t>(regVal));

        
        initialized = true;
    }

    if ((force || current.autoExposition != settings.autoExposition) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "ExposureAuto", settings.autoExposition ? 0 : 1))
    {
        if (settings.autoExposition == false)
        {
            // read back exposure values set by the now deactivated auto exposure
            current.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE) / 100;
            current.gain = Math::fromFixPoint<5>(static_cast<std::int32_t>(getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN)));

        }
        current.autoExposition = settings.autoExposition;
        return;
    }

    if (current.autoExposition == false && (force || current.exposure != settings.exposure) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE, "Exposure", settings.exposure * 100))
    {
        current.exposure = settings.exposure;
        return;
    }
    if (setRawIfChanged(cameraFd, cameraName, V4L2_CID_SATURATION, "Saturation", settings.saturation, current.saturation, force))
    {
        return;
    }

    if (setRawIfChanged(cameraFd, cameraName, V4L2_CID_BRIGHTNESS, "Brightness", settings.brightness, current.brightness, force))
    {
        return;
    }

    if ((force || current.contrast != settings.contrast) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST, "Contrast", Math::toFixPoint<5>(settings.contrast)))
    {
        current.contrast = settings.contrast;
        return;
    }

    // TODO, norm sharpness to something that is valid both on V5 and V6 *or* split up the params
    if (setRawIfChanged(cameraFd, cameraName, V4L2_CID_SHARPNESS, "Sharpness", settings.sharpness, current.sharpness, force))
    {
        return;
    }

    if (setRawIfChanged(cameraFd, cameraName, V4L2_CID_HUE, "Hue", settings.hue, current.hue, force))
    {
        return;
    }

    if ((force || current.autoWhiteBalancing != settings.autoWhiteBalancing) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE, "AutoWhiteBalance",
                                    settings.autoWhiteBalancing ? 1 : 0))
    {
        if (settings.autoWhiteBalancing == false)
        {
            // TODO: read back white balanche values set by the now deactivated auto white balance
        }
        current.autoWhiteBalancing = settings.autoWhiteBalancing;
        return;
    }

    // if (autoWhiteBalancing == false && v6.awbBottom != settings.v6.awbBottom &&
    //     setRegister(cameraFd, 0x5192, static_cast<uint16_t>(settings.v6.awbBottom)))
    // {
    //     v6.awbBottom = settings.v6.awbBottom;
    //     return;
    // }

    // if (autoWhiteBalancing == false && v6.awbTop != settings.v6.awbTop &&
    //     setRegister(cameraFd, 0x5191, static_cast<uint16_t>(settings.v6.awbTop)))
    // {
    //     v6.awbTop = settings.v6.awbTop;
    //     return;
    // }

    if ((force || current.gain != settings.gain) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", Math::toFixPoint<5>(static_cast<float>(settings.gain))))
    {
        current.gain = settings.gain;
        return;
    }

    // if (backlightCompensation != settings.backlightCompensation &&
    //     setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BACKLIGHT_COMPENSATION, "BacklightCompensation", settings.backlightCompensation ? 0 : 1))
    // {
    //     backlightCompensation = settings.backlightCompensation;
    // }

    if ((force || current.horizontalFlip != settings.horizontalFlip) &&
        setSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2, settings.horizontalFlip)) // 0x0C
    {
        current.horizontalFlip = settings.horizontalFlip;
        return;
    }

    if ((force || current.verticalFlip != settings.verticalFlip) &&
        setSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2, settings.verticalFlip)) // 0x0D
    {
        current.verticalFlip = settings.verticalFlip;
        return;
    }
}

uint16_t CameraSettingsV6Manager::getRegister(int cameraFd, uint16_t addr)
{

    // construct the query struct
    uvc_xu_control_query xu_query;
    std::memset(&xu_query, 0, sizeof(xu_query));
    xu_query.unit = 3;
    // selecting register control on the microcontroller?
    xu_query.selector = 0x0e;
    xu_query.query = UVC_SET_CUR;
    xu_query.size = 5;

    // contruct the data block
    std::uint8_t data[5];
    std::memset(data, 0, 5);
    // set flag to "Read"
    data[0] = 0;
    // split 16-bit address into two 8-bit parts
    data[1] = static_cast<uint8_t>(addr >> 8);
    data[2] = static_cast<uint8_t>(addr & 0xff);
    xu_query.data = data;
    if (-1 == ioctl(cameraFd, UVCIOC_CTRL_QUERY, &xu_query))
    {
        std::cerr << "(ERROR) : UVCIOC_CTRL_QUERY failed: " << std::strerror(errno);
        assert(false);
    }

    // wait for the microcontroller to query the register from the camera
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // query the value
    xu_query.query = UVC_GET_CUR;
    if (-1 == ioctl(cameraFd, UVCIOC_CTRL_QUERY, &xu_query))
    {
        std::cerr << "(ERROR) : UVCIOC_CTRL_QUERY failed: " << std::strerror(errno);
        assert(false);
    }

    return static_cast<uint16_t>((std::uint16_t(data[3]) << 8) | std::uint16_t(data[4]));
}

bool CameraSettingsV6Manager::setRegister(int cameraFd, uint16_t addr, uint16_t value)
{
    // 0x503D is the register that is responsible for configuring the camera's test pattern.
    // It defaults to 0.
    //const std::uint16_t addr = 0x503D
    // bit 7 is responsible for enabling the test pattern (128dec)
    //const std::uint16_t value = 128;

    // construct the query struct
    struct uvc_xu_control_query xu_query;
    std::memset(&xu_query, 0, sizeof(xu_query));
    xu_query.unit = 3;
    // selecting register control on the microcontroller?
    xu_query.selector = 0x0e;
    xu_query.query = UVC_SET_CUR;
    xu_query.size = 5;

    std::uint8_t data[5];
    std::memset(data, 0, 5);
    // set flag to "Write"
    data[0] = 1;
    // split 16-bit address into two 8-bit parts
    data[1] = static_cast<uint8_t>(addr >> 8);
    data[2] = static_cast<uint8_t>(addr & 0xff);
    // split 16-bit value into two 8-bit parts
    data[3] = static_cast<uint8_t>(value >> 8);
    data[4] = static_cast<uint8_t>(value & 0xff);
    xu_query.data = data;
    if (-1 == ioctl(cameraFd, UVCIOC_CTRL_QUERY, &xu_query))
    {
        std::cerr << "(ERROR) : UVC_SET_CUR failed: " << std::strerror(errno);
        return false;
    }
    return true;
}