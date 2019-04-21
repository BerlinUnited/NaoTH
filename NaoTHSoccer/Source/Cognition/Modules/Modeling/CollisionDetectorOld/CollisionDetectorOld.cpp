
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
        //Only reset the representation to false when there is no bumper pressed
        if (!getButtonData().isPressed[ButtonData::LeftFootLeft]
            && !getButtonData().isPressed[ButtonData::LeftFootRight])
        {
            getCollisionModel().isLeftFootColliding = false;
            collisionStartTimeLeft = getFrameInfo();
        }
    }
  }

  if (getCollisionModel().isRightFootColliding)
  {
      // do some filtering: wait at least 500ms until space is really free
      if (getFrameInfo().getTimeSince(collisionStartTimeRight.getTime()) > 500)
      {
          //Only reset the representation to false when there is no bumper pressed
          if (!getButtonData().isPressed[ButtonData::RightFootLeft]
              && !getButtonData().isPressed[ButtonData::RightFootRight])
          {
              getCollisionModel().isRightFootColliding = false;
              collisionStartTimeRight = getFrameInfo();
          }
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
  //Now we want to distinguish single bumper presses from "real" collisions
  //First "naive" implementation and very high 3000ms threshold (to check functionnality)
  if (getCollisionModel().isLeftFootColliding > 3000)
  {
      //Left bumper collision -> evasive movement
      getCollisionModel().isLeftFootColliding = false;
      collisionStartTimeLeft = getFrameInfo();
  }
  if (getCollisionModel().isRightFootColliding > 3000)
  {
      //Right bumper collision -> evasive movement
      getCollisionModel().isRightFootColliding = false;
      collisionStartTimeRight = getFrameInfo();
  }
}
