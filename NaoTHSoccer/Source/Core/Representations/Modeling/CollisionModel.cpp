
#include "CollisionModel.h"

CollisionModel::CollisionModel()
  : isColliding(false)
{
}

CollisionModel::~CollisionModel()
{

}


void CollisionModel::print(ostream &stream) const
{
  stream << "isColliding=" << (isColliding ? "true" : "false") << std::endl;
  if(isColliding)
  {
    stream << "collision start " << collisionStartTime.getTimeInSeconds()
           << std::endl;
  }
}
