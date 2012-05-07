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
public:
  bool valid;
  double distV;
  int indexV;

  int borderLeftV;
  int borderRightV;

  int maxY;
  int maxU;

  double distV2Set;
  int maxY2Set;
  int maxU2Set;

  FrameInfo lastUpdated;

  FieldColorPercept()  
  : 
  valid(false),
  distV(5.0),
  indexV(100),
  borderLeftV(0),
  borderRightV(0),
  maxY(127),
  maxU(127),
  distV2Set(distV),
  maxY2Set(maxY),
  maxU2Set(maxU)
  {}

  ~FieldColorPercept()
  {}

  void set()
  {
    borderLeftV = (int) Math::clamp<double>(indexV - distV, 0.0, 255.0);
    borderRightV = (int) Math::clamp<double>(indexV + distV, 0.0, 255.0);
  }

  inline bool isFieldColor(const int& yy, const int& cb, const int& cr) const
  {
    return borderLeftV < cr && cr < borderRightV && cb < maxU && yy < maxY;
  }

  inline bool isFieldColor(const Pixel& pixel) const
  {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }

  inline void print(ostream& stream) const
  {
    stream << "distance in Cr (V) channel = " << distV << endl;
    stream << "index in Cr (V) channel = " << indexV << endl;
  }//end print

};


#endif  /* FIELDCOLORPERCEPT_H */

