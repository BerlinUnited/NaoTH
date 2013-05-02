/*
 * File:   OpenCVImageLoader.cpp
 * Author: Kirill Yasinovskiy
 *
 * Created on 24. Juni 2011
 */

#ifdef  WIN32
#include <conio.h>
#include <windows.h>
#include <winbase.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include "myconio.h"
#endif //WIN32

#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>

#include "PlatformInterface/Platform.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/Math/Common.h"
#include "Tools/NaoTime.h"
#include "Messages/Framework-Representations.pb.h"

#include "OpenCVImageLoader.h"

#include <glib/gstdio.h>

using namespace std;
using namespace cv;
using namespace naoth;

OpenCVImageLoader::OpenCVImageLoader(const char* dirPath)
:PlatformInterface("OpenCVImageLoader", CYCLE_TIME)
{
  // register input
  directoryName = dirPath;
  //cvNamedWindow("ImageLoader");

  if(findFiles(dirPath))
  {
    currentPos = 0;
    imageLoaded = false;

    theGameData.loadFromCfg(Platform::getInstance().theConfiguration);
    theDebugServer.start(5401, true);
    time = 0;
    //how ofter an image should be repeated before next one is loaded
    maxPictureStepCount = 60000;
    //picturecount forward and backward
    forwardCount = maxPictureStepCount;
    backwardCount = maxPictureStepCount;

    registerInput<FrameInfo>(*this);
    registerInput<Image>(*this);

    // debug
    registerInput<DebugMessageIn>(*this);
    registerOutput<DebugMessageOut>(*this);
  }
  else
  // couldn't find any files, exit...
  {
    cerr << "The directory doesn't contain any data..." << endl;
    exit(EXIT_FAILURE);
  }
}

void OpenCVImageLoader::main()
{
  printHelp();
  cout << directoryName << endl;
  char c;
  while((c = getch()) && c != 'q' && c != 'x')
  {
    if (c == 'd')
    {
      stepForward(forwardCount);
    }
    if (c == 'a')
    {
      stepBack(backwardCount);
    }
    if (c == 'D')
    {
      stepForward(0);
    }
    if (c == 'A')
    {
      stepBack(0);
    }
    if (c == 'w')
    {
      // read jump name
      string filename;
      filename.reserve(100);
      cout << "jump to file: ";
      cin >> filename;
      jumpTo(filename);
    }
    if (c == 's')
    {
      jumpToStart();
    }
    if (c == 'r')
    {
      executeCognition();
    }
    if (c == 'p')
    {
      play(false);
    }
    if (c == 'P')
    {
      play(true);
    }
    if (c == 'l')
    {
      listFiles();
    }
    else if(c == 'h')
    {
      printHelp();
    }//end else if
  }// while
  cout << endl << "bye bye!" << endl;
}//end main

void OpenCVImageLoader::makeStep()
{
  if (!allFiles.empty())
  {
    // execute
    if (loadImage(loadedImage))
    {
      time += getBasicTimeStep();
      runCognition();
      runMotion();
      imageLoaded = true;
    }//end if
    else
    {
      return;
    }
  }
  else
  {
    cout << "you didn't load any files..." << endl;
  }
}//end make


void OpenCVImageLoader::stepBack(int pictureSteps)
{
  if(pictureSteps <= 0)
  {
    //cvDestroyWindow(allFiles[currentPos].c_str());
    if (currentPos > 0)
    {
      currentPos--;
    }
    else
    {
      currentPos = allFiles.size() - 1;
    }//end if
    forwardCount = maxPictureStepCount;
    backwardCount = maxPictureStepCount;
  }
  else
  {
    forwardCount = maxPictureStepCount - backwardCount;
  }//end if
  makeStep();
  backwardCount--;
  forwardCount++;
}

void OpenCVImageLoader::stepForward(int pictureSteps)
{
  if(pictureSteps <= 0)
  {
    //cvDestroyWindow(allFiles[currentPos].c_str());
    if (currentPos < allFiles.size() - 1)
    {
      currentPos++;
    }
    else
    {
      currentPos = 0;
    }//end if
    forwardCount = maxPictureStepCount;
    backwardCount = maxPictureStepCount;
  }
  else
  {
    backwardCount = maxPictureStepCount - forwardCount;
  }//end if
  makeStep();
  forwardCount--;
  backwardCount++;
}//end stepForward

void OpenCVImageLoader::jumpTo(const string fileName)
{
  //cvDestroyWindow(allFiles[currentPos].c_str());
  bool wasFound = false;
  for (unsigned int i = 0; i < allFiles.size(); ++i)
  {
    if (allFiles[i] == fileName)
    {
      wasFound = true;
      currentPos = i;
      forwardCount = maxPictureStepCount;
      backwardCount = maxPictureStepCount;
    }//end if
  }//end for
  if(wasFound)
  {
    makeStep();
  }//end if
  else
  {
    cout << "file not found!" << endl;
  }//end else
}//end jumpTo

void OpenCVImageLoader::jumpToStart()
{
  //cvDestroyWindow(allFiles[currentPos].c_str());
  currentPos = 0;
  forwardCount = maxPictureStepCount;
  backwardCount = maxPictureStepCount;
  makeStep();
}//end jumpToStart

void OpenCVImageLoader::executeCognition()
{
  if (imageLoaded)
  {
    cout << "cognition: just executing new cycle" << endl;
    time += getBasicTimeStep();
    runCognition();
    runMotion();
  }
  else
  {
    cout << "you haven't load any image yet!" << endl;
  }
}//end executeCognition

void OpenCVImageLoader::play(bool onePictureMode)
{
  int c = -1;
  while
  (
    c != 'a' && c != 'd' && c != 'A' && c != 'D' &&
    c != '\n' && c != 'w' && c != 'q' && c !='x' &&
    c !='s' && c !='r' && c !='p' && c !='P' && c !='l' && c !='h'
  )
  {
    unsigned int startTime = NaoTime::getNaoTimeInMilliSeconds();
    if(onePictureMode)
    {
      stepForward(0);
    }
    else
    {
      stepForward(forwardCount);
    }
    unsigned int waitTime = Math::clamp(33 - (NaoTime::getNaoTimeInMilliSeconds() - startTime),(unsigned int) 5, (unsigned int) 33);

#ifdef WIN32
    Sleep(waitTime);
    if(_kbhit())
#else
    // wait some time
    usleep(waitTime * 1000);
#endif
    c = getch();
  }
}// end play

void OpenCVImageLoader::printHelp()
{
  cout << endl;
  cout << "Welcome to the NaoTH openCVImageLoader" << endl;
  cout << "--------------------------------------" << endl << endl;

  cout << "\"WASD\"-control" << endl;
  cout << "d - one step forward, show each picture for " << maxPictureStepCount << " steps" << endl;
  cout << "a - one step backward , show each picture for " << maxPictureStepCount << " steps" << endl;
  cout << "D - one step forward to next picture" << endl;
  cout << "A - one step backward to previous picture" << endl;
  cout << "s - jump to the first file" << endl;
  cout << "w - jump to specific file" << endl << endl;
  cout << "p - play all files in directory, press 'p' to stop playing" << endl << "    show each picture for " << maxPictureStepCount << " steps" << endl;
  cout << "P - play all files in directory, press 'P' to stop playing" << endl << "    show each picture for one step" << endl;

  cout << "l - list all files in the directory" << endl;
  cout << "r - just execute cognition (current image) one more time" << endl;
  cout << "q or x - quit/exit" << endl << endl;
}//end printHelp

bool OpenCVImageLoader::findFiles(const char* dirName)
{
    int count = 0;

    GDir* directory = g_dir_open(dirName, 0, NULL);

    if(directory != NULL)
    {
        const char* fileName = g_dir_read_name(directory);

        while(fileName != NULL)
        {
            gchar* fileWithFullPath;
            fileWithFullPath = g_build_filename(dirName, fileName, (gchar*)NULL);

            if(g_file_test (fileWithFullPath, G_FILE_TEST_IS_REGULAR))
            {
                count++;
                allFiles.push_back(fileWithFullPath);
            }

            fileName = g_dir_read_name(directory);
        }

        g_dir_close(directory);
    }
    return count > 0;
}//end lisDir

void OpenCVImageLoader::listFiles()
{
  for (unsigned int i = 0; i < allFiles.size(); ++i)
  {
    cout << allFiles[i] << endl;
  }
  cout << "The directory have totally " << allFiles.size() << " files." << endl;
}//end listFiles

void OpenCVImageLoader::get(Image& data)
{
  //ACHTUNG: this is set by the module CameraInfoSetter
  //data.setCameraInfo(Platform::getInstance().theCameraInfo);
  if (data.cameraInfo.resolutionWidth == 320)
  {
    copyImage(data, loadedImage);
  }//if
}//end get

void OpenCVImageLoader::get(FrameInfo& data)
{
  data.setTime(time);
  data.setFrameNumber(data.getFrameNumber() + 1);
}


void OpenCVImageLoader::copyImage(Image& image, Mat mat)
{
  Mat temp;
  Size size;
  size.width = 320;
  size.height = 240;
  //resize the image
  resize(mat, temp, size, 0, 0, INTER_CUBIC);
  cvtColor(temp, temp, CV_RGB2YCrCb);

  int currentWidth = 320;
  int currentHeight = 240;
  Pixel pixel;

  for(int x = 0; x < currentWidth - 1; x++)
  {
    for(int y = 0; y < currentHeight - 1; y++)
    {
      int index = 3 * (x + y * currentWidth);
      for(int k = 0; k < 3; k++)
      {
        pixel.channels[k] = temp.data[index + k];
      }//end for
      image.set(x, y, pixel);
    }//end for
  }//end for
}//end copyImage

bool OpenCVImageLoader::loadImage(Mat& image)
{
  // Open the file.
  IplImage* img = cvLoadImage(allFiles[currentPos].c_str());
  if(!img)
  {
    printf("Could not load image file: %s\n",allFiles[currentPos].c_str());
    return false;
  }
  Mat temp(img);
  if (temp.empty())
  {
    cout << "Couldn't open the image file :" << allFiles[currentPos] << endl;
    return false;
  }
  //did we load the image properly?
  if (!temp.data)
  {
    cout << "Image data not loaded properly" << allFiles[currentPos] <<endl;
    return false;
  }//end if

  if(forwardCount == maxPictureStepCount && backwardCount == maxPictureStepCount)
  {
    cout << endl << "loaded image: " << allFiles[currentPos] << endl;
  }
  else
  {
    cout << ".";
  }
  image = temp.clone();
//  cvShowImage(allFiles[currentPos].c_str(), img);
  cvShowImage("ImageLoader", img);

  cvWaitKey(1);
  //cvDestroyWindow(allFiles[currentPos].c_str());
  cvReleaseImage(&img);
  temp.release();
  return true;
}//end loadImage



MessageQueue* OpenCVImageLoader::createMessageQueue(const std::string& name)
{
  // for single thread
  return new MessageQueue();
}
