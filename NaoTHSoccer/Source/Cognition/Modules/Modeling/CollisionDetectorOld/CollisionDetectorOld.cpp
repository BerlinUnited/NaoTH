
#include "CollisionDetectorOld.h"

CollisionDetectorOld::CollisionDetectorOld()
{
}

CollisionDetectorOld::~CollisionDetectorOld()
{

}

void CollisionDetectorOld::execute()
{
  if (getCollisionModel().isLeftFootColliding)
  {
    // do some filtering: wait at least 500ms until space is really free
    if (getFrameInfo().getTimeSince(collisionStartTimeLeft.getTime()) > 500)
    {
        getCollisionModel().isLeftFootColliding = false;
        collisionStartTimeLeft = getFrameInfo();
    }
  }

  if (getCollisionModel().isRightFootColliding)
  {
      // do some filtering: wait at least 500ms until space is really free
      if (getFrameInfo().getTimeSince(collisionStartTimeRight.getTime()) > 500)
      {
          getCollisionModel().isRightFootColliding = false;
          collisionStartTimeRight = getFrameInfo();
      }
  }
  
  if (!getCollisionModel().isLeftFootColliding && !getCollisionModel().isRightFootColliding)
  {
    // no collision yet, check if one occured
    // (and be sure it's really a collison)
    if( getButtonData().isPressed[ButtonData::RightFootLeft]
        || getButtonData().isPressed[ButtonData::RightFootRight])
    {
        getCollisionModel().isRightFootColliding = true;
        collisionStartTimeRight = getFrameInfo();
    }

    // no collision yet, check if one occured
    // (and be sure it's really a collison)
    if (getButtonData().isPressed[ButtonData::LeftFootLeft]
        || getButtonData().isPressed[ButtonData::LeftFootRight])
    {
        getCollisionModel().isLeftFootColliding = true;
        collisionStartTimeLeft = getFrameInfo();
    }
   }
}
