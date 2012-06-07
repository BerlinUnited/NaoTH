#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include "Tools/DataStructures/Printable.h"

class CollisionModel : public naoth::Printable
{
public:
  CollisionModel();
  virtual ~CollisionModel();

  virtual void print(ostream &stream) const;

  bool isColliding;

};

#endif // COLLISIONMODEL_H
