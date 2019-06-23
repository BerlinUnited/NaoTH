#ifndef _V4LCAMERAHANDLERV6_H
#define	_V4LCAMERAHANDLERV6_H

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

#include "CameraSettingsV5Manager.h"

/**
 * This is a CameraHandler that uses the V4L2 API directly. It will use the
 * Memory Map Streaming method for getting the images (which should be the
 * fastest possibility).
 *
 * For an example code look at http://v4l2spec.bytesex.org/spec/a16706.htm .
 * The whole specification is located at http://v4l2spec.bytesex.org/spec/book1.htm .
 *
 */

namespace naoth {

class V4lCameraHandlerV6
{
public:
  V4lCameraHandlerV6();
  ~V4lCameraHandlerV6();

  void init(std::string camDevice, CameraInfo::CameraID camID, bool blockingMode);

  void get(Image& theImage);
  void getCameraSettings(CameraSettings& data, bool update = false);

  void shutdown();

  bool isRunning();
  void setAllCameraParams(const CameraSettings &data);

private:
  struct buffer
  {
    void * start;
    size_t length;
  };

  void initIDMapping();
  void openDevice(bool blockingMode);

  void initDevice();
  void initMMap();
  void startCapturing();
  int readFrame();
  int readFrameMMaP();
  void stopCapturing();
  void uninitDevice();
  void closeDevice();

  int getSingleCameraParameter(int id, std::string name);
  int32_t getSingleCameraParameterUVC(CameraSettings::CameraSettingID id);
  bool setSingleCameraParameter(int id, int value, std::string name);
  bool setSingleCameraParameterUVC(CameraSettings::CameraSettingID id, int32_t value);
  void setFPS(int fpsRate);
  void internalUpdateCameraSettings();

  // tools
  int xioctl(int fd, int request, void* arg) const;
  bool hasIOError(int errOccured, int errNo, bool exitByIOError = true, std::string paramName = "") const;
  std::string getErrnoDescription(int err) const;

  int getAutoExposureGridID(size_t i, size_t j) {
    return V4L2_CID_PRIVATE_BASE + 7 + (i*CameraSettings::AUTOEXPOSURE_GRID_SIZE) + j;
  }

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
  bool initialParamsSet;
  bool wasQueried;
  bool isCapturing;
  bool bufferSwitched;
  bool blockingCaptureModeEnabled;


  int csConst[CameraSettings::numOfCameraSetting];
  // Maps a camera setting to the UVC Extension 3 selector ID
  int uvcExtensionSelector[CameraSettings::numOfCameraSetting]; 
  uint16_t uvcExtensionDataSize[CameraSettings::numOfCameraSetting];
  unsigned long long lastCameraSettingTimestamp;

  /** order in which the camera settings need to be applied */
  std::list<CameraSettings::CameraSettingID> settingsOrder;
  std::list<CameraSettings::CameraSettingID> uvcSettingsOrder;

  CameraSettingsV5Manager currentSettings;
  CameraInfo::CameraID currentCamera;

};

} // namespace naoth
#endif	/* _V4LCAMERAHANDLERV&_H */

