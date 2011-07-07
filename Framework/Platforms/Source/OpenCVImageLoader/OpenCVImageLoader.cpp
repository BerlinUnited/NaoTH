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
#include <io.h>
#include <algorithm>

#include "PlatformInterface/Platform.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/Math/Common.h"

#include "OpenCVImageLoader.h"

using namespace std;
using namespace cv;
using namespace naoth;

OpenCVImageLoader::OpenCVImageLoader(const char* dirPath)
:PlatformInterface<OpenCVImageLoader>("OpenCVImageLoader", CYCLE_TIME)
{
  // register input
  directoryName = dirPath;
  registerInput<Image>(*this);
  findFiles(dirPath);
  allFiles.reserve(1000);
  currentPos = 0;
  imageLoaded = false;

  theGameData.loadFromCfg(Platform::getInstance().theConfiguration);
  theDebugServer.start(5401, true);
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
      stepForward();
    }
    if (c == 'a')
    {
      stepBack();
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
  bool result = loadImage(loadedImage);
  // execute
  if (result)
  {
    callCognition();
    callMotion();
    imageLoaded = true;
  }//end if
  else
  {
    return;
  }
}//end make

void OpenCVImageLoader::stepBack()
{
  if (currentPos > 0)
  {
    currentPos--;
  }//end if
  else
  {
    currentPos = 0;
  }//end else
  makeStep();
}

void OpenCVImageLoader::stepForward()
{
  if (currentPos < allFiles.size() - 1)
  {
    currentPos++;
  }//end if
  else
  {
    currentPos = allFiles.size() - 1;
  }//end else
  makeStep();
}//end stepForward

void OpenCVImageLoader::jumpTo(const string fileName)
{
  bool wasFound = false;
  for (unsigned int i = 0; i < allFiles.size(); ++i)
  {
    if (allFiles[i] == fileName)
    {
      wasFound = true;
      currentPos = i;
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
  currentPos = 0;
  makeStep();
}//end jumpToStart

void OpenCVImageLoader::executeCognition()
{
  if (imageLoaded)
  {
    cout << "cognition: just executing new cycle" << endl;
    callCognition();
    callMotion();
    // HACK: set the frame number...
    // in fact we don't have to update if every time
    theGameData.frameNumber = currentFrame;
  }
  else
  {
    cout << "you haven't load any image yet!" << endl;
  }
}//end executeCognition

void OpenCVImageLoader::printHelp()
{
  cout << endl;
  cout << "Welcome to the NaoTH openCVImageLoader" << endl;
  cout << "--------------------------------------" << endl << endl;

  cout << "\"WASD\"-control" << endl;
  cout << "d - one step forward" << endl;
  cout << "a - one step back" << endl;
  cout << "s - jump to the first file" << endl;
  cout << "w - jump to specific file" << endl << endl;

  cout << "l - list all files in the directory" << endl;
  cout << "r - just execute cognition (current image) one more time" << endl;
  cout << "q or x - quit/exit" << endl << endl;
}//end printHelp

bool OpenCVImageLoader::findFiles(const char* dirName)
{
  //which files do we want to find?
  //int const size = static_cast<int>(strlen(dirName));
  //char fileName[size];
  char* fileName = (char*) malloc(strlen(dirName));
  char mask[20] = "*.*";
  //path to the directory and files:
  strcpy(fileName, (char*) dirName);
  strcat(fileName, mask);
  //struct, that contents all the information about
  //the file
  struct _finddata_t info_about_file;
  long hFile;
  if ((hFile = _findfirst(fileName, &info_about_file)) == -1L)
  {
    //we found no files
    return false;
  }
  else
  {
    while(_findnext(hFile, &info_about_file) == 0)
    {
      if (info_about_file.attrib & _A_SUBDIR)
      {
        continue;
      }//end if
      else
      {
        allFiles.push_back(info_about_file.name);
      }//end else
    }//end file
  }//end else
  //release resources
  _findclose(hFile);
  return true;
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
  data.setCameraInfo(Platform::getInstance().theCameraInfo);
  if (data.cameraInfo.resolutionWidth == 320)
  {
    copyImage(data, loadedImage);
  }//if
}//end get

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

  //release resources
  temp.release();
}//end copyImage

bool OpenCVImageLoader::loadImage(Mat& image)
{
  // Open the file.
  char name[256];
  strcpy(name, directoryName);
  strcat(name, allFiles[currentPos].c_str());
  IplImage *img = cvLoadImage(name);
  if (!img) {
    cout << "Couldn't open the image file :" << name << endl;
    return false;
  }

  cv::Mat temp(img);
  image = temp.clone();

  
  //did we load the image?
  if (image.empty())
  {
    cout << "Can not load image " << name << endl;
    return false;
  }//end if
  //did we load the image properly?
  if (!image.data)
  {
    cout << "Image data not loaded properly" << allFiles[currentPos] <<endl;
    return false;
  }//end if
  cout << "loaded image: " << name << endl;
  //imshow("Image", image);
  //cvWaitKey(0);

  //release resources
  cvReleaseImage(&img);
  temp.release();
  return true;
}//end loadImage



MessageQueue* OpenCVImageLoader::createMessageQueue(const std::string& name)
{
  // for single thread
  return new MessageQueue();
}
