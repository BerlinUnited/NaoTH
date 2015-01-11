
#include "CollisionDetector.h"

#include <Tools/Debug/DebugModify.h>

CollisionDetector::CollisionDetector()
{
}

CollisionDetector::~CollisionDetector()
{

}

void CollisionDetector::execute()
{
  double allowedRobotDistance = 0.3;
  MODIFY("CollisionDetector:allowedRobotDistance", allowedRobotDistance);

  if(getCollisionModel().isColliding)
  {
    // we were colliding in the last frame

    // no near robot in front of us any longer?
    if(getUltraSoundReceiveData().dataLeft[0] >= allowedRobotDistance
       || getUltraSoundReceiveData().dataRight[0] >= allowedRobotDistance)
    {
      // do some filtering: wait at least 500ms until space is really free
      if(getFrameInfo().getTimeSince(timeSinceFree.getTime()) > 500)
      {
        getCollisionModel().isColliding = false;
        getCollisionModel().collisionStartTime = getFrameInfo();
      }
    }
    else
    {
      // reset timer if something got too near
      timeSinceFree = getFrameInfo();
    }

  }
  else
  {
    // no collision yet, check if one occured
    // (and be sure it's really a collison)

    if(
       getUltraSoundReceiveData().dataLeft[0] < allowedRobotDistance
       && getUltraSoundReceiveData().dataRight[0] < allowedRobotDistance
       && (
         getButtonData().isPressed[ButtonData::LeftFootLeft]
         || getButtonData().isPressed[ButtonData::LeftFootRight]
         || getButtonData().isPressed[ButtonData::RightFootLeft]
         || getButtonData().isPressed[ButtonData::RightFootRight]
       ))
    {
      getCollisionModel().isColliding = true;
      getCollisionModel().collisionStartTime = getFrameInfo();
      timeSinceFree = getFrameInfo();
    }

  }
}
