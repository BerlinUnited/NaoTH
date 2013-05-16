/*
 * File:   CameraInfo.h
 * Author: Oliver Welter
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _CAMERAINFO_H
#define _CAMERAINFO_H

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Common.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{

  class CameraInfo: public Printable
  {

    friend class Serializer<CameraInfo>;
  public:
    
    CameraInfo()
    :
    resolutionWidth(320),
    resolutionHeight(240),
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
    cameraID(Bottom),
    cameraRollOffset(0.0),
    cameraTiltOffset(0.0),
    transformation(),
    openingAngleDiagonal(0.0)
    {}

    virtual ~CameraInfo()
    {}

    enum CameraID
    {
      Top,
      Bottom,
      numOfCamera //FIXME: this doesn't correspond to the type naothmessages::CameraID
    };

    unsigned int resolutionWidth;
    unsigned int resolutionHeight;

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

    CameraID cameraID;

    // for calibration
    double cameraRollOffset;
    double cameraTiltOffset;

    // offset to the neck joint
    Pose3D transformation[numOfCamera];

    // getter functions that use the existing values to calculate their result

    double getFocalLength() const;
    double getOpeningAngleWidth() const;
    double getOpeningAngleHeight() const;
    double getOpticalCenterX() const;
    double getOpticalCenterY() const;
    unsigned long getSize() const;
    double getOpeningAngleDiagonal() const;


    virtual void print(std::ostream& stream) const;

  protected:

    double openingAngleDiagonal;

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

