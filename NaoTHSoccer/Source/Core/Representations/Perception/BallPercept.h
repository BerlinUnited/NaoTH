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
  Vector2<double> centerInImage;

  /* radius of ball in image */
  double radiusInImage;

  ColorClasses::Color ballColor;

  /* relative ball position to the robot based on the camera matrix */
  Vector2<double> bearingBasedOffsetOnField; 

  /* relative ball position to the robot based on the seen size of the ball */
  Vector3<double> sizeBasedRelativePosition; 
  
  naoth::FrameInfo frameInfoWhenBallWasSeen;

  /* reset percept */
  void reset()
  {
    ballWasSeen = false;
  }//end reset

  virtual void print(ostream& stream) const
  {
    stream << "ballWasSeen = " << ballWasSeen << endl;
    stream << "centerInImage = " << centerInImage << endl;
    stream << "radiusInImage = " << radiusInImage << endl;
    stream << "bearingBasedOffsetOnField = " << bearingBasedOffsetOnField << endl;
    //stream << "frameInfoWhenBallWasSeen:" << frameInfoWhenBallWasSeen << endl;
    stream << "ball color:\n" << ColorClasses::getColorName(ballColor) << endl;
  }//end print
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
}

#endif //__BallPercept_h_


