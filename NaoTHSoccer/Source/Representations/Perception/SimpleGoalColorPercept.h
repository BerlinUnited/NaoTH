/* 
 * File:   SimpleGoalColorPercept.h
 * Author: claas
 *
 */

#ifndef SIMPLEGOALCOLORPERCEPT_H
#define SIMPLEGOALCOLORPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>

#include <Representations/Infrastructure/FrameInfo.h>

using namespace naoth;

class SimpleGoalColorPercept : public naoth::Printable
{

public:
  bool valid;

  int maxY;
  int maxU;

  int maxDistV;

  int minY;
  int minV;

  int vu_offset;

  FrameInfo lastUpdated;


  SimpleGoalColorPercept()  
    : 
    valid(false),
    maxY(200),
    maxU(64),
    maxDistV(64),
    minY(80),
    minV(127),
    vu_offset(45)
  {}

  ~SimpleGoalColorPercept()
  {}

  inline bool isInside(const Pixel& pixel) const
  {
    /*
    bool isGoal = 
      pixel.v > pixel.u + vu_offset && // difference in the v-u channel 
      pixel.u < maxU &&

      pixel.v > minV && 
      pixel.v < minV + maxDistV &&

      pixel.y < maxY &&
      pixel.y > minY;
      */

    //isGoal = isGoal || (maxFieldU < pixel.u &&
    //  minFieldV <= pixel.v && pixel.v <= maxFieldV);

    bool isGoal = 
      pixel.v > pixel.u + vu_offset; // difference in the v-u channel 

    return isGoal;
  }

  inline void print(std::ostream& stream) const
  {
    stream << "max value in Y channel = " << maxY << std::endl;
    stream << "min value in Y channel = " << minY << std::endl;
    stream << "max value in Cb (U) channel = " << maxU << std::endl;
    stream << "min value in Cr (V) channel = " << minV << std::endl;
    stream << "max distance in Cr (V) channel = " << maxDistV << std::endl;
  }//end print

};

class SimpleGoalColorPerceptTop : public SimpleGoalColorPercept
{
public:
  virtual ~SimpleGoalColorPerceptTop() {}
};

#endif  /* SIMPLEGOALCOLORPERCEPT_H */

