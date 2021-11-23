/*
 * File:   CameraInfo.h
 * Author: Oliver Welter
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef _CAMERAINFO_H_
#define _CAMERAINFO_H_

#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{
  // TODO: remove this
  static const unsigned int IMAGE_WIDTH = 640;
  static const unsigned int IMAGE_HEIGHT = 480;

  class CameraInfoParameter : public ParameterList
  {
  public:
    //diagonal angle of field of view
    double openingAngleDiagonal;

    /*
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
    */
    CameraInfoParameter(const std::string& idName);
  };

  class CameraInfo: public Printable
  {
    friend class Serializer<CameraInfo>;
  public:
    enum CameraID
    {
      Top,
      Bottom,
      numOfCamera //FIXME: this doesn't correspond to the type naothmessages::CameraID
    };
    
    CameraInfo()
    :
    cameraID(Bottom),
    resolutionWidth(IMAGE_WIDTH),
    resolutionHeight(IMAGE_HEIGHT),
    params(getCameraIDName(Bottom))
    {}

    CameraInfo(CameraID id)
    :
    cameraID(id),
    resolutionWidth(IMAGE_WIDTH),
    resolutionHeight(IMAGE_HEIGHT),
    params(getCameraIDName(id))
    {}

    virtual ~CameraInfo()
    {}

    CameraID cameraID;

    unsigned int resolutionWidth;
    unsigned int resolutionHeight;

    // getter functions that use the existing values to calculate their result

    double getFocalLength() const;
    double getOpeningAngleWidth() const;
    double getOpeningAngleHeight() const;
    double getOpticalCenterX() const;
    double getOpticalCenterY() const;
    unsigned long getSize() const;
    double getOpeningAngleDiagonal() const;

    CameraInfoParameter& getCameraInfoParameter() {
      return params;
    }

    virtual void print(std::ostream& stream) const;

    inline static std::string getCameraIDName(CameraID id)
    {
      switch(id)
      {
        case Top: return "Top";
        case Bottom: return "Bottom";
        default: return "unknown";
      }
    }

  protected:
    CameraInfoParameter params;
  };


  template<>
  class Serializer<CameraInfo>
  {
    public:
    static void serialize(const CameraInfo& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, CameraInfo& representation);
  };


  class CameraInfoTop : public CameraInfo
  {
  public:
    CameraInfoTop() : CameraInfo(Top) {};
    virtual ~CameraInfoTop() {}
  };

  template<>
  class Serializer<CameraInfoTop> : public Serializer<CameraInfo>{};
  
}

#endif  /* _CAMERAINFO_H_ */

