/* 
 * File:   CameraInfo.h
 * Author: Oliver Welter
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _CAMERAINFO_H
#define	_CAMERAINFO_H

#include "Interface/Tools/Math/Pose3D.h"
#include "Interface/Tools/Math/Common.h"

namespace naorunner
{
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

  class CameraInfoParameter : public CameraInfo
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
    CameraInfoParameter()
    {
      setParameter(resolutionWidth, resolutionHeight, openingAngleDiagonal);
      setCameraTrans();
    }

    void init(const std::string& filename)
    {
      setParameter(resolutionWidth, resolutionHeight, openingAngleDiagonal);
      setCameraTrans();
    }//end init

    double openingAngleDiagonal;

    // offset to the neck joint
    Pose3D transformation[numOfCamera];
  };
}

#endif	/* _CAMERAINFO_H */

