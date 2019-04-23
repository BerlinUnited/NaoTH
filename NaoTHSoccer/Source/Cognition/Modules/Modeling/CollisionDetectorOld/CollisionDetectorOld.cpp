
#include "CollisionDetectorOld.h"

CollisionDetectorOld::CollisionDetectorOld()
{
}

CollisionDetectorOld::~CollisionDetectorOld()
{

}

void CollisionDetectorOld::execute()
{
    if (getButtonData().isPressed[ButtonData::RightFootLeft]
        || getButtonData().isPressed[ButtonData::RightFootRight])
    {
        lastBumpTimeRight = getFrameInfo();
    }
    if (getButtonData().isPressed[ButtonData::LeftFootLeft]
        || getButtonData().isPressed[ButtonData::LeftFootRight])
    {
        lastBumpTimeLeft = getFrameInfo();
    }

  if (getCollisionModel().isLeftFootColliding)
  {
    // do some filtering: wait at least 500ms until space is really free
    if (getFrameInfo().getTimeSince(collisionStartTimeLeft.getTime()) > 1000)
    {
        //Only reset the representation to false when there is no bumper pressed
        if (!getButtonData().isPressed[ButtonData::LeftFootLeft]
            && !getButtonData().isPressed[ButtonData::LeftFootRight]
            && (getFrameInfo().getTimeSince(lastBumpTimeLeft.getTime()) > 500))
        {
            getCollisionModel().isLeftFootColliding = false;
            collisionStartTimeLeft = getFrameInfo();
        }
    }
  }

  if (getCollisionModel().isRightFootColliding)
  {
      // do some filtering: wait at least 500ms until space is really free
      if (getFrameInfo().getTimeSince(collisionStartTimeRight.getTime()) > 1000)
      {
          //Only reset the representation to false when there is no bumper pressed
          if (!getButtonData().isPressed[ButtonData::RightFootLeft]
              && !getButtonData().isPressed[ButtonData::RightFootRight]
              && (getFrameInfo().getTimeSince(lastBumpTimeRight.getTime()) > 500))
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
  //First "naive" implementation and very high ms threshold (to check functionnality)
  if ((getFrameInfo().getTimeSince(collisionStartTimeLeft.getTime()) > 2000)
      &&getCollisionModel().isLeftFootColliding)
  {
      //Left bumper collision -> evasive movement
      getCollisionModel().isLeftFootColliding = false;
      std::cout << "*************** LEFT BUMPER COLLISION ******************" << std::endl;
      collisionStartTimeLeft = getFrameInfo();
  }
  if ((getFrameInfo().getTimeSince(collisionStartTimeRight.getTime()) > 2000)
      &&getCollisionModel().isRightFootColliding)
  {
      //Right bumper collision -> evasive movement
      getCollisionModel().isRightFootColliding = false;
      std::cout << "*************** RIGHT BUMPER COLLISION ******************" << std::endl;
      collisionStartTimeRight = getFrameInfo();
  }
}
