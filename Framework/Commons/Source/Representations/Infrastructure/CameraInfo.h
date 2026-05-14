/*
 * File:   CameraInfo.h
 * Author: Oliver Welter
 *
 * Created on 1. Februar 2009, 20:07
 */

#ifndef CAMERAINFO_H
#define CAMERAINFO_H

#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include "Tools/Math/Vector2.h"

namespace naoth
{
  // TODO: remove this
  static const unsigned int IMAGE_WIDTH  = 640;
  static const unsigned int IMAGE_HEIGHT = 480;

  class CameraInfoParameter : public ParameterList
  {
  public:
    CameraInfoParameter(const std::string& idName) : ParameterList("CameraInfo" + idName)
    {
      PARAMETER_ANGLE_REGISTER(openingAngleDiagonal) = 72.6;

      PARAMETER_REGISTER(opticalCenter.x) = IMAGE_WIDTH / 2;
      PARAMETER_REGISTER(opticalCenter.y) = IMAGE_HEIGHT / 2;

      syncWithConfig();
    }

  public:
    // diagonal angle of field of view
    double openingAngleDiagonal;

    Vector2d opticalCenter;
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
    
    CameraInfo() : CameraInfo(Bottom) {}

    CameraInfo(CameraID id)
      :
      cameraID(id),
      resolutionWidth(IMAGE_WIDTH),
      resolutionHeight(IMAGE_HEIGHT),
      params(getCameraIDName(id))
    {}

    virtual ~CameraInfo() {}

    CameraID cameraID;

    unsigned int resolutionWidth;
    unsigned int resolutionHeight;

    // getter functions that use the existing values to calculate their result

    double getFocalLength() const
    {
      double halfDiagLength = 0.5 * hypot(resolutionWidth, resolutionHeight);

      // senity check
      ASSERT(halfDiagLength > 0.0 && getOpeningAngleDiagonal() > 0.0);
      return halfDiagLength / tan(0.5 * getOpeningAngleDiagonal());
    }

    inline double getOpeningAngleDiagonal() const {
      return params.openingAngleDiagonal;
    }

    double getOpeningAngleHeight() const {
      return 2.0 * atan2(static_cast<double>(resolutionHeight), getFocalLength() * 2.0);
    }

    double getOpeningAngleWidth() const {
      return 2.0 * atan2(static_cast<double>(resolutionWidth), getFocalLength() * 2.0);
    }

    double getOpticalCenterX() const {
      return params.opticalCenter.x;
      //return static_cast<double>(resolutionWidth / 2);
    }

    double getOpticalCenterY() const {
      return params.opticalCenter.y;
      //return static_cast<double>(resolutionHeight / 2);
    }

    inline unsigned long getSize() const {
      return resolutionHeight * resolutionWidth;
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

#endif // CAMERAINFO_H

