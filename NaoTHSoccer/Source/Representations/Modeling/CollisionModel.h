#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include "Tools/DataStructures/Printable.h"

#include <Representations/Infrastructure/FrameInfo.h>

class CollisionModel : public naoth::Printable
{
public:
  CollisionModel();
  virtual ~CollisionModel();

  virtual void print(std::ostream &stream) const;

  bool isLeftFootColliding;
  bool isRightFootColliding;

  // Why?
  naoth::FrameInfo lastComputedCollisionLeft;
  naoth::FrameInfo lastComputedCollisionRight;

  //
  bool collision_left_bumber;
  bool collision_right_bumber;

};

#endif // COLLISIONMODEL_H
