
#include "CollisionDetectorOld.h"

CollisionDetectorOld::CollisionDetectorOld()
{
    getDebugParameterList().add(&params);
}

CollisionDetectorOld::~CollisionDetectorOld()
{
    getDebugParameterList().remove(&params);
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
    if (getFrameInfo().getTimeSince(collisionStartTimeLeft.getTime()) > 500)
    {
        //Only reset the representation to false when there is no bumper pressed
        if (!getButtonData().isPressed[ButtonData::LeftFootLeft]
            && !getButtonData().isPressed[ButtonData::LeftFootRight]
            && (getFrameInfo().getTimeSince(lastBumpTimeLeft.getTime()) > bumpInterval))
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
              && !getButtonData().isPressed[ButtonData::RightFootRight]
              && (getFrameInfo().getTimeSince(lastBumpTimeRight.getTime()) > bumpInterval))
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
  if ((getFrameInfo().getTimeSince(collisionStartTimeLeft.getTime()) > params.collisionInterval)
      &&getCollisionModel().isLeftFootColliding)
  {
      //Left bumper collision -> evasive movement
      getCollisionModel().lastComputedCollisionLeft = getFrameInfo();
      getCollisionModel().isLeftFootColliding = false;
      collisionStartTimeLeft = getFrameInfo();
  }
  if ((getFrameInfo().getTimeSince(collisionStartTimeRight.getTime()) > params.collisionInterval)
      &&getCollisionModel().isRightFootColliding)
  {
      //Right bumper collision -> evasive movement
      getCollisionModel().lastComputedCollisionRight = getFrameInfo();
      getCollisionModel().isRightFootColliding = false;
      collisionStartTimeRight = getFrameInfo();
  }
}
