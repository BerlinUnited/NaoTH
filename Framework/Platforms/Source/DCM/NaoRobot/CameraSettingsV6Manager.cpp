#include "CameraSettingsV6Manager.h"

// NOTE: this is currently only needed for sleep
//#include <chrono>
//#include <thread>

extern "C"
{
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/uvcvideo.h>
#include <linux/usb/video.h>
}

using namespace naoth;

CameraSettingsV6Manager::CameraSettingsV6Manager(int cameraFd,
                                                 const std::string& cameraName)
    : 
  initialized(false), 
  cameraFd(cameraFd), 
  cameraName(cameraName) 
{}

/*
Control 10094849: Exposure, Auto
  Menu items:
  Auto Mode\n  Manual Mode\n
  
Control 9963776: Brightness
Control 9963795: Gain
Control 10094850: Exposure (Absolute)

Control 9963777: Contrast
Control 9963778: Saturation

Control 9963801: Hue, Auto
Control 9963779: Hue

Control 9963788: White Balance Temperature, Auto
Control 9963802: White Balance Temperature

Control 9963803: Sharpness

Control 10094858: Focus (absolute)
Control 10094860: Focus, Auto
*/

// NOTE:
/*
https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/ext-ctrls-camera.html
V4L2_CID_EXPOSURE_ABSOLUTE (integer)
    Determines the exposure time of the camera sensor. 
    The exposure time is limited by the frame interval. 
    Drivers should interpret the values as 100 µs units, 
    where the value 1 stands for 1/10000th of a second, 
    10000 for 1 second and 100000 for 10 seconds.
*/

void CameraSettingsV6Manager::query(CameraSettings &all)
{
  V6CameraSettings& settings(all.v6);
  
  // V4L query
  // NOTE: V4L2_CID_EXPOSURE_AUTO is a menu type parameter with items {0: Auto Mode, 1: Manual Mode}
  settings.autoExposition     = (getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO) == 0);

  settings.exposure           = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE);
  settings.saturation         = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SATURATION);

  settings.autoWhiteBalancing = (getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE) == 1);
  
  settings.gain_red   = getRegister32(cameraFd, 0x3400);
  settings.gain_green = getRegister32(cameraFd, 0x3402);
  settings.gain_blue  = getRegister32(cameraFd, 0x3404);
  
  // NOTE: the the current point the white_balancing parameter can be set but doesn't see to have any effect
  // settings.white_balancing = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE) == 0 ? false : true;
  
  // what about the automatic gain?
  settings.gain               = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN);

  settings.brightness         = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_BRIGHTNESS);
  settings.contrast           = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST);
  settings.sharpness          = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_SHARPNESS);
  settings.hue                = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_HUE);

  // TODO: add NAO V6 speciffic parameters? For now fixed values are set (see apply(...))
  // settings.autoFocus = ...
  // settings.focus = ...
  
  // UVC parameters
  // NOTE: can we just set those parameters fix?
  //settings.horizontalFlip     = (uint16_t)getSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2);
  settings.horizontalFlip     = getParameterUVC<uint16_t>(cameraFd, cameraName, 12, "HorizontalFlip"); // 0x0C
  //settings.verticalFlip       =(uint16_t)getSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2);
  settings.verticalFlip       = getParameterUVC<uint16_t>(cameraFd, cameraName, 13, "VerticalFlip"); // 0x0D
  
  current = settings;
}

void CameraSettingsV6Manager::apply(const CameraSettings &all, bool force)
{
  const V6CameraSettings& settings(all.v6);
  
    if (!initialized)
    {
        // set some fixed values
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_FOCUS_AUTO, "FocusAuto", 0);
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_FOCUS_ABSOLUTE, "FocusAbsolute", 0);
        
        // DEBUG: enable test pattern
        // Address: 0x503D is the register that is responsible for configuring the camera's test pattern.
        // It defaults to 0.
        // Value: bit 7 (128) is responsible for enabling the test pattern (128dec)
        //setRegister(cameraFd, 0x503D, 128));
        //uint16_t v = getRegister(cameraFd, 0x503D);
        //std::cout << "0x503D: " << v << std::endl;
        
        // Test: disable "special digital effects" (sde)
        /*
        uint16_t sde = getRegister(cameraFd, 0x5001);
        std::cout << "sde " << std::bitset<16>(sde) << std::endl;
        sde = reset(sde, 7);
        std::cout << "sde " << std::bitset<16>(sde) << std::endl;
        setRegister(cameraFd, 0x5001, sde);
        */
        
        // https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/OV5640_datasheet.pdf
        // 7.21ISP top control:
        // AWB bias manual enable (make less greenish)
        /*
        uint16_t regVal = getRegister(cameraFd, 0x5005);
        std::cout << "REG VAL " << std::bitset<16>(regVal) << std::endl;
        std::cout << "REG VAL (INT) " << regVal << std::endl;
        
        regVal ^= (1 << 5);
        regVal |= (1 << 6);
        std::cout << "REG VAL " << std::bitset<16>(regVal) << std::endl;
        std::cout << "REG VAL (INT) " << regVal << std::endl;
        */
        //setRegister(cameraFd, 0x5005, static_cast<uint16_t>(regVal));
        
        
        // test: set color gains
        /*
        setRegister32(cameraFd, 0x3400, static_cast<uint32_t>(1024)); // red
        setRegister32(cameraFd, 0x3402, static_cast<uint32_t>(1024)); // green
        setRegister32(cameraFd, 0x3404, static_cast<uint32_t>(1024)); // blue
        */
        
        
        // test: set light frequency
        /*
        uint16_t band_mode = getRegister(cameraFd, 0x3C01);
        std::cout << "band_mode " << std::bitset<16>(band_mode) << std::endl;
        band_mode = set(band_mode, 7); // set the bit 7 to 1
        setRegister(cameraFd, 0x3C01, band_mode);
        std::cout << "band_mode " << std::bitset<16>(band_mode) << std::endl;
        
        uint16_t band_ctrl = getRegister(cameraFd, 0x3C00);
        std::cout << "band_ctrl " << std::bitset<16>(band_ctrl) << std::endl;
        band_ctrl = set(band_ctrl, 2); // set the bit 2 to 0
        std::cout << "band_ctrl " << std::bitset<16>(band_ctrl) << std::endl;
        setRegister(cameraFd, 0x3C00, band_ctrl);
        
        
        uint16_t band_value = getRegister(cameraFd, 0x3C0C);
        std::cout << "Band test: " << band_mode << ", " << band_value << std::endl;
        */
        
        // NOTE: for future tests: gain registers 0x350A/0x350B
        
        initialized = true;
    }

    if ((force || current.horizontalFlip != settings.horizontalFlip) &&
        //setSingleCameraParameterUVC(cameraFd, cameraName, 12, "HorizontalFlip", 2, settings.horizontalFlip ? 1 : 0)) // 0x0C
        setParameterUVC<uint16_t>(cameraFd, cameraName, 12, "HorizontalFlip", settings.horizontalFlip ? 1 : 0)) // 0x0C
    {
        current.horizontalFlip = settings.horizontalFlip;
        return;
    }

    if ((force || current.verticalFlip != settings.verticalFlip) &&
        //setSingleCameraParameterUVC(cameraFd, cameraName, 13, "VerticalFlip", 2, settings.verticalFlip ? 1 : 0)) // 0x0D
        setParameterUVC<uint16_t>(cameraFd, cameraName, 13, "VerticalFlip", settings.verticalFlip ? 1 : 0)) // 0x0D
    {
        current.verticalFlip = settings.verticalFlip;
        return;
    }


    // NOTE: V4L2_CID_EXPOSURE_AUTO is a menu type parameter with items {0: Auto Mode, 1: Manual Mode}
    if ((force || current.autoExposition != settings.autoExposition) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_AUTO, "ExposureAuto", settings.autoExposition ? 0 : 1))
    {
        if (settings.autoExposition == false)
        {
            // read exposure and gain values set by the now deactivated auto exposure
            current.exposure = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE);
            current.gain = getSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN);

        }
        current.autoExposition = settings.autoExposition;
        return;
    }

    if (current.autoExposition == false && (force || current.exposure != settings.exposure) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_EXPOSURE_ABSOLUTE, "Exposure", settings.exposure))
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
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_CONTRAST, "Contrast", settings.contrast))
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
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_AUTO_WHITE_BALANCE, "AutoWhiteBalance", settings.autoWhiteBalancing ? 1 : 0))
    {
        if (settings.autoWhiteBalancing == false)
        {
          
          // TODO: read white balanche values set by the now deactivated auto white balance into parameters
          // Read the white balance values from the long registers.
          // See also:
          //   https://github.com/bhuman/BHumanCodeRelease/blob/d7deadc6f1a4c445c4bbd2e9f256bf058b80a24c/Src/Platform/Nao/NaoCamera.cpp#L436
          //   https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/OV5640_datasheet.pdf
          //   7.4AWB gain control [0x3400 ~ 0x3406]
          current.gain_red   = getRegister32(cameraFd, 0x3400);
          current.gain_green = getRegister32(cameraFd, 0x3402);
          current.gain_blue  = getRegister32(cameraFd, 0x3404);
          
          // print for debug
          std::cout << LOG << " white balance: " << std::endl;
          std::cout << "  red:   " << current.gain_red   << std::endl;
          std::cout << "  green: " << current.gain_green << std::endl;
          std::cout << "  blue:  " << current.gain_blue  << std::endl;
        }
        
        current.autoWhiteBalancing = settings.autoWhiteBalancing;
        return;
    }
    
    // manual white balance
    if (current.autoWhiteBalancing == false) 
    {
      if ((force || current.gain_red != settings.gain_red) &&
          setRegister32(cameraFd, 0x3400, static_cast<uint32_t>(settings.gain_red))) {
        current.gain_red = settings.gain_red;
      }
      
      if ((force || current.gain_green != settings.gain_green) &&
          setRegister32(cameraFd, 0x3402, static_cast<uint32_t>(settings.gain_green))) {
        current.gain_green = settings.gain_green;
      }
      
      if ((force || current.gain_blue != settings.gain_blue) &&
          setRegister32(cameraFd, 0x3404, static_cast<uint32_t>(settings.gain_blue))){
        current.gain_blue = settings.gain_blue;
      }
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

    
    // NOTES on gain
    // https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/OV5640_datasheet.pdf
    // 4.6.4: The OV5640 has a maximum of 64x gain.
    // The value we can set is 0...1023. 1023 / 64 = 16 => 16 corresponds to 1x gain.
    if ((force || current.gain != settings.gain) &&
        setSingleCameraParameterRaw(cameraFd, cameraName, V4L2_CID_GAIN, "Gain", settings.gain))
    {
        current.gain = settings.gain;
        
        // DEBUG: print color gains
        uint32_t red_gain   = getRegister32(cameraFd, 0x3400);
        uint32_t green_gain = getRegister32(cameraFd, 0x3402);
        uint32_t blue_gain  = getRegister32(cameraFd, 0x3404);
        std::cout << LOG << " white balance: " << std::endl;
        std::cout << "  red:   " << red_gain   << std::endl;
        std::cout << "  green: " << green_gain << std::endl;
        std::cout << "  blue:  " << blue_gain  << std::endl;
        
        return;
    }
}

uint16_t CameraSettingsV6Manager::getRegister(int cameraFd, uint16_t addr)
{
  Register data {
    .id = 0, // read register
    .addr = swapBytes(addr), // to little-endian
    .value = 0 // some default value
  };
  
  // request the value from the microcontroller
  if(!setParameterUVC(cameraFd, "cam", 0x0e, "register", data)) {
    std::cerr << "(ERROR) : getRegister failed while writing command: " << std::strerror(errno);
    assert(false); // query the value
  }
  
  // wait for the microcontroller to query the register from the camera
  //std::this_thread::sleep_for(std::chrono::milliseconds(500));
  usleep(100000); // 100ms
  
  // query the value
  if(!getParameterUVC(cameraFd, "cam", 0x0e, "register", data)) {
    std::cerr << "(ERROR) : getRegister failed while erading value: " << std::strerror(errno);
    assert(false);
  }
  
  uint16_t value = swapBytes(data.value);
  std::cout << " getRegister " << addr << ": " << value << std::endl;
  
  return value;
}

uint32_t CameraSettingsV6Manager::getRegister32(int cameraFd, uint16_t addr)
{
  uint16_t hi = getRegister(cameraFd, addr);
  uint16_t lo = getRegister(cameraFd, static_cast<uint16_t>(addr + 1));
  return static_cast<uint32_t>(hi << 8 | lo);
}

bool CameraSettingsV6Manager::setRegister32(int cameraFd, uint16_t addr, uint32_t value)
{
  std::cout << " setRegister32 " << addr << ": " << value << std::endl;
  return setRegister(cameraFd, addr,                            static_cast<uint16_t>(value >> 8)) &&
  setRegister(cameraFd, static_cast<uint16_t>(addr + 1), static_cast<uint16_t>(value & 0xff));
}

bool CameraSettingsV6Manager::setRegister(int cameraFd, uint16_t addr, uint16_t value)
{  
    /*
    // DEBUG
    {
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
    print_bytes(data, 5);
    }
    */
    
    Register data {
      .id = 1, // write register
      .addr = swapBytes(addr), // to little-endian
      .value = swapBytes(value)// to little-endian
    };
    
    //print_bytes((uint8_t*)&data, 5);
    if(!setParameterUVC(cameraFd, "cam", 0x0e, "register", data)) {
      std::cerr << "(ERROR) : setRegister failed: " << std::strerror(errno);
      return false;
    }
    
    std::cout << " setRegister " << addr << ": " << value << std::endl;
    
    // https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/OV5640_datasheet.pdf
    //"8.4AC characteristics": settling time for register setting<300ms
    
    // wait for the query to be processed before other requests
    usleep(100000); // 100ms
    
    return true;
     
}