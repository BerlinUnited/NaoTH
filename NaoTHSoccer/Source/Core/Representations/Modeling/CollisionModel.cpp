
#include "CollisionModel.h"

CollisionModel::CollisionModel()
{
}

CollisionModel::~CollisionModel()
{

}


void CollisionModel::print(ostream &stream) const
{
  stream << "isColliding=" << (isColliding ? "true" : "false") << std::endl;
}
