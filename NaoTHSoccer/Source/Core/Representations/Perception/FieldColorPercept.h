/* 
 * File:   FieldColorPercept.h
 * Author: claas
 *
 * Created on 17. march 2011, 22:53
 */

#ifndef FIELDCOLORPERCEPT_H
#define FIELDCOLORPERCEPT_H

#include <Tools/DataStructures/Printable.h>

#include <Tools/ImageProcessing/FieldColorParameters.h>

#include <Tools/ColorClasses.h>

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

    ColorRange()
    {
      
    }

    void set(int minY, int minU, int minV, int maxY, int maxU, int maxV)
    {
      min.y = minY;
      min.u = minU;
      min.v = minV;
      max.y = maxY;
      max.u = maxU;
      max.v = maxV;
    }

    PixelT<int> getMax() const
    {
      return max;
    }

    PixelT<int> getMin() const
    {
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
  bool valid;
  double distY;
  double distU;
  double distV;
  int indexY;
  int indexU;
  int indexV;

  double maxRateY;
  double maxRateU;
  double maxRateV;

  int borderLeftV;
  int borderRightV;

  int maxY;
  int maxU;
  int minU;

  FrameInfo lastUpdated;
  ColorRange range;

  FieldColorPercept()  
  : 
  valid(false),
  distY(5),
  distU(5),
  distV(5),
  maxRateY(0.0),
  maxRateU(0.0),
  maxRateV(0.0),
  indexY(0),
  indexU(0),
  indexV(0),
  //distV(5.0),
  //indexV(100),
  borderLeftV(0),
  borderRightV(0),
  maxY(127),
  maxU(127),
  minU(0)
  {}

  ~FieldColorPercept()
  {}

  void set()
  {
    borderLeftV = (int) Math::clamp<double>(indexV - distV, 0.0, 255.0);
    borderRightV = (int) Math::clamp<double>(indexV + distV, 0.0, 255.0);
    range.set(0, minU, borderLeftV, maxY, maxU, borderRightV);
  }

  inline bool isFieldColorOld(int yy, int cb, int cr) const
  {
    return borderLeftV < cr && cr < borderRightV && cb < maxU && yy < maxY;
  }

  inline bool isFieldColor(const int& y, const int& u, const int& v) const
  {
    return range.inside(y, u, v);
  }

  inline bool isFieldColor(const Pixel& pixel) const
  {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }

  inline void print(std::ostream& stream) const
  {
    stream << "max value in Y channel = " << maxY << std::endl;
    stream << "max value in Cb (U) channel = " << maxU << std::endl;
    stream << "distance in Cr (V) channel = " << distV << std::endl;
    stream << "index in Cr (V) channel = " << indexV << std::endl;
  }//end print

};

class FieldColorPerceptTop : public FieldColorPercept
{
public:
  virtual ~FieldColorPerceptTop() {}
};


#endif  /* FIELDCOLORPERCEPT_H */

