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
private:

public:
  bool valid;
  double distU;
  int indexU;

  int borderLeftU;
  int borderRightU;

  int maxV;

  FrameInfo lastUpdated;

  FieldColorPercept()  
  : 
  valid(false),
  distU(5.0),
  indexU(100),
  borderLeftU(0),
  borderRightU(0),
  maxV(127)
  {}

  ~FieldColorPercept()
  {}

  void set()
  {
    borderLeftU = (int) Math::clamp<double>(indexU - distU, 0.0, 255.0);
    borderRightU = (int) Math::clamp<double>(indexU + distU, 0.0, 255.0);
  }

  inline bool isFieldColor(const int& yy, const int& cb, const int& cr) const
  {
    return borderLeftU < cr && cr < borderRightU && cb < maxV;
  }

  inline bool isFieldColor(const Pixel& pixel) const
  {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }

  inline void print(ostream& stream) const
  {
    stream << "distance in Cr (U) channel = " << distU << endl;
    stream << "index in Cr (U) channel = " << indexU << endl;
  }//end print

};


#endif  /* FIELDCOLORPERCEPT_H */

