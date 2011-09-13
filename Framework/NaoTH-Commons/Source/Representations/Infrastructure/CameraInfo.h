/* 
 * File:   CameraInfo.h
 * Author: Oliver Welter
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _CAMERAINFO_H
#define  _CAMERAINFO_H

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Common.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{

  class CameraInfo: public Printable
  {
  public:
    
    CameraInfo()
    :
    resolutionWidth(320),
    resolutionHeight(240),
    focalLength(0.0),
    openingAngleWidth(0.0),
    openingAngleHeight(0.0),
    opticalCenterX(0.0),
    opticalCenterY(0.0),
    pixelSize(0.0),
    focus(0.0),
    xp(0.0),
    yp(0.0),
    k1(0.0),
    k2(0.0),
    k3(0.0),
    p1(0.0),
    p2(0.0),
    b1(0.0),
    b2(0.0),
    size(0),
    cameraID(Bottom),
    cameraRollOffset(0.0),
    cameraTiltOffset(0.0)
    {}

    virtual ~CameraInfo()
    {}

    enum CameraID
    {
      Top,
      Bottom,
      numOfCamera
    };

    unsigned int resolutionWidth;
    unsigned int resolutionHeight;
    
    double focalLength;
    double openingAngleWidth;
    double openingAngleHeight;
    double opticalCenterX;
    double opticalCenterY;

    //size of an Pixel on the chip
    double pixelSize;
    //measured focus
    double focus;
    //moved middle point
    double xp;
    double yp;
    //radial symmetric distortion parameters
    double k1;
    double k2;
    double k3;
    //radial asymmetric and tangential distortion parameters
    double p1;
    double p2;
    //affinity and ... distortion parameters
    double b1;
    double b2;

    unsigned long size;

    CameraID cameraID;

    // for calibration
    double cameraRollOffset;
    double cameraTiltOffset;

    void setParameter(unsigned int resolutionWidth, unsigned int resolutionHeight, double openingAngleDiagonal);

    virtual void print(ostream& stream) const;
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

    void setCameraTrans();

  public:
    CameraInfoParameter();

    void init();
    
    double openingAngleDiagonal;

    // offset to the neck joint
    Pose3D transformation[numOfCamera];


  };
  
  template<>
  class Serializer<CameraInfo>
  {
    public:
    static void serialize(const CameraInfo& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, CameraInfo& representation);
  };
  
}

#endif  /* _CAMERAINFO_H */

