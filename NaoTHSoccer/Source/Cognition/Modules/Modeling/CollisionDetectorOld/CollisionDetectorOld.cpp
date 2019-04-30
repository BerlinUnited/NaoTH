
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
      if (!getButtonData().isPressed[ButtonData::LeftFootLeft]
          && !getButtonData().isPressed[ButtonData::LeftFootRight]
          && (getFrameInfo().getTimeSince(lastBumpTimeLeft.getTime()) > params.collisionInterval))
      {
          getCollisionModel().isLeftFootColliding = false;
          collisionStartTimeLeft = getFrameInfo();
      }
  }

  if (getCollisionModel().isRightFootColliding)
  {
      if (!getButtonData().isPressed[ButtonData::RightFootLeft]
          && !getButtonData().isPressed[ButtonData::RightFootRight]
          && (getFrameInfo().getTimeSince(lastBumpTimeRight.getTime()) > params.collisionInterval))
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
  //Now we want to distinguish single bumper presses from "real" collisions
  if ((getFrameInfo().getTimeSince(collisionStartTimeLeft.getTime()) > params.collisionInterval*params.timesToBump)
      &&getCollisionModel().isLeftFootColliding)
  {
      //Left bumper collision -> evasive movement
      getCollisionModel().lastComputedCollisionLeft = getFrameInfo();
      getCollisionModel().lastComputedCollisionLeftFrame = getFrameInfo().getFrameNumber();

      getCollisionModel().isLeftFootColliding = false;
      collisionStartTimeLeft = getFrameInfo();
  }
  if ((getFrameInfo().getTimeSince(collisionStartTimeRight.getTime()) > params.collisionInterval*params.timesToBump)
      &&getCollisionModel().isRightFootColliding)
  {
      //Right bumper collision -> evasive movement
      getCollisionModel().lastComputedCollisionRight = getFrameInfo();
      getCollisionModel().lastComputedCollisionRightFrame = getFrameInfo().getFrameNumber();

      getCollisionModel().isRightFootColliding = false;
      collisionStartTimeRight = getFrameInfo();
  }
}
