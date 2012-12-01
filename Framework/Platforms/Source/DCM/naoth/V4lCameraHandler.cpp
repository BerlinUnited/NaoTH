/*
 * File:   V4lCameraHandler.cpp
 * Author: thomas
 *
 * Created on 22. April 2010, 17:24
 */

#include "V4lCameraHandler.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Debug/Stopwatch.h"
#include "Representations/Infrastructure/Image.h"
#include "PlatformInterface/Platform.h"

#include <string.h>
#include <iostream>

extern "C"
{
  #include "i2c-small.h"
  #include "Tools/NaoTime.h"
}

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
  wasQueried(false),
  isCapturing(false)
{

  for (int i = 0; i < CameraSettings::numOfCameraSetting; i++)
  {
    allowedTolerance[i] = -1;
  }

  allowedTolerance[CameraSettings::Brightness] = 5;
  allowedTolerance[CameraSettings::WhiteBalance] = 1;

  settingsOrder.push_back(CameraSettings::Brightness);
  settingsOrder.push_back(CameraSettings::BacklightCompensation);
  settingsOrder.push_back(CameraSettings::AutoExposition);
  settingsOrder.push_back(CameraSettings::AutoWhiteBalancing);
  //settingsOrder.push_back(CameraSettings::Brightness);
  settingsOrder.push_back(CameraSettings::Contrast);
  settingsOrder.push_back(CameraSettings::Saturation);
  settingsOrder.push_back(CameraSettings::Exposure);
  settingsOrder.push_back(CameraSettings::Gain);
  settingsOrder.push_back(CameraSettings::Sharpness);
  settingsOrder.push_back(CameraSettings::WhiteBalance);

  // set our IDs
  initIDMapping();
}

void V4lCameraHandler::init(const CameraSettings camSettings,
                            std::string camDevice, CameraInfo::CameraID camID)
{

  if(isCapturing)
  {
    shutdown();
  }

  currentCamera = camID;

  for(int j=0; j < CameraSettings::numOfCameraSetting; j++)
  {
    currentSettings.data[j] = camSettings.data[j];
  }

  cameraName = camDevice;

  // open the device
  openDevice(true);//in blocking mode
  setAllCameraParams(camSettings);
  setFPS(30);
  initDevice();


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
  csConst[CameraSettings::AutoExposition] = V4L2_CID_EXPOSURE_AUTO;
  csConst[CameraSettings::AutoWhiteBalancing] = V4L2_CID_AUTO_WHITE_BALANCE;
  csConst[CameraSettings::Brightness] = V4L2_CID_BRIGHTNESS;
  csConst[CameraSettings::Contrast] = V4L2_CID_CONTRAST;
  csConst[CameraSettings::Saturation] = V4L2_CID_SATURATION;
  csConst[CameraSettings::Hue] = V4L2_CID_HUE;
  csConst[CameraSettings::Gain] = V4L2_CID_GAIN;
  csConst[CameraSettings::HorizontalFlip] = V4L2_CID_HFLIP;
  csConst[CameraSettings::VerticalFlip] = V4L2_CID_VFLIP;
  csConst[CameraSettings::Exposure] = V4L2_CID_EXPOSURE;
  csConst[CameraSettings::BacklightCompensation] = V4L2_CID_BACKLIGHT_COMPENSATION;
  csConst[CameraSettings::WhiteBalance] = V4L2_CID_DO_WHITE_BALANCE;
  csConst[CameraSettings::Sharpness] = V4L2_CID_SHARPNESS;

//---------------------------------------------------------------------
// copied from the driver for information:
//---------------------------------------------------------------------
//  /* Fill in min, max, step and default value for these controls. */
//  switch (qc->id) {
//  case V4L2_CID_BRIGHTNESS:
//    return v4l2_ctrl_query_fill(qc, 0, 255, 1, 55);
//  case V4L2_CID_CONTRAST:
//    return v4l2_ctrl_query_fill(qc, 0, 127, 1, 32);
//  case V4L2_CID_SATURATION:
//    return v4l2_ctrl_query_fill(qc, 0, 255, 1, 128);
//  case V4L2_CID_HUE:
//    return v4l2_ctrl_query_fill(qc, -180, 180, 1, 0);
//  case V4L2_CID_VFLIP:
//  case V4L2_CID_HFLIP:
//    return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
//  case V4L2_CID_SHARPNESS:
//    return v4l2_ctrl_query_fill(qc, -7, 7, 1, 0);
//  case V4L2_CID_EXPOSURE_AUTO:
//    return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
//  case V4L2_CID_AUTO_WHITE_BALANCE:
//    return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
//  case V4L2_CID_GAIN:
//    return v4l2_ctrl_query_fill(qc, 0, 255, 1, 32);
//  case V4L2_CID_EXPOSURE:
//    return v4l2_ctrl_query_fill(qc, 0, 512, 1, 0);
//  case V4L2_CID_DO_WHITE_BALANCE:
//    return v4l2_ctrl_query_fill(qc, -180, 180, 1, -166);
//  case V4L2_CID_BACKLIGHT_COMPENSATION:
//    return v4l2_ctrl_query_fill(qc, 0, 4, 1, 1);
//  }
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
  fmt.fmt.pix.width = 320;
  fmt.fmt.pix.height = 240;
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
      VERIFY(-1 != xioctl(fd, VIDIOC_QBUF, &lastBuf));
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
      std::cerr << "[V4L get] Could not get image, error code " << resultCode << "/" << errno << std::endl;
    }
    else
    {
      theImage.setCameraInfo(Platform::getInstance().theCameraInfo);
      //if(currentBuf.bytesused != theImage.cameraInfo.size * SIZE_OF_YUV422_PIXEL)
      if(currentBuf.bytesused < theImage.cameraInfo.size * SIZE_OF_YUV422_PIXEL)
      {
        theImage.wrongBufferSizeCount++;
//        cout << "wrong image buffer size: " << (theImage.cameraInfo.size * SIZE_OF_YUV422_PIXEL) << ", buffer: " << currentBuf.bytesused << "/" << currentBuf.length << endl;
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

//  // HACK
//  if(id == V4L2_CID_DO_WHITE_BALANCE)
//  {
//    //min = -120;
//    //max = -36;
//    //min = -1;
//    //max = 1;
//  }

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

  if(id == CameraSettings::Exposure)
  {
    value = (value << 2) >> 2;
  }

  if(id == CameraSettings::WhiteBalance)
  {
    value = ((value + 180) * 45) / 45 - 180;
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

  // HACK
    setSingleCameraParameter(V4L2_CID_EXPOSURE_AUTO,1);
//    setSingleCameraParameter(V4L2_CID_EXPOSURE_AUTO,0);
//    usleep(1000);

//    setSingleCameraParameter(V4L2_CID_BRIGHTNESS,128);
//    //setSingleCameraParameter(V4L2_CID_EXPOSURE_AUTO,0);
//    usleep(1000);

  for(std::list<CameraSettings::CameraSettingID>::const_iterator it=settingsOrder.begin();
      it != settingsOrder.end(); it++)
  {
    if(csConst[*it] != -1)
    {
      bool success = false;
      int realValue = data.data[*it];

      std::cout << "setting "
        << CameraSettings::getCameraSettingsName(*it) << " to " << data.data[*it]
        << ": ";

      int errorNumber = 0;
      while(!success && errorNumber < 100)
      {
        int clippedValue = setSingleCameraParameter(csConst[*it], data.data[*it]);
        usleep(1000);
        if(clippedValue != data.data[*it])
        {
          std::cout << "(clipped " << clippedValue << ")";
        }

        if(allowedTolerance[*it] == -1)
        {
          success = true;
        }
        else
        {
          realValue = getSingleCameraParameter(csConst[*it]);
          if(csConst[*it] == CameraSettings::Exposure)
          {
            clippedValue = (clippedValue << 2) >> 2;
          }

          if(csConst[*it] == CameraSettings::WhiteBalance)
          {
            clippedValue = ((clippedValue + 180) * 45) / 45 - 180;
          }

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
    } // end if csConst was set
  } // end for each camera setting (in order

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
