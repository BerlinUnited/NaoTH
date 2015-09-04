/* 
 * File:   FieldColorPercept.h
 * Author: claas
 *
 * Created on 17. march 2011, 22:53
 */

#ifndef FIELDCOLORPERCEPT_H
#define FIELDCOLORPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/Math/Common.h>
#include <Representations/Infrastructure/FrameInfo.h>

using namespace naoth;

class FieldColorPercept : public naoth::Printable
{

  class ColorRange
  {
  protected:
    PixelT<int> min;
    PixelT<int> max;

  public:
    ColorRange(){}

    void set(int minY, int minU, int minV, int maxY, int maxU, int maxV)
    {
      min.y = minY;
      min.u = minU;
      min.v = minV;
      max.y = maxY;
      max.u = maxU;
      max.v = maxV;
    }

    PixelT<int> getMax() const {
      return max;
    }

    PixelT<int> getMin() const {
      return min;
    }

    inline bool inside(const Pixel& pixel) const
    {
      return  min.y <= pixel.y && pixel.y <= max.y && 
              min.u <= pixel.u && pixel.u <= max.u && 
              min.v <= pixel.v && pixel.v <= max.v;
    }

    inline bool inside(int y, int u, int v) const
    {
      return  min.y <= y && y <= max.y && 
              min.u <= u && u <= max.u && 
              min.v <= v && v <= max.v;
    }
  };

public:
  FrameInfo lastUpdated;
  ColorRange range;
  Pixel histogramField;


  FieldColorPercept(){}
  ~FieldColorPercept(){}

  inline bool isFieldColorOld(int y, int u, int v) const {
    return range.inside(y, u, v);
  }

  inline bool isFieldColor(int y, int u, int v) const {
    Vector2d dp = Vector2d(u, v) - Vector2d(128,128);
    double center = -Math::pi + Math::pi_4;
    double sigma = Math::pi_4;
    double blackOffset = 50;
    double brightnesConeRadiusUV = 50;
    double yy = std::max(1.0,((double)y)-blackOffset) / 255.0;
    return dp.angle() > center - sigma && dp.angle() < center + sigma && dp.abs() > yy*brightnesConeRadiusUV;
  }

  inline bool isFieldColor(const Pixel& pixel) const {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }

  inline void print(std::ostream& stream) const
  {
  }

};

class FieldColorPerceptTop : public FieldColorPercept
{
public:
  virtual ~FieldColorPerceptTop() {}
};


#endif  /* FIELDCOLORPERCEPT_H */

