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
  yC = new unsigned int[size];
  reset();
};

void ShadingCorrection::clear()
{
  if(yC != NULL)
  {
    delete[] yC;
    yC = NULL;
  }
}//end destructor

unsigned int ShadingCorrection::getY(unsigned int x, unsigned int y) const
{
  //if not initialized or invalid pixel, return default value
  if(x > width || y > height || yC == NULL )
  {
    return 1024;
  }
  return yC[y * width + x];
}//end getY

bool ShadingCorrection::loadCorrectionFromFile(string camConfigPath, string hardwareID )
{
  if(yC != NULL)
  {
    stringstream cameraYCPath;
    cameraYCPath << camConfigPath << "/camera_";
    cameraYCPath << hardwareID << "_" << width << "x" << height;
    if(camID == CameraInfo::Bottom)
    {
      cameraYCPath << "_bottom.yc";
    }
    else
    {
      cameraYCPath << "_top.yc";
    }
   
    cout << "Load " << cameraYCPath.str();
    ifstream inputFileStream ( cameraYCPath.str().c_str() , ifstream::in | ifstream::binary );

    if(inputFileStream.fail())
    {
      // could not open color table
      reset();
      cout << " fail" << endl;
      return false;
    }//end if

    inputFileStream.read((char*) yC, size * sizeof(unsigned int));
    inputFileStream.close();

    // check the brigness Correction data
    for(unsigned int pos = 0; pos < size; pos++)
    {
      if(yC[pos] > 261120) // bigger than 1024 * 255 ?
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
    cameraYCPath << camConfigPath << "/camera_";
    cameraYCPath << hardwareID << "_" << width << "x" << height;
    if(camID == CameraInfo::Bottom)
    {
      cameraYCPath << "_bottom.yc";
    }
    else
    {
      cameraYCPath << "_top.yc";
    }
    cout << "Load " << cameraYCPath.str() << endl;
    ofstream outputFileStream ( cameraYCPath.str().c_str() , ofstream::out | ofstream::binary );

    outputFileStream.write((char*) yC, size * sizeof(unsigned int));
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


