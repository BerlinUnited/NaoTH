/*
 * File:   V4lCameraHandler.cpp
 * Author: Thomas Krause <krause@informatik.hu-berlin.de>
 * Author: Heinrch Mellmann <mellmann@informatik.hu-berlin.de>
 *
 * Created on 22. April 2010, 17:24
 *
 * 
 * The code is based on the V4L examples:
 * https://hverkuil.home.xs4all.nl/codec-api/uapi/v4l/v4l2grab.c.html
 * https://01.org/linuxgraphics/gfx-docs/drm/media/uapi/v4l/capture.c.html
 *
 */

#include "V4lCameraHandler.h"

#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/NaoTime.h"

#include "CameraSettingsV5Manager.h"
#include "CameraSettingsV6Manager.h"

#include <cstring>
#include <iostream>
#include <cerrno>

// C-includes
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> // map or unmap files or devices into memory 
#include <poll.h>

// needed for the reset of the camera on NAO V6
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

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

// useful macros
#define LOG "[CameraHandler:" << __LINE__ << ", Camera: " << cameraName << "] "
#define hasIOError(...) hasIOErrorPrint(__LINE__, __VA_ARGS__)
#define check(...) hasIOError(__VA_ARGS__, errno)


static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,
                                     int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;
 
    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(file,I2C_SMBUS,&args);
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
                         I2C_SMBUS_BYTE_DATA,&data))
        return -1;
    else
        return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value)
{
    union i2c_smbus_data data;
    data.byte = value;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,command, I2C_SMBUS_BYTE_DATA, &data);
}

// NOTE: send a single value as block data
static inline __s32 i2c_smbus_write_block_data(int file, __u8 command, __u8 value)
{
    union i2c_smbus_data data;
    data.block[0] = 1;
    data.block[1] = value;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_DATA, &data);
}

using namespace naoth;
using namespace std;

V4lCameraHandler::V4lCameraHandler()
  : cameraName("none"),
    currentCamera(CameraInfo::numOfCamera),
    fd(-1),
    framesSinceStart(0),
    isCapturing(false)
{
  // DEBUG: test of the hasIOError macro
  //hasIOError(-1, EPIPE);
}

V4lCameraHandler::~V4lCameraHandler()
{
  std::cout << "[V4lCameraHandler] stop wait" << std::endl;
  shutdown();
  std::cout << "[V4lCameraHandler] stop done" << std::endl;
}

void V4lCameraHandler::init(const std::string& camDevice, CameraInfo::CameraID camID, bool isV6)
{
  // shut down the camera if it was running before doing anything else
  if (isCapturing) {
    shutdown();
  }
  
  // initialize parameter manager
  if (isV6) {
    resetV6Camera();
    settingsManager = std::make_shared<CameraSettingsV6Manager>();
  } else {
    settingsManager = std::make_shared<CameraSettingsV5Manager>();
  }

  currentCamera = camID;
  cameraName = camDevice;

  // open the device
  openDevice();
  initDevice(naoth::IMAGE_WIDTH, naoth::IMAGE_HEIGHT);
  setFrameRate(30);
  mapBuffers();
  
  // DEBUG: read and print available controls 
  settingsManager->enumerate_controls(fd);
  
  startCapturing();
}

// reset camera
void V4lCameraHandler::resetV6Camera() const 
{
  // NOTE: reset should be done only once at the start of the program
  static bool resetDone = false;
  if(resetDone) {
    return;
  }
  resetDone = true;
  
  // Infos about working with i2c:
  // https://github.com/mozilla-b2g/i2c-tools/blob/master/tools/i2cbusses.c
  //  open_i2c_dev(...)
  
  // connect to the i2c bus
  int i2c = open("/dev/i2c-head", O_RDWR);
  if(i2c < 0) {
    fprintf(stderr, "[V4lCameraHandler] Error: Could not open file '/dev/i2c-head': %s\n", strerror(errno));
    assert(false);
  } 
  
  // select the slave device (camera)
  int err = ioctl(i2c, I2C_SLAVE, 0x41);
  if(err < 0) {
    fprintf(stderr, "[V4lCameraHandler] Error: I2C_SLAVE: %s\n", strerror(errno));
    assert(false);
  }
  
  // check if the device is set to writing mode?
  const __s32 result = i2c_smbus_read_byte_data(i2c, 0x3);
  if (result < 0) {
    fprintf(stderr, "[V4lCameraHandler] Error: i2c_smbus_read_byte_data: %s\n", strerror(errno));
  } else if ((result & 0xc) != 0) {
    // configure as output
    //check(i2c_smbus_write_byte_data(i2c, 0x3, 0xf3));
    check(i2c_smbus_write_block_data(i2c, 0x3, 0xf3));
    //i2c_smbus_write_byte_data(i2c, 0x1, 0x0); // reset_camera_hold
  }
  
  // send the reset command?
  //check(i2c_smbus_write_byte_data(i2c, 0x1, 0x0)); // reset_camera_hold
  //check(i2c_smbus_write_byte_data(i2c, 0x1, 0xc)); // reset_camera_release
  check(i2c_smbus_write_block_data(i2c, 0x1, 0x0)); // reset_camera_hold
  check(i2c_smbus_write_block_data(i2c, 0x1, 0xc)); // reset_camera_release
  
  close(i2c);
  fprintf(stdout, "[V4lCameraHandler] Done resetting cameras '/dev/i2c-head'.");
  sleep(3);
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

void V4lCameraHandler::initDevice(uint32_t width, uint32_t height)
{
  /* Select video input, video standard and tune here. */
  // set image format
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(struct v4l2_format));
  fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width       = width;
  fmt.fmt.pix.height      = height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field       = V4L2_FIELD_NONE;
  VERIFY(xioctl(fd, VIDIOC_S_FMT, &fmt) >= 0);

  // NOTE: VIDIOC_S_FMT may change width and height.
  // Make sure the settings were accepted
  ASSERT(fmt.fmt.pix.sizeimage == width * height * 2);

  // query and verify the capabilities
  struct v4l2_capability cap;
  VERIFY(xioctl(fd, VIDIOC_QUERYCAP, &cap) != -1);
  VERIFY(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE);
  VERIFY(cap.capabilities & V4L2_CAP_STREAMING);
}

void V4lCameraHandler::setFrameRate(int rate)
{
  struct v4l2_streamparm fps;
  memset(&fps, 0, sizeof(fps));
  fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(!ioctl(fd, VIDIOC_G_PARM, &fps));
  
  fps.parm.capture.timeperframe.numerator = 1;
  fps.parm.capture.timeperframe.denominator = rate;
  VERIFY(ioctl(fd, VIDIOC_S_PARM, &fps) != -1);
}

// map buffers
void V4lCameraHandler::mapBuffers()
{
  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof(req));
  
  req.count  = frameBufferCount; // number of internal buffers
  req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  VERIFY(-1 != ioctl(fd, VIDIOC_REQBUFS, &req));
  VERIFY(req.count == frameBufferCount);

  for (size_t i = 0; i < req.count; ++i)
  {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = i;

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
  
  // initialize the strucuture holding the currently captured frame
  memset(&currentBuf, 0, sizeof(currentBuf));
  currentBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  currentBuf.memory = V4L2_MEMORY_MMAP;

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(-1 != ioctl(fd, VIDIOC_STREAMON, &type));

  // initialize internal state
  isCapturing = true;
  framesSinceStart = 0;
}

void V4lCameraHandler::stopCapturing()
{
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(-1 != xioctl(fd, VIDIOC_STREAMOFF, &type));
  isCapturing = false;
}

void V4lCameraHandler::closeDevice()
{
  if (-1 == close(fd)) {
    hasIOError(-1, errno);
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
  // return the current buffer to the incomming queue
  // NOTE: it might be better to check the V4L2_BUF_FLAG_QUEUED flag
  if (currentBuf.bytesused != 0) {
    // put buffer back in the drivers incoming queue
    VERIFY(-1 != xioctl(fd, VIDIOC_QBUF, &currentBuf));
    // clear the current buffer
    memset(&currentBuf, 0, sizeof(currentBuf));
    currentBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    currentBuf.memory = V4L2_MEMORY_MMAP;
  }
  
  // poll: suspend execution until the driver has captured data
  pollfd pollfds[1] =
  {
    {fd, POLLIN | POLLPRI, 0},
  };
  int polled = poll(pollfds, 1, maxPollTime);
  
  if (polled < 0) {
    std::cerr << LOG << "Polling camera failed after " << maxPollTime << " ms. Error was: " << strerror(errno) << std::endl;
    return -1;
  }

  if (pollfds[0].revents && !(pollfds[0].revents & POLLIN)) {
    std::cerr << LOG << "Poll for camera returned unexpected flags: " << pollfds[0].revents << std::endl;
    return -1;
  }

  /*
  // NOTE: 
  V4L2_BUF_FLAG_LAST
  Last buffer produced by the hardware. mem2mem codec drivers set this flag on the capture queue 
  for the last buffer when the ioctl VIDIOC_QUERYBUF or VIDIOC_DQBUF ioctl is called. Due to hardware 
  limitations, the last buffer may be empty. In this case the driver will set the bytesused field to 0, 
  regardless of the format. Any subsequent call to the VIDIOC_DQBUF ioctl will not block anymore, 
  but return an EPIPE error code.
  
  // https://www.kernel.org/doc/html/v4.8/media/uapi/v4l/buffer.html?highlight=v4l2_buf_flag_last
  // https://github.com/gebart/python-v4l2capture/issues/16#issuecomment-473519282
  // https://github.com/gebart/python-v4l2capture/issues/16
  */
    
  // Get the most recent buffer. Deque all camera images from the outgoing queue to get the newest one. 
  // NOTE: poll was successful, so we know that at least one buffer should be available.
  v4l2_buffer lastBuf;
  bool first = true;
  int errorCode = 0;
  // We are dequeuing images from the buffer until there is no image left. The loop stops when we reach 
  // a busy buffer and errorCode == -1 is returned (with errno == EAGAIN)
  while((errorCode = xioctl(fd, VIDIOC_DQBUF, &currentBuf)) == 0)
  {
    if(first) {
      first = false;
    }
    else if(xioctl(fd, VIDIOC_QBUF, &lastBuf) < 0) {
      // was not able to return the buffer for some reason
      std::cout << LOG << "Buffer { .index = " << currentBuf.index 
                       << ", .flags = " << std::bitset<32>(currentBuf.flags)
                       << ", .bytesused = " << currentBuf.bytesused 
                       << "}" << std::endl;
      hasIOError(errorCode, errno);
    }
    lastBuf = currentBuf;
  }
  
  // EAGAIN is expected: the above loop reached a busy buffer
  if (errno == EAGAIN)
  {
    // the above loop made at least one iteration, it means currentBuf is valid
    if (!first) {
      errorCode = 0;
    }
  }
  else
  {
    // we did do a poll on the file descriptor and still got an error, something is wrong: abort the loop
    // print some info about the buffer
    std::cout << LOG << "Buffer { .index = " << currentBuf.index << ", .bytesused = " << currentBuf.bytesused << ", .flags = " << currentBuf.flags << "}" << std::endl;
    hasIOError(errorCode, errno);
  }

  ASSERT(currentBuf.index < frameBufferCount);
  if (errorCode == 0) {
    return currentBuf.index;
  } else {
    return -1;
  }
}

void V4lCameraHandler::get(Image& theImage)
{
  if (isCapturing)
  {
    //unsigned long long start = NaoTime::getSystemTimeInMicroSeconds();
    int resultCode = readFrame();
    //unsigned long long end = NaoTime::getSystemTimeInMicroSeconds();
    //std::cout << LOG << " readFrame (us): " << (end - start) << std::endl;

    if (resultCode < 0)
    {
      std::cerr << LOG << "Could not get image, error code " << errno << " (" << strerror(errno) << ")" << std::endl;
    }
    else
    {
      //if(currentBuf.bytesused != theImage.cameraInfo.size * Image::PIXEL_SIZE_YUV422)
      if (currentBuf.bytesused != theImage.cameraInfo.getSize() * Image::PIXEL_SIZE_YUV422)
      {
        theImage.wrongBufferSizeCount++;
        //        cout << "wrong image buffer size: " << (theImage.cameraInfo.size * Image::PIXEL_SIZE_YUV422) << ", buffer: " << currentBuf.bytesused << "/" << currentBuf.length << endl;
      }
      else
      {
        theImage.wrapImageDataYUV422(static_cast<unsigned char*>(buffers[currentBuf.index].start), currentBuf.bytesused);
        theImage.cameraInfo.cameraID = currentCamera;
        
        // for debug purposes
        theImage.currentBuffer = currentBuf.index;
        theImage.bufferCount = frameBufferCount;
        
        // convert the timestamp to robot time (milliseconds sinse the start of the process)
        const struct timeval t = currentBuf.timestamp;
        theImage.timestamp = static_cast<unsigned int>(t.tv_sec * 1000LL + t.tv_usec / 1000LL - NaoTime::startingTimeInMilliSeconds);
        
        // NOTE: usual time difference seems to be (robotTime - theImage.timestamp) ~ 31 or 50 ms. Why?!
        // DEBUG: sanity check to make sure the time stamps align reasonably (delay < 1s)
        unsigned int robotTime = NaoTime::getNaoTimeInMilliSeconds();
        if(robotTime > theImage.timestamp + 1000 || robotTime < theImage.timestamp) {
          std::cout << LOG << "WARNING: image timestamp doesn't match robot time:" << std::endl; 
          std::cout << "  robot= " << robotTime << std::endl; 
          std::cout << "  image= " << theImage.timestamp << std::endl;
        }

        /*
        // DEBUG: troubleshooting the timing
        std::cout << "it: " << theImage.timestamp << std::endl;
        std::cout << "rt: " << robotTime << std::endl;
        
        #define V4L2_BUF_FLAG_TIMESTAMP_MASK        0x0000e000
        #define V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC   0x00002000
        std::cout << "is monotonic: " << (currentBuf.flags & V4L2_BUF_FLAG_TIMESTAMP_MASK) << std::endl;
        
        #define V4L2_BUF_FLAG_TSTAMP_SRC_SOE 	0x00010000
        std::cout << "is SOE: " << (currentBuf.flags & V4L2_BUF_FLAG_TSTAMP_SRC_SOE) << std::endl;
        
        struct timespec m;
        clock_gettime(CLOCK_MONOTONIC ,&m);

        struct timespec r;
        clock_gettime(CLOCK_REALTIME ,&r);

        unsigned long long ts = NaoTime::getSystemTimeInMicroSeconds();
        
        //std::cout << "tm: " << (currentBuf.flags & V4L2_BUF_FLAG_TIMESTAMP_MASK) << std::endl;
        //std::cout << "sm: " << (currentBuf.flags & V4L2_BUF_FLAG_TSTAMP_SRC_MASK) << std::endl;
        //std::cout << "fl: " << std::bitset<32>(currentBuf.flags) << std::endl;
        std::cout << "i0: " << currentBuf.timestamp.tv_sec << std::endl;
        std::cout << "m0: " << m.tv_sec << std::endl;
        std::cout << "r0: " << r.tv_sec << std::endl << std::endl;
        
        std::cout << "i1: " << currentBuf.timestamp.tv_usec << std::endl;
        std::cout << "m1: " << (m.tv_nsec/1000LL) << std::endl;
        std::cout << "r1: " << (r.tv_nsec/1000LL) << std::endl;
        std::cout << "ns: " << ts << std::endl << std::endl;
        */
        
        framesSinceStart++;
      }
    }
  }
}

void V4lCameraHandler::getCameraSettings(CameraSettings &data, bool update)
{
  //FIXME: this seems to be never executed, because update is never set to true
  if (update)
  {
    std::cout << LOG << "V4L camera settings are updated" << std::endl;
    internalUpdateCameraSettings();
    data = currentSettings;
  }
}

void V4lCameraHandler::setAllCameraParams(const CameraSettings& data)
{
  ASSERT_MSG(settingsManager, "No settingsManager was set.");

  // TODO: move the initialization of the parameters into a separate function which 
  // is executed once at the start

  // HACK: wait for some frames bevore setting parameters
  // to give the camera driver some time. This seeps to increase the chance
  // that the parameters are actually applied 
  constexpr int framesToWait = 5;

  if (framesSinceStart < framesToWait) {
    std::cerr << LOG << "CAN NOT SET PARAMETERS YET, WAITING ..." << std::endl;
    return;
  } else if (framesSinceStart == framesToWait) {
    // initialize parameters
    internalUpdateCameraSettings();
    // force apply settins
    settingsManager->apply(fd, cameraName, data, true);
  } else {
    // update: only apply changed settings 
    settingsManager->apply(fd, cameraName, data);
  }
} // end setAllCameraParams

void V4lCameraHandler::internalUpdateCameraSettings()
{
  ASSERT_MSG(settingsManager, "No settingsManager was set.");
  settingsManager->query(fd, cameraName, currentSettings);
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

bool V4lCameraHandler::hasIOErrorPrint(int lineNumber, int errOccured, int errNo, bool exitByIOError) const
{
  if (errOccured < 0 && errNo != EAGAIN)
  {
    std::cout << LOG << "[hasIOError:" << lineNumber << "]"
              << " failed with errno " << errNo << " (" << strerror(errNo) << ") >> exiting" << std::endl;
    
    // NOTE: it hoolds errOccured < 0
    assert(!exitByIOError);
    
    return true;
  }
  return false;
}
