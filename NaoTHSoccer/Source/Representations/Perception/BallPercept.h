/** 
 * @file BallPercept.h
 *
 * Declaration of class BallPercept
 */

#ifndef __BallPercept_h_
#define __BallPercept_h_

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>
#include "Tools/ColorClasses.h"

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Representations/Infrastructure/FrameInfo.h>


class BallPercept : public naoth::Printable
{
public:
  BallPercept(): 
    ballWasSeen(false),
    radiusInImage(0.0),
    ballColor(ColorClasses::orange)
  {}

  /* indicates wether a ball was seen, i.e., the percept is valid */
  bool ballWasSeen;

  /* center of ball in image coordinates */
  Vector2d centerInImage;

  /* radius of ball in image */
  double radiusInImage;

  ColorClasses::Color ballColor;

  /* relative ball position to the robot based on the camera matrix */
  Vector2d bearingBasedOffsetOnField; 

  /* relative ball position to the robot based on the seen size of the ball */
  Vector3<double> sizeBasedRelativePosition; 
  
  naoth::FrameInfo frameInfoWhenBallWasSeen;

  /* reset percept */
  void reset()
  {
    ballWasSeen = false;
  }//end reset

  virtual void print(std::ostream& stream) const
  {
    stream << "ballWasSeen = " << ballWasSeen << std::endl;
    stream << "centerInImage = " << centerInImage << std::endl;
    stream << "radiusInImage = " << radiusInImage << std::endl;
    stream << "bearingBasedOffsetOnField = " << bearingBasedOffsetOnField << std::endl;
    stream << "frameInfoWhenBallWasSeen:" << frameInfoWhenBallWasSeen << std::endl;
    stream << "ball color:\n" << ColorClasses::getColorName(ballColor) << std::endl;
  }//end print
};

class BallPerceptTop : public BallPercept
{
public:
  virtual ~BallPerceptTop() {}
};

namespace naoth
{
  template<>
  class Serializer<BallPercept>
  {
  public:
    static void serialize(const BallPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BallPercept& representation);
  };

  template<>
  class Serializer<BallPerceptTop> : public Serializer<BallPercept>
  {};
}


#endif //__BallPercept_h_


