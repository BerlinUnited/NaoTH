/* 
 * File:   SimpleBallColorPercept.h
 * Author: claas
 *
 */

#ifndef SIMPLEBALLCOLORPERCEPT_H
#define SIMPLEBALLCOLORPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>

#include <Representations/Infrastructure/FrameInfo.h>

using namespace naoth;

class SimpleBallColorPercept : public naoth::Printable
{

public:
  bool valid;

  int maxY;
  int maxU;
  int minV;

  FrameInfo lastUpdated;

  SimpleBallColorPercept()  
  : 
  valid(false),
  maxY(200),
  maxU(160),
  minV(180)
  {}

  ~SimpleBallColorPercept()
  {}

  inline bool isInside(const Pixel& pixel) const
  {
    return pixel.v > minV && pixel.y < maxY && pixel.u < maxU;
  }

  inline void print(std::ostream& stream) const
  {
    stream << "min value in Cr (V) channel = " << minV << std::endl;
  }//end print

};


class SimpleBallColorPerceptTop : public SimpleBallColorPercept
{
public:
  virtual ~SimpleBallColorPerceptTop() {}
};

#endif  /* SIMPLEBALLCOLORPERCEPT_H */

