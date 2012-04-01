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

public:
  bool valid;
  double distU;
  int indexU;

  FrameInfo lastUpdated;

  FieldColorPercept()  
  : 
  valid(false),
  distU(5.0),
  indexU(100)
  {}

  ~FieldColorPercept()
  {}

  inline bool isFieldColor(const int& yy, const int& cb, const int& cr) const
  {
    return abs(cr - indexU) < distU;
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

