
#include "CollisionModel.h"

CollisionModel::CollisionModel()
{
}

CollisionModel::~CollisionModel()
{

}


void CollisionModel::print(std::ostream &stream) const
{
    stream << "isLeftFootColliding=" << (isLeftFootColliding ? "true" : "false") << std::endl;
    stream << "isRightFootColliding=" << (isRightFootColliding ? "true" : "false") << std::endl;
    stream << "Last computed collision Left: " << lastComputedCollisionLeft.getFrameNumber() << std::endl;
    stream << "Last computed collision Right: " << lastComputedCollisionRight.getFrameNumber() << std::endl;
}
