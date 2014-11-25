/*
 * File:   V4lCameraHandler.h
 * Author: thomas
 *
 * Created on 22. April 2010, 17:24
 */

#ifndef _V4LCAMERAHANDLER_H
#define	_V4LCAMERAHANDLER_H

#include <string>
#include <list>

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
 * NOTE (Settings issues):
 * In general, it seems that the camera settings are preserved after the camera was started once.
 * Exposure is a special case - while the setting value itself is preserved, it seems to be 
 * NOT applied internaly at the start of the camera. So we have to reset it every time.
 * Exposure setting is ONLY applied if the new value is different from the current one. 
 * This is why we have to change it in the "HACK (exposure)".
 * 
 * There are also some driver issues, see "HACK (FadeToBlack and Sharpness)"
 */

namespace naoth {

class V4lCameraHandler
{
public:
  V4lCameraHandler();
  ~V4lCameraHandler();

  void init(std::string camDevice, CameraInfo::CameraID camID, bool blockingMode);

  void get(Image& theImage);
  void getCameraSettings(CameraSettings& data, bool update = false);

  void shutdown();

  bool isRunning();
  void setAllCameraParams(const CameraSettings &data);

private:
  void initIDMapping();
  void openDevice(bool blockingMode);

  void initDevice();
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
  bool setSingleCameraParameter(int id, int value);
  void setFPS(int fpsRate);
  void internalUpdateCameraSettings();

  // tools
  int xioctl(int fd, int request, void* arg) const;
  bool hasIOError(int errOccured, int errNo, bool exitByIOError = true) const;
  std::string getErrnoDescription(int err) const;

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

  /** Image buffers (v4l2) */
  struct buffer* buffers;
  /** Buffer number counter */
  unsigned int n_buffers;

  struct v4l2_buffer currentBuf;
  struct v4l2_buffer lastBuf;

  unsigned char* currentImage;

  bool atLeastOneImageRetrieved;
  bool wasQueried;
  bool isCapturing;
  bool bufferSwitched;
  bool blockingCaptureModeEnabled;


  int csConst[CameraSettings::numOfCameraSetting];
  unsigned long long lastCameraSettingTimestamp;

  /** order in which the camera settings need to be applied */
  std::list<CameraSettings::CameraSettingID> settingsOrder;

  CameraSettings currentSettings;
  CameraInfo::CameraID currentCamera;

};

} // namespace naoth
#endif	/* _V4LCAMERAHANDLER_H */

