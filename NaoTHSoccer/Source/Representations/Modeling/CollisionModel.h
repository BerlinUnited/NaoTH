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
  naoth::FrameInfo lastComputedCollisionLeft;
  naoth::FrameInfo lastComputedCollisionRight;


};

#endif // COLLISIONMODEL_H
