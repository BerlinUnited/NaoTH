/* 
 * File:   FieldColorPercept.h
 * Author: claas
 *
 * Created on 17. März 2011, 22:53
 */

#ifndef FIELDCOLORPERCEPT_H
#define  FIELDCOLORPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/ImageProcessing/FieldColorParameters.h>

#include <Representations/Infrastructure/FrameInfo.h>


using namespace naoth;

class FieldColorPercept : public naoth::Printable
{
  
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

  FrameInfo lastUpdated;

  FieldColorPercept()
  {
    maxWeightedY = 0.0;
    maxWeightedCb = 0.0;
    maxWeightedCr = 0.0;
    maxWeightedIndexY = 160;
    maxWeightedIndexCb = 160;
    maxWeightedIndexCr = 160;
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
    return
      (
//        yy > theParameters.fieldColorMin.y
//        &&
//        yy < theParameters.fieldColorMax.y
//        &&
//        cb > theParameters.fieldColorMin.u
//        &&
//        cb < theParameters.fieldColorMax.u
//        &&
//        cr > theParameters.fieldColorMin.v
//        &&
//        cr < theParameters.fieldColorMax.v
//        &&
        abs((int) cr  - (int) maxWeightedIndexCr) < (int) distCr
        &&          
        abs((int) cb  - (int) maxWeightedIndexCb) < (int) distCb
        &&
        abs((int) yy  - (int) maxWeightedIndexY) < (int) distY
      );
  }

  inline bool isFieldColor(const Pixel& pixel) const
  {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }


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

