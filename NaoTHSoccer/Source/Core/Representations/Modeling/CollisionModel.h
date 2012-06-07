#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include "Tools/DataStructures/Printable.h"

#include <Representations/Infrastructure/FrameInfo.h>

class CollisionModel : public naoth::Printable
{
public:
  CollisionModel();
  virtual ~CollisionModel();

  virtual void print(ostream &stream) const;

  naoth::FrameInfo collisionStartTime;
  bool isColliding;

};

#endif // COLLISIONMODEL_H
