/*
 * File:   OpenCVImageLoader.h
 * Author: Kirill Yasinovskiy
 *
 * Created on 24. Juni 2011
 */

#ifndef _OpenCVImageLoader_h_
#define _OpenCVImageLoader_h_

#define CYCLE_TIME 100

//include section
//representations:
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/IRData.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/BatteryData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/GameData.h"

//platform:
#include "PlatformInterface/PlatformInterface.h"
#include "DebugCommunication/DebugServer.h"

#ifndef WIN32
#pragma GCC diagnostic ignored "-Wconversion"
#endif
//opencv:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#ifndef WIN32
#pragma GCC diagnostic error "-Wconversion"
#endif

//glib 2.0
#include <glib.h>

using namespace naoth;
using namespace std;
using namespace cv;

class OpenCVImageLoader : public PlatformInterface
{
public:
  OpenCVImageLoader(const char* dirPath);
  ~OpenCVImageLoader(){/*cvDestroyWindow("ImageLoader");*/};

  virtual OpenCVImageLoader& getPlatform(){return *this;}

  virtual string getBodyID() const { return "naoth-opencvimageloader"; }

  virtual string getBodyNickName() const {return "naoth"; }

  virtual string getHeadNickName() const {return "naoth"; }

  void main();
  bool findFiles(const char* dirName);
  void listFiles();


  void printHelp();

  void executeCognition();
  void stepForward(int pictureSteps);
  void stepBack(int pictureSteps);
  void jumpToStart();
  void jumpTo(const string fileName);
  void play(bool onePictureMode);

  //get methods
  virtual void get(Image& data);
  void get(GameData& data){ data = theGameData; }
  void get(FrameInfo& data);


protected:
  virtual MessageQueue* createMessageQueue(const string& name);

private:
  //directory name
  const gchar* directoryName;

  //vector containing all file names in directory
  vector<string> allFiles;
  //current position
  unsigned int currentPos;

  GameData theGameData;
  unsigned int time;
  int maxPictureStepCount;
  int forwardCount;
  int backwardCount;
  bool imageLoaded;

  void copyImage(Image& image, Mat mat);

  Mat loadedImage;

  bool loadImage(Mat& mat);
  void makeStep();

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


#endif //end OpenCVImageLoader
