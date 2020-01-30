/*
 * File:   V4lCameraHandler.cpp
 * Author: thomas
 *
 * Created on 22. April 2010, 17:24
 */

#include "V4lCameraHandler.h"

#include "Tools/Debug/NaoTHAssert.h"
//#include "Tools/Debug/Stopwatch.h"
#include "Tools/NaoTime.h"

#include "Representations/Infrastructure/Image.h"

#include "CameraSettingsV5Manager.h"
#include "CameraSettingsV6Manager.h"

#include <cstring>
#include <iostream>
#include <cerrno>

#include <poll.h>

/*
// NOTE: for future improvements of the error handling take a look at 
   https://linux.die.net/man/3/explain_ioctl
  
  #include <libexplain/ioctl.h>
  
  if (ioctl(fildes, request, data) < 0)
  {
      int err = errno;
      fprintf(stderr, "%s\n",
          explain_errno_ioctl(err, fildes, request, data));
      exit(EXIT_FAILURE);
  }
*/

#define LOG "[CameraHandler:" << __LINE__ << ", Camera: " << cameraName << "] "
#define hasIOError(...) hasIOErrorPrint(__LINE__, __VA_ARGS__)

using namespace naoth;
using namespace std;

V4lCameraHandler::V4lCameraHandler()
    : cameraName("none"),
      currentCamera(CameraInfo::numOfCamera),
      fd(-1),
      framesSinceStart(0),
      initialParamsSet(false),
      wasQueried(false),
      isCapturing(false),
      error_count(0)
{

  // DEBUG: test
  //hasIOError(-1, EPIPE);
}

V4lCameraHandler::~V4lCameraHandler()
{
  std::cout << "[V4lCameraHandler] stop wait" << std::endl;
  shutdown();
  std::cout << "[V4lCameraHandler] stop done" << std::endl;
}

void V4lCameraHandler::init(std::string camDevice, CameraInfo::CameraID camID, bool isV6)
{
  // shut down the camera if it was running before doing anything else
  if (isCapturing) {
    shutdown();
  }
  
  if (isV6) {
    settingsManager = std::make_shared<CameraSettingsV6Manager>();
  } else {
    settingsManager = std::make_shared<CameraSettingsV5Manager>();
  }

  currentCamera = camID;
  cameraName = camDevice;

  // open the device
  openDevice();
  initDevice();
  setFPS(30);

  settingsManager->enumerate_controls(fd);
  
  startCapturing();
}

void V4lCameraHandler::setFPS(int fpsRate)
{
  struct v4l2_streamparm fps;
  memset(&fps, 0, sizeof(fps));
  fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(!ioctl(fd, VIDIOC_G_PARM, &fps));
  
  fps.parm.capture.timeperframe.numerator = 1;
  fps.parm.capture.timeperframe.denominator = fpsRate;
  VERIFY(ioctl(fd, VIDIOC_S_PARM, &fps) != -1);
}

void V4lCameraHandler::openDevice()
{
  struct stat st;
  memset(&st, 0, sizeof(st));

  if (-1 == stat(cameraName.c_str(), &st))
  {
    std::cerr << LOG << "[V4L open] Cannot identify '" << cameraName << "': " 
              << errno << ", " << strerror(errno) << std::endl;
    return;
  }

  if (!S_ISCHR(st.st_mode))
  {
    std::cerr << LOG << "[V4L open] " << cameraName << " is no device " << std::endl;
    return;
  }

  std::cout << LOG << "Opening camera device '" << cameraName << "' " << std::endl;

  // always open file descriptor in non-blocking mode, blocking will be achived with "poll" calls later
  fd = open(cameraName.c_str(), O_RDWR | O_NONBLOCK, 0);

  if (-1 == fd)
  {
    std::cerr << LOG << "[V4L open] Cannot open '" << cameraName << "': " 
              << errno << ", " << strerror(errno) << std::endl;
    return;
  }
}

void V4lCameraHandler::initDevice()
{
  /* Select video input, video standard and tune here. */
  // set image format
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(struct v4l2_format));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = naoth::IMAGE_WIDTH;
  fmt.fmt.pix.height = naoth::IMAGE_HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  VERIFY(ioctl(fd, VIDIOC_S_FMT, &fmt) >= 0);

  /* Note VIDIOC_S_FMT may change width and height. */
  ASSERT(fmt.fmt.pix.sizeimage == naoth::IMAGE_WIDTH * naoth::IMAGE_HEIGHT * 2);

  memset(&currentBuf, 0, sizeof(currentBuf));
  currentBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  currentBuf.memory = V4L2_MEMORY_MMAP;

  // query and verify the capabilities
  struct v4l2_capability cap;
  VERIFY(ioctl(fd, VIDIOC_QUERYCAP, &cap) != -1);
  VERIFY(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE);
  VERIFY(cap.capabilities & V4L2_CAP_STREAMING);

  mapBuffers();
}

// map buffers
void V4lCameraHandler::mapBuffers()
{
  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof(req));

  req.count = frameBufferCount; // number of internal buffers, since we use debug images that should be quite big
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  VERIFY(-1 != ioctl(fd, VIDIOC_REQBUFS, &req));
  VERIFY(req.count == frameBufferCount);

  for (size_t i = 0; i < req.count; ++i)
  {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    VERIFY(-1 != ioctl(fd, VIDIOC_QUERYBUF, &buf));

    buffers[i].length = buf.length;
    buffers[i].start = mmap(
        NULL, /* start anywhere */
        buf.length,
        PROT_READ | PROT_WRITE, /* required */
        MAP_SHARED,             /* recommended */
        fd, buf.m.offset);

    VERIFY(MAP_FAILED != buffers[i].start);
  }
}

void V4lCameraHandler::unmapBuffers()
{
  for (size_t i = 0; i < frameBufferCount; ++i) {
    VERIFY(-1 != munmap(buffers[i].start, buffers[i].length));
  }
}

void V4lCameraHandler::startCapturing()
{
  // queue buffers
  for (size_t i = 0; i < frameBufferCount; ++i)
  {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    VERIFY(-1 != xioctl(fd, VIDIOC_QBUF, &buf));
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(-1 != ioctl(fd, VIDIOC_STREAMON, &type));

  // initialize internal state
  isCapturing = true;
  wasQueried = false;
  lastBuf = currentBuf;
  framesSinceStart = 0;
}

void V4lCameraHandler::stopCapturing()
{
  isCapturing = false;
}

void V4lCameraHandler::closeDevice()
{
  if (-1 == close(fd)) {
    hasIOError(-1, errno);
    return;
  }
  fd = -1;
}

void V4lCameraHandler::shutdown()
{
  stopCapturing();
  unmapBuffers();
  closeDevice();
}

bool V4lCameraHandler::isRunning()
{
  return isCapturing;
}

int V4lCameraHandler::readFrame()
{
  //not the first frame and the buffer changed last frame
  if (wasQueried)
  {
    //put buffer back in the drivers incoming queue
    VERIFY(-1 != xioctl(fd, VIDIOC_QBUF, &lastBuf));
    //std::cout << "give buffer to driver" << std::endl;
  }

  struct v4l2_buffer buf;
  memset(&(buf), 0, sizeof(struct v4l2_buffer));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  // wait for available data via poll
  pollfd pollfds[1] =
  {
    {fd, POLLIN | POLLPRI, 0},
  };
  //  unsigned int startTime = NaoTime::getNaoTimeInMilliSeconds();
  int polled = poll(pollfds, 1, maxPollTime);

  //  unsigned int stopTime = NaoTime::getNaoTimeInMilliSeconds();
  //  std::cout << LOG << "polling took " << (stopTime -  startTime) << " ms" << std::endl;
  if (polled < 0)
  {
    std::cerr << LOG << "Polling camera failed after " << maxPollTime << " ms. Error was: " << strerror(errno) << std::endl;
    return -1;
  }

  if (pollfds[0].revents && !(pollfds[0].revents & POLLIN))
  {
    std::cerr << LOG << "Poll for camera returned unexpected poll code " << pollfds[0].revents << std::endl;
  }

  int errorCode = 0;

  // Deque all camera images in queue until there is none left. Since we polled, we know data should be available.
  bool first = true;
  v4l2_buffer lastValidBuf;
  do
  {
    /*
    // NOTE: 
    Last buffer produced by the hardware. mem2mem codec drivers set this flag on the capture queue 
    for the last buffer when the ioctl VIDIOC_QUERYBUF or VIDIOC_DQBUF ioctl is called. Due to hardware 
    limitations, the last buffer may be empty. In this case the driver will set the bytesused field to 0, 
    regardless of the format. Any subsequent call to the VIDIOC_DQBUF ioctl will not block anymore, 
    but return an EPIPE error code.
    
    // https://www.kernel.org/doc/html/v4.8/media/uapi/v4l/buffer.html?highlight=v4l2_buf_flag_last
    // https://github.com/gebart/python-v4l2capture/issues/16#issuecomment-473519282
    // https://github.com/gebart/python-v4l2capture/issues/16
    */
    errorCode = xioctl(fd, VIDIOC_DQBUF, &buf);

    if (errorCode == 0)
    {
      if (first)
      {
        first = false;
      }
      else
      {
        errorCode = xioctl(fd, VIDIOC_QBUF, &lastValidBuf);
        if (errorCode != 0)
        {
          std::cout << LOG << "Buffer { .index = " << buf.index << ", .bytesused = " << buf.bytesused << ", .flags = " << buf.flags << "}" << std::endl;
          hasIOError(errorCode, errno);
        }
      }
      lastValidBuf = buf;
    }
    else
    {
      if (errno == EAGAIN)
      {
        // last element taken from the queue, abort loop
        if (!first) {
          // reset error code since first try was successfull
          errorCode = 0;
        }
        break;
      }
      else
      {
        // we did do a poll on the file descriptor and still got an error, something is wrong: abort the loop
        // print some info about the buffer
        std::cout << LOG << "Buffer { .index = " << buf.index << ", .bytesused = " << buf.bytesused << ", .flags = " << buf.flags << "}" << std::endl;
        hasIOError(errorCode, errno);
        break;
      }
    }
  } while (errorCode == 0);
  currentBuf = lastValidBuf;

  //remember current buffer for the next frame as last buffer
  lastBuf = currentBuf;

  wasQueried = true;
  ASSERT(currentBuf.index < frameBufferCount);
  if (errorCode == 0) {
    return currentBuf.index;
  } else {
    return -1;
  }
}

void V4lCameraHandler::get(Image &theImage)
{
  if (isCapturing)
  {
    //STOPWATCH_START("ImageRetrieve");
    int resultCode = readFrame();
    //STOPWATCH_STOP("ImageRetrieve");

    if (resultCode < 0)
    {
      std::cerr << LOG << "Could not get image, error code " << errno << " (" << strerror(errno) << ")" << std::endl;
    }
    else
    {
      //if(currentBuf.bytesused != theImage.cameraInfo.size * Image::PIXEL_SIZE_YUV422)
      if (currentBuf.bytesused < theImage.cameraInfo.getSize() * Image::PIXEL_SIZE_YUV422)
      {
        theImage.wrongBufferSizeCount++;
        //        cout << "wrong image buffer size: " << (theImage.cameraInfo.size * Image::PIXEL_SIZE_YUV422) << ", buffer: " << currentBuf.bytesused << "/" << currentBuf.length << endl;
      }
      else
      {
        theImage.wrapImageDataYUV422((unsigned char *)buffers[currentBuf.index].start, currentBuf.bytesused);
        theImage.cameraInfo.cameraID = currentCamera;
        theImage.currentBuffer = currentBuf.index;
        theImage.bufferCount = frameBufferCount;
        theImage.timestamp =
            (unsigned int)((((unsigned long long)currentBuf.timestamp.tv_sec) * NaoTime::long_thousand +
                            ((unsigned long long)currentBuf.timestamp.tv_usec) / NaoTime::long_thousand) -
                           NaoTime::startingTimeInMilliSeconds);

        framesSinceStart++;
      }
    }
  }
}

void V4lCameraHandler::getCameraSettings(CameraSettings &data, bool update)
{
  if (update)
  {
    std::cout << LOG << "V4L camera settings are updated" << std::endl;
    internalUpdateCameraSettings();
    data = currentSettings;
  }
}


void V4lCameraHandler::setAllCameraParams(const CameraSettings& data)
{
  if (framesSinceStart < 5)
  {
    // do nothing if no image was retrieved yet
    std::cerr << LOG << "CAN NOT SET PARAMETERS YET, WAITING ..." << std::endl;
    return;
  }
  else if (framesSinceStart == 5)
  {
    internalUpdateCameraSettings();
    if (settingsManager)
    {
      settingsManager->apply(fd, cameraName, data, true);
    }
  }
  else
  {
    if (settingsManager)
    {
      settingsManager->apply(fd, cameraName, data);
    }
  }
} // end setAllCameraParams

void V4lCameraHandler::internalUpdateCameraSettings()
{
  if (settingsManager)
  {
    settingsManager->query(fd, cameraName, currentSettings);
  }
}

// https://01.org/linuxgraphics/gfx-docs/drm/media/uapi/v4l/capture.c.html
int V4lCameraHandler::xioctl(int fd, int request, void *arg) const
{
  int r;
  // TODO: possibly endless loop?
  do {
    r = ioctl(fd, request, arg);
  } while (-1 == r && EINTR == errno); // repeat if the call was interrupted
  return r;
}

bool V4lCameraHandler::hasIOErrorPrint(int lineNumber, int errOccured, int errNo, bool exitByIOError)
{
  if (errOccured < 0 && errNo != EAGAIN)
  {
    std::cout << LOG << "[hasIOError:" << lineNumber << "]"
              << " failed with errno " << errNo << " (" << strerror(errNo) << ") >> exiting" << std::endl;
    if (exitByIOError && error_count > 10)
    {
      assert(errOccured >= 0);
    }
    error_count++;
    return true;
  }
  return false;
}
