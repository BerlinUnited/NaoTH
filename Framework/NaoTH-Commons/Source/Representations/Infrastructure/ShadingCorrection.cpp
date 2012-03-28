#include "ShadingCorrection.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace naoth;
using namespace std;

ShadingCorrection::ShadingCorrection()
:
camID(CameraInfo::Bottom),
width(0),
height(0),
size(0)
{
  data[0] = NULL;
  data[1] = NULL;
  data[2] = NULL;
}//end constructor

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
  data[0] = new unsigned short[size];
  data[1] = new unsigned short[size];
  data[2] = new unsigned short[size];
  reset();
  loadCorrectionFromFiles
  (
    Platform::getInstance().theConfigDirectory,
    Platform::getInstance().theHeadHardwareIdentity
  );
}

void ShadingCorrection::clear()
{
  delete[] data[0];
  delete[] data[1];
  delete[] data[2];
}//end destructor


bool ShadingCorrection::loadCorrectionFromFiles(string camConfigPath, string hardwareID )
{
  bool ok = false;
  if(data[0] != NULL && data[1] != NULL && data[2] != NULL)
  {
    stringstream cameraYCPath;
    stringstream cameraUCPath;
    stringstream cameraVCPath;

    cameraYCPath << camConfigPath << "private/camera_";
    cameraYCPath << hardwareID << "_" << width << "x" << height;
    cameraUCPath << cameraYCPath;
    cameraVCPath << cameraYCPath;
    if(camID == CameraInfo::Bottom)
    {
      cameraYCPath << "_bottom.yc";
      cameraUCPath << "_bottom.uc";
      cameraVCPath << "_bottom.vc";
    }
    else
    {
      cameraYCPath << "_top.yc";
      cameraUCPath << "_top.uc";
      cameraVCPath << "_top.vc";
    }
   
    cout << "Loading from " << cameraYCPath.str() << endl;
    ok = loadCorrectionFile(cameraYCPath.str(), 0);
    cout << "Loading from " << cameraUCPath.str() << endl;
    ok = ok || loadCorrectionFile(cameraUCPath.str(), 1);
    cout << "Loading from " << cameraVCPath.str() << endl;    
    ok = ok || loadCorrectionFile(cameraVCPath.str(), 2);
  }
  return ok;
}//end loadCorrectionFromFiles

bool ShadingCorrection ::loadCorrectionFile(string filePath, unsigned int idx)
{
  ifstream inputFileStream ( filePath.c_str() , ifstream::in | ifstream::binary );

  if(inputFileStream.fail())
  {
    // could not open file
    cout << filePath << " failed to load" << endl;
    reset(idx);
    return false;
  }//end if

  inputFileStream.read((char*) data[idx], size * sizeof(unsigned short));
  inputFileStream.close();

  // check the brigness Correction data
  for(unsigned int pos = 0; pos < size; pos++)
  {
    if(data[idx][pos] > 65025) // bigger than 255 * 255 ?
    {
      std::cout << std::endl << "brightness correction table broken!!! ["
        << pos << "] = "
        << data[idx][pos]
        << std::endl << std::endl;
      cout << " failed" << endl;
      reset(idx);
      return false;
    }
  }
  cout << " ok" << endl;
  return true;
}//end loadCorrectionFile


void ShadingCorrection::saveCorrectionToFiles(string camConfigPath, string hardwareID)
{
  if(data[0] != NULL && data[1] != NULL && data[2] != NULL)
  {
    stringstream cameraYCPath;
    stringstream cameraUCPath;
    stringstream cameraVCPath;

    cameraYCPath << camConfigPath << "private/camera_";
    cameraYCPath << hardwareID << "_" << width << "x" << height;
    cameraUCPath << cameraYCPath;
    cameraVCPath << cameraYCPath;
    if(camID == CameraInfo::Bottom)
    {
      cameraYCPath << "_bottom.yc";
      cameraUCPath << "_bottom.uc";
      cameraVCPath << "_bottom.vc";
    }
    else
    {
      cameraYCPath << "_top.yc";
      cameraUCPath << "_top.uc";
      cameraVCPath << "_top.vc";
    }
    cout << "Saving to " << cameraYCPath.str() << endl;
    saveCorrectionFile(cameraYCPath.str(), 0);
    cout << "Saving to " << cameraUCPath.str() << endl;
    saveCorrectionFile(cameraUCPath.str(), 1);
    cout << "Saving to " << cameraVCPath.str() << endl;
    saveCorrectionFile(cameraVCPath.str(), 2);
  }
}//end saveCorrectionToFile

void ShadingCorrection ::saveCorrectionFile(string filePath, unsigned int idx)
{
  ofstream outputFileStream ( filePath.c_str() , ofstream::out | ofstream::binary );
  outputFileStream.write((char*) data[idx], size * sizeof(unsigned short));
  outputFileStream.close();
}//end saveCorrectionFile


void ShadingCorrection::reset()
{
  reset(0);
  reset(1);
  reset(2);
}//end reset

void ShadingCorrection::reset(unsigned int idx)
{
  for(unsigned long pos = 0; pos < size; pos++)
  {
    data[idx][pos] = 1024;
  }
}//end reset


