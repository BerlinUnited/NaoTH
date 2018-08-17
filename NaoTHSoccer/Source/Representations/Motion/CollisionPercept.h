/**
* @file CollisionPercept.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class CollisionPercept
*/

#ifndef __CollisionPercept_h_
#define __CollisionPercept_h_

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include <vector>
#include "Tools/DataStructures/Point.h"

/**
* This describes the CollisionPercept
*/
class CollisionPercept : public naoth::Printable
{
public:
  CollisionPercept()
    : 
    timeCollisionArmLeft(0), 
    timeCollisionArmRight(0)
  {}

  ~CollisionPercept(){}

  // time stamp of the last collision
  unsigned int timeCollisionArmLeft;
  unsigned int timeCollisionArmRight;

  std::vector<Point> referenceHull;
  std::vector<Point> pointBufferLeft;
  std::vector<Point> pointBufferRight;
  std::vector<Point> newHullLeft;
  std::vector<Point> newHullRight;

  
  virtual void print(std::ostream& stream) const
  {
    stream << "timeCollisionArmLeft = " << timeCollisionArmLeft << '\n';
    stream << "timeCollisionArmRight = " << timeCollisionArmRight << '\n';
    stream << "referenceHull:" << "\n";
    for (size_t i = 0; i < referenceHull.size(); i++){
      stream << "x= " << referenceHull[i].getX() << " y = " << referenceHull[i].getY() << "\n";
    }
	stream << "Left Point Buffer:" << "\n";
	for (size_t i = 0; i < pointBufferLeft.size(); i++){
		stream << "x= " << pointBufferLeft[i].getX() << " y = " << pointBufferLeft[i].getY() << "\n";
	}
	stream << "Right Point Buffer:" << "\n";
	for (size_t i = 0; i < pointBufferRight.size(); i++){
		stream << "x= " << pointBufferRight[i].getX() << " y = " << pointBufferRight[i].getY() << "\n";
	}
  }
};

namespace naoth
{
  template<>
  class Serializer<CollisionPercept>
  {
  public:
    static void serialize(const CollisionPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, CollisionPercept& representation);
  };
}

#endif // __CollisionPercept_h_
