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
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/BumperData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/GameData.h>

//platform:
#include "PlatformInterface/PlatformInterface.h"


//opencv:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace naoth;
using namespace std;
using namespace cv;

class OpenCVImageLoader : public PlatformInterface<OpenCVImageLoader>
{
public:
  OpenCVImageLoader(const char* dirPath);
  ~OpenCVImageLoader(){};

  virtual string getBodyID() const { return "naoth-opencvimageloader"; }

  virtual string getBodyNickName() const {return "naoth"; }

  void main();
  bool findFiles(const char* dirName);
  void listFiles();


  void printHelp();
  
  void executeCognition();
  void stepForward();
  void stepBack();
  void jumpToStart();
  void jumpTo(const string fileName);

  //get methods
  void get(Image& data);
  void get(GameData& data){ data = theGameData; }

  
protected:
  virtual MessageQueue* createMessageQueue(const string& name);

private:
  //directory name
  const char* directoryName;
  //vector containing all file names in directory
  vector<const string> allFiles;
  //current position
  unsigned int currentPos;

  GameData theGameData;
  unsigned int currentFrame;
  bool imageLoaded;

  void copyImage(Image& image, Mat mat);

  Mat loadedImage;

  bool loadImage(Mat& mat);
  void makeStep();
};


#endif //end OpenCVImageLoader