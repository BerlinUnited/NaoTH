/* 
 * File:   FieldColorPercept.h
 * Author: claas
 *
 * Created on 17. März 2011, 22:53
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
  double distV;
  int indexV;

  int borderLeftV;
  int borderRightV;

  int maxY;
  int maxU;

  FrameInfo lastUpdated;
  ColorRange range;

  FieldColorPercept()  
  : 
  valid(false),
  distV(5.0),
  indexV(100),
  borderLeftV(0),
  borderRightV(0),
  maxY(127),
  maxU(127)
  {}

  ~FieldColorPercept()
  {}

  void set()
  {
    borderLeftV = (int) Math::clamp<double>(indexV - distV, 0.0, 255.0);
    borderRightV = (int) Math::clamp<double>(indexV + distV, 0.0, 255.0);
  }

  inline bool isFieldColorOld(const int& yy, const int& cb, const int& cr) const
  {
    return borderLeftV < cr && cr < borderRightV && cb < maxU && yy < maxY;
  }

  inline bool isFieldColor(int y, int u, int v) const
  {
    return range.inside(y, u, v);
  }

  inline bool isFieldColor(const Pixel& pixel) const
  {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }

  inline void print(ostream& stream) const
  {
    stream << "max value in Y channel = " << maxY << endl;
    stream << "max value in Cb (U) channel = " << maxU << endl;
    stream << "distance in Cr (V) channel = " << distV << endl;
    stream << "index in Cr (V) channel = " << indexV << endl;
  }//end print

};


#endif  /* FIELDCOLORPERCEPT_H */

