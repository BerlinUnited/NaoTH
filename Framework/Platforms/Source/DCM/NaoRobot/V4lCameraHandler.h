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
#include <memory>

extern "C"
{
#include <getopt.h>

#include <fcntl.h>
#include <unistd.h>
//#include <errno.h>
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
#include "V4LCameraSettingsManager.h"

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
 *
 * The code is based on the V4L examples:
 * https://hverkuil.home.xs4all.nl/codec-api/uapi/v4l/v4l2grab.c.html
 * https://01.org/linuxgraphics/gfx-docs/drm/media/uapi/v4l/capture.c.html
 *
 */

namespace naoth {

class V4lCameraHandler
{
public:
  V4lCameraHandler();
  ~V4lCameraHandler();

  void init(std::string camDevice, CameraInfo::CameraID camID, bool isV6 = false);

  void shutdown();
  bool isRunning();
  
  void get(Image& theImage);
  void getCameraSettings(CameraSettings& data, bool update = false);
  void setAllCameraParams(const CameraSettings& data);

private:

  void resetV6Camera() const;

  void openDevice();
  void initDevice();
  
  void mapBuffers();
  void unmapBuffers();
  void startCapturing();
  
  void stopCapturing();
  void closeDevice();
  
  int readFrame();
  
  void setFPS(int fpsRate);
  void internalUpdateCameraSettings();
  
  // tools
  int xioctl(int fd, int request, void* arg) const;
  bool hasIOErrorPrint(int lineNumber, int errOccured, int errNo, bool exitByIOError = true) const;

private: // data members

  // camera stuff
  std::string cameraName;
  CameraInfo::CameraID currentCamera;

  /** The camera file descriptor */
  int fd;

  // NOTE: the currentBuffer is allways returned to the queue, before a new buffer is dequed,
  //       so 3 buffers are enough. This might change in the future if we decide to retain 
  //       images for longer.
  /** Amount of available frame buffers. */
  static const constexpr unsigned frameBufferCount = 3; 
  
  /** Maximal time in ms to wait for a poll to deliver an image */
  static const constexpr unsigned maxPollTime = 1000;
  
  /** 
    This holds adresses and sizes of the mapped memory buffers use to capture frames. 
    Needed so we can unmap the memory at the end.
  */
  struct Buffer
  {
    void* start;
    size_t length;
  };
  struct Buffer buffers[frameBufferCount];

  /** Structure holding the information regarding the currently captured frame. */
  struct v4l2_buffer currentBuf;

  size_t framesSinceStart;
  bool isCapturing;

  /** Settings */
  CameraSettings currentSettings;
  std::shared_ptr<V4LCameraSettingsManager> settingsManager;
};

} // namespace naoth
#endif	/* _V4LCAMERAHANDLER_H */

