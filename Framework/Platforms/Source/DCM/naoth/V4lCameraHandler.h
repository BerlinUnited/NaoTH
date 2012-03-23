/* 
 * File:   V4lCameraHandler.h
 * Author: thomas
 *
 * Created on 22. April 2010, 17:24
 */

#ifndef _V4LCAMERAHANDLER_H
#define	_V4LCAMERAHANDLER_H

#include <string>

extern "C"
{
#include <getopt.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>

#include <linux/videodev2.h>
}

#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraSettings.h"

//#include "Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/Stopwatch.h"


struct buffer
{
  void * start;
  size_t length;
};

/**
 * This is a CameraHandler that uses the V4L2 API directly. It will use the
 * Memory Map Streaming method for getting the images (which should be the
 * fastest possibility).
 *
 * For an example code look at http://v4l2spec.bytesex.org/spec/a16706.htm .
 * The whole specification is located at http://v4l2spec.bytesex.org/spec/book1.htm .
 *
 * @author Thomas Krause
 *
 */
 
 namespace naoth{
 
class V4lCameraHandler
{
public:
  V4lCameraHandler();

  void init(std::string camDevice = "/dev/video");

  void get(Image& theImage);

  void setCameraSettings(const CameraSettings& data, bool queryNew);

  void getCameraSettings(CameraSettings& data);

  void shutdown();

  virtual ~V4lCameraHandler();

private:

  int xioctl(int fd, int request, void* arg);
  bool hasIOError(int errOccured, int errNo, bool exitByIOError);
  void initIDMapping();
  void initialCameraSelection(CameraInfo::CameraID camId);
  void openDevice(bool blockingMode);
  void initDevice();
  void initI2C();
  void initMMap();
  void initUP(unsigned int buffer_size);
  void initRead(unsigned int buffer_size);
  void startCapturing();
  int readFrame();
  int readFrameMMaP();
  int readFrameUP();
  int readFrameRead();
  void stopCapturing();
  void uninitDevice();
  void closeDevice();

  int getSingleCameraParameter(int id);
  int getSingleCameraParameterCheckFlip(int id, CameraInfo::CameraID camId);
  bool setSingleCameraParameter(int id, int value);
  bool setSingleCameraParameterCheckFlip(CameraSettings::CameraSettingID i,CameraInfo::CameraID newCam, int value);
  void setFPS(int fpsRate);
  void internalUpdateCameraSettings();
  void internalSetCameraSettings(const CameraSettings& data);

  string getErrnoDescription(int err);

  typedef enum
  {
    IO_READ,
    IO_MMAP,
    IO_USERPTR,
    Num_of_MethodIO
  } MethodIO;

  MethodIO selMethodIO;
  MethodIO actMethodIO;

  std::string cameraName;

  /** The camera file descriptor */
  int fd;

  /** The camera adapter file descriptor */
  int fdAdapter;

  /** Image buffers (v4l2) */
  struct buffer* buffers;
  /** Buffer number counter */
  unsigned int n_buffers ;

  struct v4l2_buffer currentBuf;
  struct v4l2_buffer lastBuf;

  unsigned char* currentImage;

  bool hadReset;
  bool wasQueried;
  bool isCapturing;
  bool bufferSwitched;
  bool blockingCaptureModeEnabled;

  int csConst[CameraSettings::numOfCameraSetting];

  CameraSettings currentSettings[CameraInfo::numOfCamera];
  CameraInfo::CameraID currentCamera;

  unsigned int noBufferChangeCount;

};

} // namespace naoth
#endif	/* _V4LCAMERAHANDLER_H */

