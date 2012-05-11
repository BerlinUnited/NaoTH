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

// define some non-standard constants
#ifndef V4L2_CID_AUTOEXPOSURE
#define V4L2_CID_AUTOEXPOSURE     (V4L2_CID_BASE+32)
#endif
#ifndef V4L2_CID_CAM_INIT
#define V4L2_CID_CAM_INIT         (V4L2_CID_BASE+33)
#endif

using namespace naoth;

V4lCameraHandler::V4lCameraHandler()
:
  selMethodIO(IO_MMAP),
  actMethodIO(Num_of_MethodIO),
  fd(-1), buffers(NULL),
//  selMethodIO(IO_USERPTR),
//  selMethodIO(IO_READ),
  n_buffers(0),
  hadReset(false),
  wasQueried(false),
  isCapturing(false)
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

  // set our IDs
  initIDMapping();

  // open the device
  openDevice(true);//in blocking mode
  initDevice();
  internalUpdateCameraSettings();
  setFPS(30);

  // start capturing
  startCapturing();
;
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
  if(errOccured < 0)
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
  //csConst[CameraSettings::AutoExposition] = V4L2_CID_AUTOEXPOSURE;
  csConst[CameraSettings::AutoWhiteBalancing] = V4L2_CID_AUTO_WHITE_BALANCE;
  //csConst[CameraSettings::AutoGain] = V4L2_CID_AUTOGAIN;
  csConst[CameraSettings::Brightness] = V4L2_CID_BRIGHTNESS;
  csConst[CameraSettings::Contrast] = V4L2_CID_CONTRAST;
  csConst[CameraSettings::Saturation] = V4L2_CID_SATURATION;
  csConst[CameraSettings::Hue] = V4L2_CID_HUE;
  //csConst[CameraSettings::RedChroma] = V4L2_CID_RED_BALANCE;
  //csConst[CameraSettings::BlueChroma] = V4L2_CID_BLUE_BALANCE;
  csConst[CameraSettings::Gain] = V4L2_CID_GAIN;
  csConst[CameraSettings::HorizontalFlip] = V4L2_CID_HFLIP;
  csConst[CameraSettings::VerticalFlip] = V4L2_CID_VFLIP;
  csConst[CameraSettings::Exposure] = V4L2_CID_EXPOSURE;

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

void V4lCameraHandler::openDevice(bool blockingMode)
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
  unsigned int min;

  // set default parameters and init the camera
  /*
  struct v4l2_control control;
  memset(&control, 0, sizeof (struct v4l2_control));
  control.id = V4L2_CID_CAM_INIT;
  control.value = 0;

  int errorOccured = -1;
  std::cout << "Initializing camera ";
  for(int i = 0; i < 20 && errorOccured < 0; i++)
  {
    errorOccured = ioctl(fd, VIDIOC_S_CTRL, &control);
    if(errorOccured < 0 && (errno == EBUSY) )
    {
      usleep(10);
      std::cout << ".";
    }
    else 
    {
      hasIOError(errorOccured, errno);
    }
  }
  std::cout << std::endl;
  hasIOError(errorOccured, errno);
  */

  /*
  v4l2_std_id esid0 = 0x04000000UL; // use 0x08000000UL when the width is 640
  VERIFY(!ioctl(fd, VIDIOC_S_STD, &esid0));
  */

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
    noBufferChangeCount = 0;
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
      noBufferChangeCount = 0;
      if(hadReset)
      {
        hadReset = false;
      }
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

  //not first frame and no other error while getting next buffer
  if(wasQueried && ( (!blockingCaptureModeEnabled && errorOccured < 0) || errorOccured == 0) )
  {
    if(lastBuf.index != currentBuf.index)
    {
      //yes
      bufferSwitched = true;
//      std::cout << "switching buffers" << std::endl;
    }
    else
    {
      //no => count number of failed/overwritten frames
//      std::cout << "didn't switch buffers" << std::endl;
      bufferSwitched = false;
      noBufferChangeCount++;
    }
  }

  //remember current buffer for the next frame as last buffer
  lastBuf = currentBuf;

  wasQueried = true;
  ASSERT(currentBuf.index < n_buffers);
  return currentBuf.index;
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
      noBufferChangeCount = 0;
      if(hadReset)
      {
        hadReset = false;
      }
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


    //not first frame and no other error while getting next buffer
    if(wasQueried && ( (!blockingCaptureModeEnabled && errorOccured < 0) || errorOccured == 0) )
    {
      cout << "current buffer: " << currentBuf.index << " last buffer: " << lastBuf.index << endl;
      if(lastBuf.index != currentBuf.index)
      {
        //yes
        bufferSwitched = true;
        std::cout << "switching buffers" << std::endl;
      }
      else
      {
        //no => count number of failed/overwritten frames
        std::cout << "didn't switch buffers" << std::endl;
        bufferSwitched = false;
        noBufferChangeCount++;
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
    if(hadReset)
    {
      theImage.bufferFailedCount = 0;
      theImage.wrongBufferSizeCount = 0;
    }

    //try to reinitialize the camera device after some failed buffer switchings
    if(noBufferChangeCount % currentSettings[currentCamera].data[CameraSettings::FPS] == 1)
    {
      if(!hadReset)
      {
        theImage.bufferFailedCount++;
      }
    }

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

  int errorOccured = -1;
  int returnValue = -1;

  std::cout << "Getting camera parameter (" << id << ") ";
  for(int i = 0; i < 20 && errorOccured < 0; i++)
  {
    errorOccured = ioctl(fd, VIDIOC_G_CTRL, &control_s);
    if(errorOccured < 0 && (errno == EBUSY))
    {
      usleep(10);
      std::cout << ".";
    }
    else
    {
      hasIOError(errorOccured, errno, false);
    }
  }
  std::cout << std::endl;

  if(!hasIOError(errorOccured, errno, false))
  {
     returnValue = control_s.value;
  }

  return returnValue;
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
  if (int errCode = ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
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

  // clip value
  if (value < 0)
  {
    value = queryctrl.default_value;
  }
  if (value < queryctrl.minimum)
  {
    value = queryctrl.minimum;
  }
  if (value > queryctrl.maximum)
  {
    value = queryctrl.maximum;
  }

  struct v4l2_control control_s;
  control_s.id = id;
  control_s.value = value;

  int errorOccured = -1;
  std::cout << "setting camera parameter (" << id << ") ";
  for(int i = 0; i < 20 && errorOccured < 0; i++)
  {
    errorOccured = ioctl(fd, VIDIOC_S_CTRL, &control_s);
    if(errorOccured < 0 && (errno == EBUSY))
    {
      usleep(10);
      std::cout << ".";
    }
    else
    {
      std::cout << std::endl;
      return !hasIOError(errorOccured, errno, false);
    }
  }
  std::cout << std::endl;
  return !hasIOError(errorOccured, errno, false);
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
    if (csConst[i] > -1 && i != CameraSettings::CameraSelection && data.data[i] != currentSettings[currentCamera].data[i])
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

void V4lCameraHandler::shutdown()
{
  stopCapturing();
  uninitDevice();
  closeDevice();

  close(fdAdapter);
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
