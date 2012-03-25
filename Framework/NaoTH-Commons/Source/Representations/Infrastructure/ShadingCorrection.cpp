#include "ShadingCorrection.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace naoth;
using namespace std;

ShadingCorrection::ShadingCorrection()
:
yC(NULL),
camID(CameraInfo::Bottom),
width(0),
height(0),
size(0)
{}//end constructor

ShadingCorrection::~ShadingCorrection()
{
  clear();
}//end destructor

void ShadingCorrection::init(unsigned int w, unsigned int h, CameraInfo::CameraID cam)
{
  clear();
  width = w;
  height = h;
  camID = cam;
  size = w * h;
  yC = new unsigned short[size];
  reset();
  loadCorrectionFromFile
  (
    Platform::getInstance().theConfigDirectory,
    Platform::getInstance().theHeadHardwareIdentity
  );
}

void ShadingCorrection::clear()
{
  delete[] yC;
}//end destructor


bool ShadingCorrection::loadCorrectionFromFile(string camConfigPath, string hardwareID )
{
  if(yC != NULL)
  {
    stringstream cameraYCPath;

    cameraYCPath << camConfigPath << "private/camera_";
    cameraYCPath << hardwareID << "_" << width << "x" << height;
    if(camID == CameraInfo::Bottom)
    {
      cameraYCPath << "_bottom.yc";
    }
    else
    {
      cameraYCPath << "_top.yc";
    }
   
    cout << "Loading from " << cameraYCPath.str() << endl;
    ifstream inputFileStream ( cameraYCPath.str().c_str() , ifstream::in | ifstream::binary );

    if(inputFileStream.fail())
    {
      // could not open color table
      reset();
      cout << " fail" << endl;
      return false;
    }//end if

    inputFileStream.read((char*) yC, size * sizeof(unsigned short));
    inputFileStream.close();

    // check the brigness Correction data
    for(unsigned int pos = 0; pos < size; pos++)
    {
//      if(yC[pos] > 261120) // bigger than 1024 * 255 ?
      if(yC[pos] > 65025) // bigger than 255 * 255 ?
      {
        std::cout << std::endl << "brightness correction table broken!!! ["
          << pos << "] = "
          << yC[pos]
          << std::endl << std::endl;
        // clear the colortable
        reset();
        cout << " fail" << endl;
        return false;
      }
    }
    cout << " ok" << endl;
    return true;
  }
  return false;
}//end loadCorrectionFromFile

void ShadingCorrection::saveCorrectionToFile(string camConfigPath, string hardwareID)
{
  if(yC != NULL)
  {
    stringstream cameraYCPath;
    cameraYCPath << camConfigPath << "private/camera_";
    cameraYCPath << hardwareID << "_" << width << "x" << height;
    if(camID == CameraInfo::Bottom)
    {
      cameraYCPath << "_bottom.yc";
    }
    else
    {
      cameraYCPath << "_top.yc";
    }
    cout << "Saving to " << cameraYCPath.str() << endl;
    ofstream outputFileStream ( cameraYCPath.str().c_str() , ofstream::out | ofstream::binary );

    outputFileStream.write((char*) yC, size * sizeof(unsigned short));
    outputFileStream.close();
  }
}//end saveCorrectionToFile

void ShadingCorrection::reset()
{
  for(unsigned long pos = 0; pos < size; pos++)
  {
    yC[pos] = 1024;
  }
}//end reset


