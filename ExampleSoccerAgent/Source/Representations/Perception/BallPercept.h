/** 
 * @file BallPercept.h
 *
 * Declaration of class BallPercept
 */

#ifndef __BallPercept_h_
#define __BallPercept_h_

#include <Tools/Math/Vector2.h>

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Representations/Infrastructure/FrameInfo.h>


class BallPercept : public naoth::Printable
{
public:
  BallPercept(): 
    ballWasSeen(false),
    radiusInImage(0.0)
  {}

  /* indicates wether a ball was seen, i.e., the percept is valid */
  bool ballWasSeen;

  /* center of ball in image coordinates */
  Vector2<double> centerInImage;

  /* radius of ball in image */
  double radiusInImage;

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
  }//end print
};
/*
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
*/
#endif //__BallPercept_h_


