#include "BumperCollisionDetector.h"

BumperCollisionDetector::BumperCollisionDetector()
{
    getDebugParameterList().add(&params);
}

BumperCollisionDetector::~BumperCollisionDetector()
{
    getDebugParameterList().remove(&params);
}

void BumperCollisionDetector::execute()
{   
    // record time when on of the bumper sides where pressed on the left foot
    if (getButtonData().isPressed[ButtonData::LeftFootLeft] ||
        getButtonData().isPressed[ButtonData::LeftFootRight]) {
        lastBumpTimeLeft = getFrameInfo();
    }

    // record time when on of the bumper sides where pressed on the right foot
    if (getButtonData().isPressed[ButtonData::RightFootLeft]
        || getButtonData().isPressed[ButtonData::RightFootRight])
    {
        lastBumpTimeRight = getFrameInfo();
    }

    // reset the colliding bool if buttons are not pressed and the time since the last press/collision is too long ago
    if (getCollisionPercept().isLeftFootColliding){
      if (!getButtonData().isPressed[ButtonData::LeftFootLeft]
          && !getButtonData().isPressed[ButtonData::LeftFootRight]
          && (getFrameInfo().getTimeSince(lastBumpTimeLeft.getTime()) > params.collisionInterval))
      {
          getCollisionPercept().isLeftFootColliding = false;
          collisionStartTimeLeft = getFrameInfo();
      }
    }

    // reset the colliding bool if buttons are not pressed and the time since the last press/collision is too long ago
    if (getCollisionPercept().isRightFootColliding){
        if (!getButtonData().isPressed[ButtonData::RightFootLeft]
            && !getButtonData().isPressed[ButtonData::RightFootRight]
            && (getFrameInfo().getTimeSince(lastBumpTimeRight.getTime()) > params.collisionInterval))
        {
            getCollisionPercept().isRightFootColliding = false;
            collisionStartTimeRight = getFrameInfo();
        }
    }
  
    if (!getCollisionPercept().isLeftFootColliding && !getCollisionPercept().isRightFootColliding){
        // no collision yet, check if one occured
        // (and be sure it's really a collison)
        if( getButtonData().isPressed[ButtonData::RightFootLeft]
            || getButtonData().isPressed[ButtonData::RightFootRight])
        {
            getCollisionPercept().isRightFootColliding = true;
            collisionStartTimeRight = getFrameInfo();
        }

        // no collision yet, check if one occured
        // (and be sure it's really a collison)
        if (getButtonData().isPressed[ButtonData::LeftFootLeft]
            || getButtonData().isPressed[ButtonData::LeftFootRight])
        {
            getCollisionPercept().isLeftFootColliding = true;
            collisionStartTimeLeft = getFrameInfo();
        }
    }

  //Now we want to distinguish single bumper presses from "real" collisions
  if ((getFrameInfo().getTimeSince(collisionStartTimeLeft.getTime()) > params.collisionInterval*params.timesToBump)
      && getCollisionPercept().isLeftFootColliding)
  {
      //Left bumper collision
      getCollisionPercept().lastComputedCollisionLeft = getFrameInfo().getTimeInSeconds();
      getCollisionPercept().collision_left_bumper = true;

      getCollisionPercept().isLeftFootColliding = false;
      collisionStartTimeLeft = getFrameInfo();
  }
  else {
      getCollisionPercept().collision_left_bumper = false;
  }

  if ((getFrameInfo().getTimeSince(collisionStartTimeRight.getTime()) > params.collisionInterval*params.timesToBump)
      && getCollisionPercept().isRightFootColliding)
  {
      //Right bumper collision
      getCollisionPercept().lastComputedCollisionRight = getFrameInfo().getTimeInSeconds();
      getCollisionPercept().collision_right_bumper = true;

      getCollisionPercept().isRightFootColliding = false;
      collisionStartTimeRight = getFrameInfo();
  }
  else {
      getCollisionPercept().collision_right_bumper = false;
  }
}
