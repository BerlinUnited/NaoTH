#ifndef _OpenCVWebCamController_H
#define _OpenCVWebCamController_H


#include "PlatformInterface/PlatformInterface.h"
#include "DebugCommunication/DebugServer.h"

// Representations
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/IRData.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/BatteryData.h"
#include <Representations/Infrastructure/GameData.h>

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#endif
// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#endif

//std library
#include <string>
#include <vector>

using namespace cv;
using namespace naoth;
using namespace std;

class OpenCVWebCamController : public PlatformInterface
{
public:
  
  OpenCVWebCamController();
  ~OpenCVWebCamController();

  virtual OpenCVWebCamController& getPlatform(){return *this;}

  virtual string getBodyID() const { return "naoth-opencvwebcam"; }

  virtual string getBodyNickName() const {return "opencvwebcam"; }

  virtual string getHeadNickName() const {return "opencvwebcam"; }

  /////////////////////// init ///////////////////////
  void init();

  /////////////////////// main ///////////////////////
  void main();
  unsigned long getCurrentFrameNumber()
  {
    return frameNumber;
  }


  /////////////////////// get ///////////////////////
  //get methods
  virtual void get(Image& data);
  void get(FrameInfo& data);
  void get(GameData& data){ data = theGameData; }


  /////////////////////// set ///////////////////////

protected:
  unsigned long frameNumber;
  virtual MessageQueue* createMessageQueue(const string& name);


private:
  void copyImage(Image& image, Mat& capturedImage);
  void executeFrame();
  void makeStep();

  unsigned int time;

  GameData theGameData;

  string windowName;

  VideoCapture capture;

  Mat yCbCr;
  Mat frameNative;
  Mat frame;

  unsigned int frameLossCounter;

private:

  DebugServer theDebugServer;
public:
  void get(DebugMessageIn& data)
  {
    theDebugServer.getDebugMessageIn(data);
  }

  void set(const DebugMessageOut& data)
  {
    if(data.answers.size() > 0)
      theDebugServer.setDebugMessageOut(data);
  }

};

#endif //#define _OpenCVWebCamController_H