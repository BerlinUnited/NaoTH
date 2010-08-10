/* 
 * File:   CameraInfo.h
 * Author: Oliver Welter
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _CAMERAINFO_H
#define	_CAMERAINFO_H

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Common.h"
#include "Tools/Debug/ParameterList.h"

class CameraInfo
{
public:
  
  CameraInfo()
    : 
    cameraID(Bottom)
  {
  }

  enum CameraID
  {
    Top,
    Bottom,
    numOfCamera
  };

  void setParameter(
    unsigned int resolutionWidth,
    unsigned int resolutionHeight,
    double openingAngleDiagonal
    )
  {
    openingAngleDiagonal = Math::fromDegrees(openingAngleDiagonal);
    
    // calculate focal length
    double d2 = resolutionWidth*resolutionWidth+resolutionHeight*resolutionHeight;
    double halfDiagLength = 0.5*sqrt(d2);

    this->focalLength = halfDiagLength/tan(0.5*openingAngleDiagonal);

    // calculate opening angle
    this->openingAngleHeight = 2.0*atan2((double)this->resolutionHeight,this->focalLength*2.0);
    this->openingAngleWidth = 2.0*atan2((double)this->resolutionWidth,this->focalLength*2.0);

    // calculate optical senter
    this->opticalCenterY = this->resolutionHeight/2;
    this->opticalCenterX = this->resolutionWidth/2;
    
    // values needed by Image
    this->size = this->resolutionHeight * this->resolutionWidth;

  }//end setParameter

  //double pixelSize;
  unsigned int resolutionWidth;
  unsigned int resolutionHeight;
  
  double focalLength;
  double openingAngleWidth;
  double openingAngleHeight;
  double opticalCenterX;
  double opticalCenterY;

  unsigned long size;

  CameraID cameraID;

  // for calibration
  double cameraRollOffset;
  double cameraTiltOffset;


};

class CameraInfoParameter : public CameraInfo, public ParameterList
{
private:
  struct CameraTransInfo
  {
    Vector3<double> offset;
    double rotationY;
  };

  CameraTransInfo cameraTrans[numOfCamera];

  void setCameraTrans()
  {
    for(int i=0; i<numOfCamera; i++)
    {
      transformation[i].translation = cameraTrans[i].offset;
      transformation[i].rotation = RotationMatrix::getRotationY(Math::fromDegrees(cameraTrans[i].rotationY));
    }
  }

public:
  CameraInfoParameter():ParameterList("CameraInfo")
  {
    PARAMETER_REGISTER(resolutionWidth) =320;
    PARAMETER_REGISTER(resolutionHeight) = 240;
    PARAMETER_REGISTER(openingAngleDiagonal) = 58;
    PARAMETER_REGISTER(cameraRollOffset) = 0;
    PARAMETER_REGISTER(cameraTiltOffset) = 0;

    PARAMETER_REGISTER(cameraTrans[Top].offset.x) = 53.9;
    PARAMETER_REGISTER(cameraTrans[Top].offset.y) = 0;
    PARAMETER_REGISTER(cameraTrans[Top].offset.z) = 67.9;
    PARAMETER_REGISTER(cameraTrans[Top].rotationY) = 0;
    PARAMETER_REGISTER(cameraTrans[Bottom].offset.x) = 48.8;
    PARAMETER_REGISTER(cameraTrans[Bottom].offset.y) = 0;
    PARAMETER_REGISTER(cameraTrans[Bottom].offset.z) = 23.81;
    PARAMETER_REGISTER(cameraTrans[Bottom].rotationY) = 40.0;

    setParameter(resolutionWidth, resolutionHeight, openingAngleDiagonal);
    setCameraTrans();
  }

  void init(const std::string& filename)
  {
    setConfigFile(filename);
    loadFromConfigFile();
    setParameter(resolutionWidth, resolutionHeight, openingAngleDiagonal);
    setCameraTrans();
  }//end init
  
  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream){
    ParameterList::executeDebugCommand(command, arguments, outstream);
    setParameter(resolutionWidth, resolutionHeight, openingAngleDiagonal);
    setCameraTrans();
  }//end executeDebugCommand

  double openingAngleDiagonal;

  // offset to the neck joint
  Pose3D transformation[numOfCamera];
};

#endif	/* _CAMERAINFO_H */

