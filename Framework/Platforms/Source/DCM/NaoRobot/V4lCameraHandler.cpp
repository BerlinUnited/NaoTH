/*
 * File:   V4lCameraHandler.cpp
 * Author: thomas
 *
 * Created on 22. April 2010, 17:24
 */

#include "V4lCameraHandler.h"

#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/NaoTime.h"

#include "Representations/Infrastructure/Image.h"

#include <string.h>
#include <iostream>

//Custom V4L control variables
#define V4L2_MT9M114_FADE_TO_BLACK (V4L2_CID_PRIVATE_BASE) //boolean, enable or disable fade to black feature

using namespace naoth;
using namespace std;


V4lCameraHandler::V4lCameraHandler()
  :
  selMethodIO(IO_MMAP),
  actMethodIO(Num_of_MethodIO),
  fd(-1), buffers(NULL),
//  selMethodIO(IO_USERPTR),
//  selMethodIO(IO_READ),
  n_buffers(0),
  atLeastOneImageRetrieved(false),
  wasQueried(false),
  isCapturing(false)
{

  for (int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    allowedTolerance[i] = -1;
  }

//  allowedTolerance[CameraSettings::Brightness] = 5;
//  allowedTolerance[CameraSettings::WhiteBalance] = 1;

  for (int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    waitTime[i] = -1;
  }

  waitTime[CameraSettings::AutoWhiteBalancing] = 3000000;

  settingsOrder.push_back(CameraSettings::AutoExposition);
  settingsOrder.push_back(CameraSettings::AutoWhiteBalancing);
  settingsOrder.push_back(CameraSettings::Brightness);
  settingsOrder.push_back(CameraSettings::Contrast);
  settingsOrder.push_back(CameraSettings::Saturation);
  settingsOrder.push_back(CameraSettings::Hue);
  settingsOrder.push_back(CameraSettings::VerticalFlip);
  settingsOrder.push_back(CameraSettings::HorizontalFlip);
  settingsOrder.push_back(CameraSettings::Sharpness);
  settingsOrder.push_back(CameraSettings::Gain);
  settingsOrder.push_back(CameraSettings::WhiteBalance);
  settingsOrder.push_back(CameraSettings::BacklightCompensation);
  settingsOrder.push_back(CameraSettings::FadeToBlack);
  settingsOrder.push_back(CameraSettings::Exposure);

  // set our IDs
  initIDMapping();
}

void V4lCameraHandler::init(std::string camDevice, CameraInfo::CameraID camID,
                            bool blockingMode)
{

  if(isCapturing)
  {
    shutdown();
  }

  currentCamera = camID;
  cameraName = camDevice;

  // open the device
  openDevice(blockingMode);//in blocking mode
  setFPS(30);
  initDevice();

//  setFlipParameters(camID == CameraInfo::Top);
//  internalUpdateCameraSettings();

  for(int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    currentSettings.data[i] = -1;
  }

  // start capturing
  startCapturing();

}

int V4lCameraHandler::xioctl(int fd, int request, void* arg)
{
  int r;
  do
  {
    r = ioctl (fd, request, arg);
  }
  while (-1 == r && EINTR == errno);
  return r;
}

bool V4lCameraHandler::hasIOError(int errOccured, int errNo, bool exitByIOError = true)
{
  if(errOccured < 0 && errNo != EAGAIN)
  {
    std::cout << " failed with errno " << errNo << " (" << getErrnoDescription(errNo) << ") >> exiting" << std::endl;
    if(exitByIOError)
    {
      assert(errOccured >= 0);
    }
    return true;
  }
  return false;
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
  csConst[CameraSettings::Brightness] = V4L2_CID_BRIGHTNESS;
  csConst[CameraSettings::Contrast] = V4L2_CID_CONTRAST;
  csConst[CameraSettings::Saturation] = V4L2_CID_SATURATION;
  csConst[CameraSettings::Hue] = V4L2_CID_HUE;
  csConst[CameraSettings::VerticalFlip] = V4L2_CID_VFLIP;
  csConst[CameraSettings::HorizontalFlip] = V4L2_CID_HFLIP;
  csConst[CameraSettings::Sharpness] = V4L2_CID_SHARPNESS;
  csConst[CameraSettings::AutoExposition] = V4L2_CID_EXPOSURE_AUTO;
  csConst[CameraSettings::AutoWhiteBalancing] = V4L2_CID_AUTO_WHITE_BALANCE;
  csConst[CameraSettings::Gain] = V4L2_CID_GAIN;
  csConst[CameraSettings::Exposure] = V4L2_CID_EXPOSURE;
  csConst[CameraSettings::WhiteBalance] = V4L2_CID_DO_WHITE_BALANCE;
  csConst[CameraSettings::BacklightCompensation] = V4L2_CID_BACKLIGHT_COMPENSATION;
  csConst[CameraSettings::FadeToBlack] = V4L2_MT9M114_FADE_TO_BLACK;

//---------------------------------------------------------------------
// copied from the driver for information:
//---------------------------------------------------------------------
/*
V4L2_CID_BRIGHTNESS:
Range: [0 .. 255]
Default: 55
Description: Set brightness in auto exposure mode.

V4L2_CID_CONTRAST:
Range: [16 .. 64] (Fixed point number: 16 = 0.5, 64 = 2.0)
Default: 32
Description: Controls contrast enhancement and noise reduction values.

V4L2_CID_SATURATION:
Range: [0 .. 255]
Default: 128
Description: Zero means gray-scale, values > 128 result in a boosted saturation.

V4L2_CID_HUE:
Range: [-22° .. 22°]
Default: 0
Description: Hue rotation. Not all values are possible depending on camera internals. The camera chip will clip to the nearest possible number.

V4L2_CID_VFLIP:
Range: Boolean
Default: 0
Description: Vertical flip

V4L2_CID_HFLIP:
Range: Boolean
Default: 0
Description: Horizontal flip

V4L2_CID_SHARPNESS:
Range: [0 .. 7] and -7
Default: 0
Description: Relative adjustment to the applied sharpness.
             Set to -7 to ensure that no sharpness is applied.

V4L2_CID_EXPOSURE_AUTO:
Range: Boolean
Default: 1
Description: Auto exposure feature. Automatically controls gain
             and exposure.

V4L2_CID_AUTO_WHITE_BALANCE:
Range: Boolean
Default: 1
Description: Auto white balance feature.

V4L2_CID_GAIN:
Range: [0 .. 255]
Default: 32
Description: The amount of gain that is applied if auto exposure
             is disabled. 32 = 1x gain.

V4L2_CID_EXPOSURE:
Range: [0 .. 512]
Default: 0
Description: The absolute exposure time if auto exposure is disabled.

V4L2_CID_DO_WHITE_BALANCE:
Range: [2700 .. 6500] ° Kelvin
Default: 6500
Description: White balance color temperature. Ignored if auto white balance
             is enabled.

V4L2_CID_BACKLIGHT_COMPENSATION:
Range: [0 .. 4]
Default: 1
Description: -

V4L2_MT9M114_FADE_TO_BLACK (V4L2_CID_PRIVATE_BASE):
Range: Boolean
Default: 1
Description: Enable/disable fade-to-black feature.
 */
//---------------------------------------------------------------------


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

  currentSettings.data[CameraSettings::FPS] = fpsRate;
}

void V4lCameraHandler::openDevice(bool blockingMode)
{

  struct stat st;
  memset (&st, 0, sizeof (st));

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

  std::cout << "Opening camera device '" << cameraName << "' ";
  blockingCaptureModeEnabled = blockingMode;
  if(blockingMode)
  {
    fd = open(cameraName.c_str(), O_RDWR, 0);
    std::cout << "(blocking mode)";
  }
  else
  {
    fd = open(cameraName.c_str(), O_RDWR | O_NONBLOCK, 0);
    std::cout << "(non blocking mode)";
  }
  std::cout << endl;

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
  memset (&cap, 0, sizeof (cap));
  unsigned int min;

  memset(&(currentBuf), 0, sizeof (struct v4l2_buffer));
  currentBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  currentBuf.memory = V4L2_MEMORY_MMAP;

  VERIFY(ioctl(fd, VIDIOC_QUERYCAP, &cap) != -1);

  VERIFY(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE);

  VERIFY(selMethodIO == IO_READ || selMethodIO == IO_MMAP || selMethodIO == IO_USERPTR);

  switch(selMethodIO)
  {
    case IO_READ:
      VERIFY(cap.capabilities & V4L2_CAP_READWRITE);
      break;

    case IO_MMAP:
    case IO_USERPTR:
      VERIFY(cap.capabilities & V4L2_CAP_STREAMING);
      break;
    default: ASSERT(false);
  }

  /* Select video input, video standard and tune here. */

  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof (struct v4l2_format));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = naoth::IMAGE_WIDTH;
  fmt.fmt.pix.height = naoth::IMAGE_HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
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

  switch (selMethodIO)
  {
    case IO_READ:
      initRead(fmt.fmt.pix.sizeimage);
      break;

    case IO_MMAP:
      initMMap();
      break;

    case IO_USERPTR:
      initUP(fmt.fmt.pix.sizeimage);
      break;
    default: ASSERT(false);
  }
  actMethodIO = selMethodIO;
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

void V4lCameraHandler::initUP(unsigned int buffer_size)
{
  struct v4l2_requestbuffers req;
  unsigned int page_size = getpagesize();

  buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);
  memset(&(req), 0, sizeof (v4l2_requestbuffers));

  req.count = 5; // number of internal buffers, since we use debug images that should be quite big
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_USERPTR;

  VERIFY(-1 != ioctl(fd, VIDIOC_REQBUFS, &req));

  VERIFY(req.count >= 2);

  buffers = (struct buffer*) calloc(req.count, sizeof (*buffers));

  VERIFY(buffers);

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
  {
    buffers[n_buffers].length = buffer_size;
    buffers[n_buffers].start = memalign (page_size, buffer_size);
    cout << n_buffers << " buffer_size " << buffers[n_buffers].length << endl;
    cout << n_buffers << " page_size " << buffers[n_buffers].start << endl;

    VERIFY(NULL != buffers[n_buffers].start);
  }
  cout << " page_size " << page_size << endl;

}

void V4lCameraHandler::initRead(unsigned int		buffer_size)
{
  buffers =  (struct buffer*) calloc (1, sizeof (*buffers));

  VERIFY(NULL != buffers);

    buffers[0].length = buffer_size;
    buffers[0].start = malloc (buffer_size);

    VERIFY(NULL != buffers[0].start);
}

void V4lCameraHandler::startCapturing()
{
  if(actMethodIO != Num_of_MethodIO)
  {
    if(actMethodIO != IO_READ)
    {
      unsigned int i;
      enum v4l2_buf_type type;

      for (i = 0; i < n_buffers; ++i)
      {
        struct v4l2_buffer buf;

        memset(&(buf), 0, sizeof (struct v4l2_buffer));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.index = i;
        if(actMethodIO == IO_MMAP)
        {
          buf.memory = V4L2_MEMORY_MMAP;
        }
        else
        {
          buf.memory = V4L2_MEMORY_USERPTR;
          buf.m.userptr	= (unsigned long) buffers[i].start;
          buf.length = buffers[i].length;
        }

        VERIFY(-1 != xioctl(fd, VIDIOC_QBUF, &buf));
      }

      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      VERIFY(-1 != ioctl(fd, VIDIOC_STREAMON, &type));
    }

    isCapturing = true;
    wasQueried = false;
    bufferSwitched = true;
    lastBuf = currentBuf;
    atLeastOneImageRetrieved = false;
  }
}

int V4lCameraHandler::readFrameMMaP()
{
  //not the first frame and the buffer changed last frame
  if (wasQueried)
  {
    //no => did the buffer index change?
    if(bufferSwitched)
    {
      //put buffer back in the drivers incoming queue
      if(blockingCaptureModeEnabled)
      {
        VERIFY(-1 != xioctl(fd, VIDIOC_QBUF, &lastBuf));
      }
      else
      {
        xioctl(fd, VIDIOC_QBUF, &lastBuf);
      }
      //std::cout << "give buffer to driver" << std::endl;
    }
   }

  struct v4l2_buffer buf;
  memset(&(buf), 0, sizeof (struct v4l2_buffer));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  int errorOccured = -1;
  if(blockingCaptureModeEnabled)
  {
    //in blocking mode just get a buffer from the drivers outgoing queue
    errorOccured = xioctl(fd, VIDIOC_DQBUF, &buf);
    hasIOError(errorOccured, errno);
//    std::cout << "get buffer from driver blocking" << std::endl;
  }
  else
  {
    //in non-blocking mode make some tries if no buffer in the  drivers outgoing queue is ready
    //but limit the number of tries (better to loose a frame then to stuck in an endless loop)
    for(int i = 0; i < 20 && errorOccured < 0; i++)
    {
      errorOccured = xioctl(fd, VIDIOC_DQBUF, &buf);
//      std::cout << "get buffer from driver nonblocking" << std::endl;
      if(errorOccured < 0 && errno == EAGAIN)
      {
        usleep(100);
      }
      else
      {
        hasIOError(errorOccured, errno);
      }
    }
  }
  currentBuf = buf;

  //remember current buffer for the next frame as last buffer
  lastBuf = currentBuf;

  wasQueried = true;
  ASSERT(currentBuf.index < n_buffers);
  if(errorOccured == 0)
    return currentBuf.index;
  else
    return -1;
}

int V4lCameraHandler::readFrameUP()
{
  cout << "#";
  //not the first frame and the buffer changed last frame
  if (wasQueried)
  {
    //no => did the buffer index change?
    if(bufferSwitched)
    {
      //put buffer back in the drivers incoming queue
      VERIFY(-1 != xioctl(fd, VIDIOC_QBUF, &lastBuf));
      //std::cout << "give buffer to driver" << std::endl;
      cout << "queued buffer: " << lastBuf.index << ", l = " << lastBuf.length << endl;
    }
   }

  struct v4l2_buffer buf;
  memset(&(buf), 0, sizeof (struct v4l2_buffer));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_USERPTR;

  int errorOccured = -1;
  if(blockingCaptureModeEnabled)
  {
    //in blocking mode just get a buffer from the drivers outgoing queue
    errorOccured = xioctl(fd, VIDIOC_DQBUF, &buf);
    cout << "after dequeue:" << buf.index << ", l = " << buf.length << endl;
    hasIOError(errorOccured, errno);
    //    std::cout << "get buffer from driver blocking" << std::endl;
  }
  else
  {
    //in non-blocking mode make some tries if no buffer in the  drivers outgoing queue is ready
    //but limit the number of tries (better to loose a frame then to stuck in an endless loop)
    for(int i = 0; i < 20 && errorOccured < 0; i++)
    {
      errorOccured = ioctl(fd, VIDIOC_DQBUF, &buf);
      //      std::cout << "get buffer from driver nonblocking" << std::endl;
      if(errorOccured < 0 && errno == EAGAIN)
      {
        usleep(100);
      }
      else
      {
        hasIOError(errorOccured, errno);
      }
    }
  }

    if(errorOccured < 0 && errno != EAGAIN && errno != EIO)
    {
      hasIOError(errorOccured, errno, true);
      cout << "|";
    }
    else
    {
      for (unsigned int i = 0; i < n_buffers; ++i)
      {
          cout << i << "usrptr = " << buf.m.userptr << ", bufptr = " << buffers[i].start << ", l = "<< buf.length << " / "<< buffers[i].length << endl;
        if (buf.m.userptr == (unsigned long) buffers[i].start && buf.length == buffers[i].length)
        {
          currentBuf = buf;
          currentBuf.index = i;
          currentBuf.bytesused = buf.length;
          cout << "frame in buffer: " << i << ", l = " << buf.length << endl;
        cout << "current buffer: " << currentBuf.index << " last buffer: " << lastBuf.index << endl;
        }
      }
    }

  //remember current buffer for the next frame as last buffer
  lastBuf = currentBuf;

  wasQueried = true;
  ASSERT(currentBuf.index < n_buffers);
  return currentBuf.index;
}

int V4lCameraHandler::readFrameRead()
{
  if (-1 == read (fd, buffers[0].start, buffers[0].length))
  {
    switch (errno)
    {
    case EAGAIN:
      return 0;

        case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

    default:
      exit(-1);
      }
    }
  currentBuf.bytesused = buffers[0].length;
  return 0;
}


int V4lCameraHandler::readFrame()
{
  switch (actMethodIO)
  {
    case IO_READ:
      return readFrameRead();

    case IO_MMAP:
      return readFrameMMaP();

    case IO_USERPTR:
      return readFrameUP();
    default: ASSERT(false);
  }
  return -1;
}

void V4lCameraHandler::get(Image& theImage)
{
  if(isCapturing)
  {

    //STOPWATCH_START("ImageRetrieve");
    int resultCode = readFrame();
    //STOPWATCH_STOP("ImageRetrieve");

    if (resultCode < 0)
    {
      std::cerr << "[V4L get!!!] Could not get image, error code " << resultCode
                << "/" << errno << " (" << strerror(errno) << ")" << std::endl;
    }
    else
    {
      //if(currentBuf.bytesused != theImage.cameraInfo.size * Image::PIXEL_SIZE_YUV422)
      if(currentBuf.bytesused < theImage.cameraInfo.getSize() * Image::PIXEL_SIZE_YUV422)
      {
        theImage.wrongBufferSizeCount++;
//        cout << "wrong image buffer size: " << (theImage.cameraInfo.size * Image::PIXEL_SIZE_YUV422) << ", buffer: " << currentBuf.bytesused << "/" << currentBuf.length << endl;
      }
      else
      {
        if(actMethodIO == IO_USERPTR)
        {
          theImage.wrapImageDataYUV422((unsigned char*) currentBuf.m.userptr, currentBuf.bytesused);
        }
        else
        {
          theImage.wrapImageDataYUV422((unsigned char*) buffers[currentBuf.index].start, currentBuf.bytesused);
        }
        theImage.cameraInfo.cameraID = currentCamera;
        theImage.currentBuffer = currentBuf.index;
        theImage.bufferCount = n_buffers;
        theImage.timestamp =
          (unsigned int) ( (((unsigned long long)currentBuf.timestamp.tv_sec) * NaoTime::long_thousand +
                            ((unsigned long long)currentBuf.timestamp.tv_usec) / NaoTime::long_thousand) -
                          NaoTime::startingTimeInMilliSeconds);

        atLeastOneImageRetrieved = true;
      }
    }
  }
}

void V4lCameraHandler::stopCapturing()
{
  if(actMethodIO != Num_of_MethodIO)
  {
    if(actMethodIO != IO_READ)
    {
      enum v4l2_buf_type type;

      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      VERIFY(-1 != ioctl(fd, VIDIOC_STREAMOFF, &type));
    }
  }
  isCapturing = false;
}

void V4lCameraHandler::uninitDevice()
{
  unsigned int i;

  switch (actMethodIO)
  {
    case IO_READ:
      free (buffers[0].start);
      break;

    case IO_MMAP:
      for (i = 0; i < n_buffers; ++i)
      {
        VERIFY(-1 != munmap(buffers[i].start, buffers[i].length));
      }
      break;

    case IO_USERPTR:
      for (i = 0; i < n_buffers; ++i)
      {
        free (buffers[i].start);
      }
      break;

        default: ASSERT(false);
    }
    free (buffers);
}

void V4lCameraHandler::closeDevice()
{
  if (-1 == close(fd))
  {
    return;
  }
  fd = -1;
}

void V4lCameraHandler::getCameraSettings(CameraSettings& data, bool update)
{
  if(update)
  {
    std::cout << "V4L camera settings are updated" << std::endl;
    internalUpdateCameraSettings();
  }
  for (unsigned int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    data.data[i] = currentSettings.data[i];
  }
}

int V4lCameraHandler::getSingleCameraParameter(int id)
{
  struct v4l2_queryctrl queryctrl;
  memset (&queryctrl, 0, sizeof (queryctrl));
  queryctrl.id = id;
  if (int errCode = ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
  {
    std::cerr << "VIDIOC_QUERYCTRL failed: "
              << getErrnoDescription(errCode) << std::endl;
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
  memset (&control_s, 0, sizeof (control_s));
  control_s.id = id;

  int errorOccured = -1;
  int returnValue = -1;

  for(int i = 0; i < 20 && errorOccured < 0; i++)
  {
    errorOccured = ioctl(fd, VIDIOC_G_CTRL, &control_s);
    if(errorOccured < 0)
    {
      switch (errno)
      {
        case EAGAIN:
          usleep(10);
          break;

        case EBUSY:
          usleep(100000);
          break;

        default:
          hasIOError(errorOccured, errno, false);
      }
    }
  }
  if(!hasIOError(errorOccured, errno, false))
  {
     returnValue = control_s.value;
  }
  return returnValue;
}

int V4lCameraHandler::setSingleCameraParameter(int id, int value)
{
  struct v4l2_queryctrl queryctrl;
  memset (&queryctrl, 0, sizeof (queryctrl));
  queryctrl.id = id;
  if (int errCode = xioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
  {
    std::cerr << "VIDIOC_QUERYCTRL failed with code " << errCode << " "
                 << getErrnoDescription(errCode) << std::endl;
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

  int min = queryctrl.minimum;
  int max = queryctrl.maximum;

  // clip value
  if (value == -1)
  {
    value = queryctrl.default_value;
  }
  if (value < min)
  {
    value = min;
  }
  if (value > max)
  {
    value = max;
  }

  struct v4l2_control control_s;
  memset (&control_s, 0, sizeof (control_s));
  control_s.id = id;
  control_s.value = value;

  int errorOccured = -1;
  errorOccured = xioctl(fd, VIDIOC_S_CTRL, &control_s);
  hasIOError(errorOccured, errno, false);

  // return the clipped value
  return value;
}

void V4lCameraHandler::setAllCameraParams(const CameraSettings& data)
{
  if(!atLeastOneImageRetrieved)
  {
    // do nothing if no image was retrieved yet
    std::cerr << "CAN NOT SET PARAMETER YET" << std::endl;
    return;
  }

  for(std::list<CameraSettings::CameraSettingID>::const_iterator it=settingsOrder.begin();
      it != settingsOrder.end(); it++)
  {
    if(csConst[*it] != -1)
    {
      // only set if it was changed
      if(data.data[*it] != currentSettings.data[*it])
      {
        bool success = false;
        int realValue = data.data[*it];

        std::cout << "setting "
          << CameraSettings::getCameraSettingsName(*it) << " to " << data.data[*it]
          << ": ";

        int errorNumber = 0;
        while(!success && errorNumber < 100)
        {
          if(*it == CameraSettings::Exposure)
          {
            // HACK: set first two other values
            setSingleCameraParameter(csConst[*it], data.data[*it]-1);
            setSingleCameraParameter(csConst[*it], data.data[*it]+1);
          }
          int clippedValue = setSingleCameraParameter(csConst[*it], data.data[*it]);
          usleep(1000);
          if(clippedValue != data.data[*it])
          {
            std::cout << "(clipped " << clippedValue << ")";
          }

          if(waitTime[*it] > 0)
          {
            usleep(waitTime[*it]);
          }

          if(allowedTolerance[*it] == -1)
          {
            success = true;
          }
          else
          {
            realValue = getSingleCameraParameter(csConst[*it]);
            success = abs(realValue - clippedValue) <= allowedTolerance[*it];

            if(!success)
            {
              errorNumber++;
              std::cout << "." << std::flush;
              usleep(1000);
            }
          }


        } // end while not successfull

        if(success)
        {
          std::cout << std::endl;
        }
        else
        {
          std::cout << "XXX hang up at " << realValue << std::endl;
        }
        usleep(1000);
      } // end if not the same as current value
    } // end if csConst was set
  } // end for each camera setting (in order)

  // Assume that every update succeeded.
  // If we would only store the successfull ones we would set the
  // camera parameters for ever.
  // The single parameter setting will try several times, thus we
  // already done our best to really set the data.
  currentSettings = data;

}

void V4lCameraHandler::internalUpdateCameraSettings()
{

  for (int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    if (csConst[i] > -1)
    {
      int value = getSingleCameraParameter(csConst[i]);
      currentSettings.data[i] = value;
    }
  }

}

void V4lCameraHandler::shutdown()
{
  stopCapturing();
  uninitDevice();
  closeDevice();
}

bool V4lCameraHandler::isRunning()
{
  return isCapturing;
}

V4lCameraHandler::~V4lCameraHandler()
{
  shutdown();
}

string V4lCameraHandler::getErrnoDescription(int err)
{
  switch (err)
  {
    case	EPERM		: return "Operation not permitted";
    case	ENOENT	: return "No such file or directory";
    case	ESRCH		: return "No such process";
    case	EINTR		: return "Interrupted system call";
    case	EIO		 	: return "I/O error";
    case	ENXIO		: return "No such device or address";
    case	E2BIG		: return "Argument list too long";
    case	ENOEXEC	: return "Exec format error";
    case	EBADF		: return "Bad file number";
    case	ECHILD	: return "No child processes";
    case	EAGAIN	: return "Try again";
    case	ENOMEM	: return "Out of memory";
    case	EACCES	: return "Permission denied";
    case	EFAULT	: return "Bad address";
    case	ENOTBLK	: return "Block device required";
    case	EBUSY		: return "Device or resource busy";
    case	EEXIST	: return "File exists";
    case	EXDEV		: return "Cross-device link";
    case	ENODEV	: return "No such device";
    case	ENOTDIR	: return "Not a directory";
    case	EISDIR	: return "Is a directory";
    case	EINVAL	: return "Invalid argument";
    case	ENFILE	: return "File table overflow";
    case	EMFILE	: return "Too many open files";
    case	ENOTTY	: return "Not a typewriter";
    case	ETXTBSY	: return "Text file busy";
    case	EFBIG		: return "File too large";
    case	ENOSPC	: return "No space left on device";
    case	ESPIPE	: return "Illegal seek";
    case	EROFS		: return "Read-only file system";
    case	EMLINK	: return "Too many links";
    case	EPIPE		: return "Broken pipe";
    case	EDOM		: return "Math argument out of domain of func";
    case	ERANGE	: return "Math result not representable";
  }
  return "Unknown errorcode";
}
