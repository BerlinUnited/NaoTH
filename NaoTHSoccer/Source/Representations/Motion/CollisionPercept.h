/**
* @file CollisionPercept.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class CollisionPercept
*/

#ifndef CollisionPercept_h
#define CollisionPercept_h

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include <vector>
#include "Tools/Math/Vector2.h"
#include <Representations/Infrastructure/FrameInfo.h>

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

  std::string lastCollisionDirection;

  std::vector<Vector2d> referenceHullLeft;
  std::vector<Vector2d> referenceHullRight;
  std::vector<Vector2d> pointBufferLeft;
  std::vector<Vector2d> pointBufferRight;
  std::vector<Vector2d> newHullLeft;
  std::vector<Vector2d> newHullRight;

  //stuff for bumper collision
  bool isLeftFootColliding;
  bool isRightFootColliding;
  double lastComputedCollisionLeft;
  double lastComputedCollisionRight;
  bool collision_left_bumper;
  bool collision_right_bumper;
  
  virtual void print(std::ostream& stream) const
  {
    stream << "Last collision direction: " << lastCollisionDirection << std::endl;
    stream << "timeCollisionArmLeft = " << timeCollisionArmLeft << '\n';
    stream << "timeCollisionArmRight = " << timeCollisionArmRight << '\n';
    stream << "referenceHullLeft: " << "\n";
    for (size_t i = 0; i < referenceHullLeft.size(); i++){
      stream << "x= " << referenceHullLeft[i].x << " y = " << referenceHullLeft[i].y << "\n";
    }
	stream << "referenceHullRight: " << "\n";
	for (size_t i = 0; i < referenceHullRight.size(); i++){
		stream << "x= " << referenceHullRight[i].x << " y = " << referenceHullRight[i].y << "\n";
	}
    stream << "isLeftFootColliding=" << (isLeftFootColliding ? "true" : "false") << std::endl;
    stream << "isRightFootColliding=" << (isRightFootColliding ? "true" : "false") << std::endl;
    stream << "Last computed collision Left: " << lastComputedCollisionLeft << std::endl;
    stream << "Last computed collision Right: " << lastComputedCollisionRight << std::endl;
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

#endif // CollisionPercept_h
