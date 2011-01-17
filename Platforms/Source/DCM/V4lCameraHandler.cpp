/* 
 * File:   V4lCameraHandler.cpp
 * Author: thomas
 * 
 * Created on 22. April 2010, 17:24
 */

#include "V4lCameraHandler.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Representations/Infrastructure/Image.h"
#include "PlatformInterface/Platform.h"

#include <string.h>
#include <iostream>

extern "C"
{
#include "i2c-small.h"
}
// define some non-standard constants
#ifndef V4L2_CID_AUTOEXPOSURE
#define V4L2_CID_AUTOEXPOSURE     (V4L2_CID_BASE+32)
#endif
#ifndef V4L2_CID_CAM_INIT
#define V4L2_CID_CAM_INIT         (V4L2_CID_BASE+33)
#endif

using namespace naoth;

V4lCameraHandler::V4lCameraHandler()
: fd(-1), buffers(NULL), n_buffers(0), wasQueried(false)
{

}

void V4lCameraHandler::init(std::string camDevice)
{
  for(int i=0; i < CameraInfo::numOfCamera; i++)
  {
    for(int j=0; j < CameraSettings::numOfCameraSetting; j++)
    {
      currentSettings[i].data[j] = -1;
    }
  }

  cameraName = camDevice;
  
  // set to the default camera
  fastCameraSelection(CameraInfo::Bottom);

  // set our IDs
  initIDMapping();

  // open the device
  openDevice();

  // init device for both cameras
  initDevice();
  fastCameraSelection(CameraInfo::Top);
  
  initDevice();
  fastCameraSelection(CameraInfo::Bottom);
  
  // allocate the internal camera buffers
  initMMap();

  internalUpdateCameraSettings();
  
  // start capturing
  startCapturing();
  setFPS(30);
  
}

void V4lCameraHandler::initIDMapping()
{
  // initialize with an invalid value in order not to be used when updating
  // the params in V4L
  for (int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    csConst[i] = -1;
  }


  // map the existing parameters that can be used safely
  csConst[CameraSettings::AutoExposition] = V4L2_CID_AUTOEXPOSURE;
  csConst[CameraSettings::AutoWhiteBalancing] = V4L2_CID_AUTO_WHITE_BALANCE;
  csConst[CameraSettings::AutoGain] = V4L2_CID_AUTOGAIN;
  csConst[CameraSettings::Brightness] = V4L2_CID_BRIGHTNESS;
  csConst[CameraSettings::Contrast] = V4L2_CID_CONTRAST;
  csConst[CameraSettings::Saturation] = V4L2_CID_SATURATION;
  csConst[CameraSettings::Hue] = V4L2_CID_HUE;
  csConst[CameraSettings::RedChroma] = V4L2_CID_RED_BALANCE;
  csConst[CameraSettings::BlueChroma] = V4L2_CID_BLUE_BALANCE;
  csConst[CameraSettings::Gain] = V4L2_CID_GAIN;
  csConst[CameraSettings::HorizontalFlip] = V4L2_CID_HFLIP;
  csConst[CameraSettings::VerticalFlip] = V4L2_CID_VFLIP;
  csConst[CameraSettings::Exposure] = V4L2_CID_EXPOSURE;

}

void V4lCameraHandler::fastCameraSelection(CameraInfo::CameraID camId)
{
  std::cout << "FAST camera selection to " << camId << std::endl;
  int i2cFd = open("/dev/i2c-0", O_RDWR);
  ASSERT(i2cFd != -1);
  VERIFY(ioctl(i2cFd, 0x703, 8) == 0);
  VERIFY(i2c_smbus_read_byte_data(i2cFd, 170) >= 2); // at least Nao V3
  unsigned char cmd[2] = {2, 0}; // select lower camera by default
  if (camId == CameraInfo::Top)
  {
    // adjust to the top camera
    cmd[0] = 1;
  }
  VERIFY(i2c_smbus_write_block_data(i2cFd, 220, 1, cmd) != -1);
  close(i2cFd);

  currentCamera = camId;
  currentSettings[camId].data[CameraSettings::CameraSelection] = camId;

}


void V4lCameraHandler::setFPS(int fpsRate)
{
  struct v4l2_streamparm fps;
  memset(&fps, 0, sizeof (struct v4l2_streamparm));
  fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(!ioctl(fd, VIDIOC_G_PARM, &fps));
  fps.parm.capture.timeperframe.numerator = 1;
  fps.parm.capture.timeperframe.denominator = fpsRate;
  VERIFY(ioctl(fd, VIDIOC_S_PARM, &fps) != -1);

  currentSettings[currentCamera].data[CameraSettings::FPS] = fpsRate;
}

void V4lCameraHandler::get(Image& theImage)
{
  int resultCode = readFrame();
  if (resultCode < 0)
  {
    std::cerr << "[V4L get] Could not get image, error code " << resultCode << std::endl;
  } else
  {
    theImage.wrapImageDataYUV422((unsigned char*) buffers[currentBuf.index].start, currentBuf.bytesused);
    //theImage.wrapImageDataYUV422((unsigned char*) buffers[resultCode].start, buffers[resultCode].length);
    theImage.setCameraInfo(Platform::getInstance().theCameraInfo);
    theImage.cameraInfo.cameraID = currentCamera;
  }

}

void V4lCameraHandler::openDevice()
{

  struct stat st;

  if (-1 == stat(cameraName.c_str(), &st))
  {
    std::cerr << "[V4L open] Cannot identify '" << cameraName << "': " << errno << ", "
      << strerror(errno) << std::endl;
    return;
  }

  if (!S_ISCHR(st.st_mode))
  {
    std::cerr << "[V4L open] " << cameraName << " is no device " << std::endl;
    return;
  }

  fd = open(cameraName.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

  if (-1 == fd)
  {
    std::cerr << "[V4L open] Cannot open '" << cameraName << "': " << errno << ", "
      << strerror(errno) << std::endl;
    return;
  }

}

void V4lCameraHandler::initDevice()
{
  struct v4l2_capability cap;
  unsigned int min;


  // set default parameters and init the camera
  struct v4l2_control control;
  memset(&control, 0, sizeof (struct v4l2_control));
  control.id = V4L2_CID_CAM_INIT;
  control.value = 0;
  VERIFY(ioctl(fd, VIDIOC_S_CTRL, &control) >= 0);

  v4l2_std_id esid0 = 0x04000000UL; // use 0x08000000UL when the width is 640
  VERIFY(!ioctl(fd, VIDIOC_S_STD, &esid0));

  memset(&(currentBuf), 0, sizeof (struct v4l2_buffer));
  currentBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  currentBuf.memory = V4L2_MEMORY_MMAP;

  VERIFY(ioctl(fd, VIDIOC_QUERYCAP, &cap) != -1);

  VERIFY(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE);
  VERIFY(cap.capabilities & V4L2_CAP_STREAMING);

  /* Select video input, video standard and tune here. */

  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof (struct v4l2_format));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 320;
  fmt.fmt.pix.height = 240;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  //  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB444;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  VERIFY(ioctl(fd, VIDIOC_S_FMT, &fmt) >= 0);

  /* Note VIDIOC_S_FMT may change width and height. */

  /* Buggy driver paranoia. */
  min = fmt.fmt.pix.width * 2;
  if (fmt.fmt.pix.bytesperline < min)
    fmt.fmt.pix.bytesperline = min;
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if (fmt.fmt.pix.sizeimage < min)
    fmt.fmt.pix.sizeimage = min;

  std::cout << fmt.fmt.pix.sizeimage << endl;
}

void V4lCameraHandler::initMMap()
{
  struct v4l2_requestbuffers req;

  memset(&(req), 0, sizeof (v4l2_requestbuffers));

  req.count = 5; // number of internal buffers, since we use debug images that should be quite big
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  VERIFY(-1 != ioctl(fd, VIDIOC_REQBUFS, &req));

  VERIFY(req.count >= 2);

  buffers = (struct buffer*) calloc(req.count, sizeof (*buffers));

  VERIFY(buffers);

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
  {
    struct v4l2_buffer buf;

    memset(&(buf), 0, sizeof (struct v4l2_buffer));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = n_buffers;

    VERIFY(-1 != ioctl(fd, VIDIOC_QUERYBUF, &buf));
    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
      mmap(NULL /* start anywhere */,
      buf.length,
      PROT_READ | PROT_WRITE /* required */,
      MAP_SHARED /* recommended */,
      fd, buf.m.offset);

    VERIFY(MAP_FAILED != buffers[n_buffers].start);
  }

}

void V4lCameraHandler::startCapturing()
{
  unsigned int i;
  enum v4l2_buf_type type;

  for (i = 0; i < n_buffers; ++i)
  {
    struct v4l2_buffer buf;

    memset(&(buf), 0, sizeof (struct v4l2_buffer));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    VERIFY(-1 != ioctl(fd, VIDIOC_QBUF, &buf));
  }

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  VERIFY(-1 != ioctl(fd, VIDIOC_STREAMON, &type));

}

int V4lCameraHandler::readFrame()
{

  if (wasQueried)
  {
    VERIFY(-1 != ioctl(fd, VIDIOC_QBUF, &currentBuf));
  }

  // blocking call, waits for a new to image available
  VERIFY(ioctl(fd, VIDIOC_DQBUF, &currentBuf) >= 0);
  wasQueried = true;
  ASSERT(currentBuf.index < n_buffers);

  //process_image(buffers[buf.index].start);

  return currentBuf.index;

}

void V4lCameraHandler::stopCapturing()
{
  enum v4l2_buf_type type;

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  VERIFY(-1 != ioctl(fd, VIDIOC_STREAMOFF, &type));
}

void V4lCameraHandler::uninitDevice()
{
  unsigned int i;

  for (i = 0; i < n_buffers; ++i)
    if (-1 == munmap(buffers[i].start, buffers[i].length))
      return;

  free(buffers);
}

void V4lCameraHandler::closeDevice()
{
  if (-1 == close(fd))
    return;

  fd = -1;
}

void V4lCameraHandler::getCameraSettings(CameraSettings& data)
{
  for (unsigned int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    data.data[i] = currentSettings[currentCamera].data[i];
  }
}

void V4lCameraHandler::setCameraSettings(const CameraSettings& data, bool queryNew)
{
  if (queryNew)
  {
    internalUpdateCameraSettings();
  }
  else
  {
    internalSetCameraSettings(data);
  }
}

int V4lCameraHandler::getSingleCameraParameter(int id)
{
  struct v4l2_queryctrl queryctrl;
  queryctrl.id = id;
  if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
  {
    std::cerr << "VIDIOC_QUERYCTRL failed" << std::endl;
    return -1;
  }
  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
  {
    std::cerr << "not getting camera parameter since it is not available" << std::endl;
    return -1; // not available
  }

  if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
  {
    std::cerr << "not getting camera parameter since it is not supported" << std::endl;
    return -1; // not supported
  }

  struct v4l2_control control_s;
  control_s.id = id;
  if (ioctl(fd, VIDIOC_G_CTRL, &control_s) < 0)
  {
    std::cerr << "VIDIOC_G_CTRL failed" << std::endl;
    return -1;
  }

  return control_s.value;
}

int V4lCameraHandler::getSingleCameraParameterCheckFlip(int id, CameraInfo::CameraID camId)
{
  int value = getSingleCameraParameter(id);
  // flip the meaing of VerticalFlip and HorizontalFlip according to camera selection
  if (id == CameraSettings::VerticalFlip || id == CameraSettings::HorizontalFlip)
  {
    if (value == 0)
    {
      value = (camId == CameraInfo::Top) ?
        1 : 0;
    }
    else if (value == 1)
    {
      value = (camId == CameraInfo::Top) ?
        0 : 1;
    }
  }

  return value;

}

bool V4lCameraHandler::setSingleCameraParameter(int id, int value)
{
  struct v4l2_queryctrl queryctrl;
  queryctrl.id = id;
  if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
  {
    std::cerr << "VIDIOC_QUERYCTRL failed" << std::endl;
    return false;
  }

  if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
  {
    std::cerr << "V4L2_CTRL_FLAG_DISABLED failed" << std::endl;
    return false;
  }
  if (queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
  {
    std::cerr << "V4L2_CTRL_FLAG_DISABLED failed" << std::endl;
    return false; // not supported
  }

  // clip value
  if (value < 0)
    value = queryctrl.default_value;
  if (value < queryctrl.minimum)
    value = queryctrl.minimum;
  if (value > queryctrl.maximum)
    value = queryctrl.maximum;

  struct v4l2_control control_s;
  control_s.id = id;
  control_s.value = value;
  if (ioctl(fd, VIDIOC_S_CTRL, &control_s) < 0)
  {
    std::cerr << "VIDIOC_S_CTRL failed (" << errno << ": " <<
      strerror(errno) << ")" << std::endl;
    return false;
  }

  return true;
}

void V4lCameraHandler::internalSetCameraSettings(const CameraSettings& data)
{

  // set camera settings for "old" camera
  for (int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    if (i == CameraSettings::FPS && currentSettings[currentCamera].data[i] != data.data[i])
    {
      setFPS(data.data[i]);
    }
    else if (csConst[i] > -1 && i != CameraSettings::CameraSelection && data.data[i] != currentSettings[currentCamera].data[i])
    {
      std::cout << "Camera parameter "
        << CameraSettings::getCameraSettingsName((CameraSettings::CameraSettingID) i)
        << " was changed from " << currentSettings[currentCamera].data[i] << " to " << data.data[i]
        << std::endl;

      
      if (setSingleCameraParameterCheckFlip((CameraSettings::CameraSettingID) i, 
        (CameraInfo::CameraID) currentSettings[currentCamera].data[CameraSettings::CameraSelection], data.data[i]))
      {
        currentSettings[currentCamera].data[i] = data.data[i];
      }
      else
      {
        std::cerr << "COULD NOT SET "
          << CameraSettings::getCameraSettingsName((CameraSettings::CameraSettingID) i)
          << " CAMERA PARAMETER" << std::endl;
      }
    }
  }

  
  if (currentSettings[currentCamera].data[CameraSettings::CameraSelection] != data.data[CameraSettings::CameraSelection])
  {
    fastCameraSelection((CameraInfo::CameraID) data.data[CameraSettings::CameraSelection]);
  }


}

bool V4lCameraHandler::setSingleCameraParameterCheckFlip(CameraSettings::CameraSettingID i, CameraInfo::CameraID newCam,int value)
{
  // flip the meaing of VerticalFlip and HorizontalFlip according to camera selection
  if (i == CameraSettings::VerticalFlip || i == CameraSettings::HorizontalFlip)
  {
    if (value == 0)
    {
      value = (newCam == CameraInfo::Top) ?
        1 : 0;
    }
    else if (value == 1)
    {
      value = (newCam == CameraInfo::Top) ?
        0 : 1;
    }
  }

  return setSingleCameraParameter(csConst[i], value);

}

void V4lCameraHandler::internalUpdateCameraSettings()
{
  for (int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    if (csConst[i] > -1)
    {
      int value = getSingleCameraParameterCheckFlip(csConst[i], currentCamera);
      if (value > -1)
      {
        currentSettings[currentCamera].data[i] = value;
      }
    }
  }

}

V4lCameraHandler::~V4lCameraHandler()
{
  stopCapturing();
  uninitDevice();
  closeDevice();
}

