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

  naoth::FrameInfo collisionStartTime;

  bool isColliding;
  bool isLeftFootColliding;
  bool isRightFootColliding;


};

#endif // COLLISIONMODEL_H
