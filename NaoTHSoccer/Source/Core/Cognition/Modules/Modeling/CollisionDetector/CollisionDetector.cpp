
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
  double allowedRobotDistance = 400;
  MODIFY("CollisionDetector:allowedRobotDistance", allowedRobotDistance);

  if(getCollisionModel().isColliding)
  {
    // we were colliding in the last frame

    // no near robot in front of us any longer?
    if(getUltraSoundReceiveData().rawdata >= allowedRobotDistance)
    {
      getCollisionModel().isColliding = false;
      getCollisionModel().collisionStartTime = getFrameInfo();
    }

  }
  else
  {
    // no collision yet, check if one occured
    // (and be sure it's really a collison)

    if(getUltraSoundReceiveData().rawdata < allowedRobotDistance
       && (
         getButtonData().isPressed[ButtonData::LeftFootLeft]
         || getButtonData().isPressed[ButtonData::LeftFootRight]
         || getButtonData().isPressed[ButtonData::RightFootLeft]
         || getButtonData().isPressed[ButtonData::RightFootRight]
       ))
    {
      getCollisionModel().isColliding = true;
      getCollisionModel().collisionStartTime = getFrameInfo();
    }

  }
}
