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
 
namespace naoth
{
class V4lCameraHandler
{
public:
  V4lCameraHandler();

  void init(std::string camDevice = "/dev/video");

  void get(Image& theImage);

  void setCameraSettings(const CameraSettings& data, bool queryNew);

  void getCameraSettings(CameraSettings& data);

  virtual ~V4lCameraHandler();
private:

  void initIDMapping();
  void fastCameraSelection(CameraInfo::CameraID camId);
  void openDevice();
  void openIC2();
  void initDevice();
  void initMMap();
  void initQueueBuffers();
  void startCapturing();
  int readFrame();
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

  std::string cameraName;

  /** The camera file descriptor */
  int fd;
  int i2cFd;

  /** Image buffers (v4l2) */
  struct buffer* buffers;
  /** Buffer number counter */
  unsigned int n_buffers ;

  struct v4l2_buffer currentBuf;
  struct v4l2_buffer lastBuf;

  bool wasQueried;
  bool isCapturing;

  int csConst[CameraSettings::numOfCameraSetting];

  CameraSettings currentSettings[CameraInfo::numOfCamera];
  CameraInfo::CameraID currentCamera;

  unsigned int noBufferChangeCount;

};

#endif	/* _V4LCAMERAHANDLER_H */
}
