/* 
 * File:   FieldColorPercept.h
 * Author: claas
 *
 * Created on 17. März 2011, 22:53
 */

#ifndef FIELDCOLORPERCEPT_H
#define FIELDCOLORPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/ImageProcessing/FieldColorParameters.h>
#include <Tools/ColorClasses.h>

#include <Representations/Infrastructure/FrameInfo.h>


using namespace naoth;

class FieldColorPercept : public naoth::Printable
{
private:
  class ColorRegion
  {
  private:
    Pixel min;
    Pixel max;
    bool valid;
  public:
    ColorRegion()
    {
      set(0, 0, 0, 0, 0, 0);
      valid = false;
    }

    void set
    (
      unsigned char minY,
      unsigned char maxY,
      unsigned char minU,
      unsigned char maxU,
      unsigned char minV,
      unsigned char maxV
    )
    {
      min.y = minY;
      min.u = minU;
      min.v = minV;
      max.y = maxY;
      max.u = maxU;
      max.v = maxV;
    }

    void set( Pixel& min_, Pixel& max_ )
    {
      min = min_;
      max = max_;
    }


    inline bool inside(const unsigned short& y, const unsigned short& u, const unsigned short& v) const
    {
      return min.y < y && y < max.y && min.u < u && u < max.u && min.v < v && v < max.v;
    }//end inside
  };


  ColorRegion greenRegion;

public:
  double distY;
  double distCb;
  double distCr;

  double maxWeightedY;
  double maxWeightedCb;
  double maxWeightedCr;

  unsigned int maxWeightedIndexY;
  unsigned int maxWeightedIndexCb;
  unsigned int maxWeightedIndexCr;


  void set()
  {
    greenRegion.set(
      (unsigned char)(max(maxWeightedIndexY - distY,0.0)),
      (unsigned char)(min(maxWeightedIndexY + distY,255.0)),
      (unsigned char)(max(maxWeightedIndexCb - distCb,0.0)),
      (unsigned char)(min(maxWeightedIndexCb + distCb,255.0)),
      (unsigned char)(max(maxWeightedIndexCr - distCr,0.0)),
      (unsigned char)(min(maxWeightedIndexCr + distCr,255.0))
      );
  }//end set


  FrameInfo lastUpdated;

  FieldColorPercept()
    : distY(0.0),
      distCb(0.0),
      distCr(0.0),
      maxWeightedY(0.0),
      maxWeightedCb(0.0),
      maxWeightedCr(0.0),
      maxWeightedIndexY(0),
      maxWeightedIndexCb(0),
      maxWeightedIndexCr(0)
  {
  }

  ~FieldColorPercept()
  {}

  void init(FieldColorParameters& params)
  {
    distY = params.fieldcolorDistMax.y;
    distCb = params.fieldcolorDistMax.u;
    distCr = params.fieldcolorDistMax.v;
  }

  inline bool isFieldColor(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    return greenRegion.inside(yy, cb, cr);
    /*
    return
      (
        abs((int) cr  - (int) maxWeightedIndexCr) < (int) distCr
        &&          
        abs((int) cb  - (int) maxWeightedIndexCb) < (int) distCb
        &&
        abs((int) yy  - (int) maxWeightedIndexY) < (int) distY
      );
      */
  }

  inline bool isFieldColor(const Pixel& pixel) const
  {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }

  /*
  inline bool isOnlyFieldColor(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr, const ColorClasses::Color& color) const
  {
    return
      (
        isFieldColor(yy, cb, cr) &&
        color != ColorClasses::white &&
        color != ColorClasses::orange &&
        color != ColorClasses::skyblue &&
        color != ColorClasses::yellow
      );
  }
  
  inline bool isOnlyFieldColor(const Pixel& pixel, const ColorClasses::Color& color) const
  {
    return isOnlyFieldColor(pixel.y, pixel.u, pixel.v, color);
  }
  */

  inline void print(ostream& stream) const
  {
    stream << "distance in Y channel = " << distY << endl;
    stream << "distance in Cb channel = " << distCb << endl;
    stream << "distance in Cr channel = " << distCr << endl;
    stream << "max rate value in Y channel = " << maxWeightedY << endl;
    stream << "max rate value in Cb channel = " << maxWeightedCb << endl;
    stream << "max rate value in Cr channel = " << maxWeightedCr << endl;
    stream << "max value in Y channel = " << maxWeightedIndexY << endl;
    stream << "max value in Cb channel = " << maxWeightedIndexCb << endl;
    stream << "max value in Cr channel = " << maxWeightedIndexCr << endl;
  }//end print

};


#endif  /* FIELDCOLORPERCEPT_H */

