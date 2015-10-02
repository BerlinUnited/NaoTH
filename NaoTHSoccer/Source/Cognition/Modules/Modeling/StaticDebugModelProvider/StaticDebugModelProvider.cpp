/**
* @file StaticDebugModelProvider.cpp
*/

#include "StaticDebugModelProvider.h"

StaticDebugModelProvider::StaticDebugModelProvider()
{

}

StaticDebugModelProvider::~StaticDebugModelProvider()
{
}

void StaticDebugModelProvider::execute()
{
  //Roboter
  Pose2D pose;
  double rotation = 0;
  MODIFY("StaticDebugModelProvider:Robotx", pose.translation.x);
  MODIFY("StaticDebugModelProvider:Roboty", pose.translation.y);
  MODIFY("StaticDebugModelProvider:Robotrot", rotation);
  pose.rotation = Math::fromDegrees(rotation);
  getRobotPose() = pose;

  // update the goal model based on the robot pose
  getSelfLocGoalModel().update(getRobotPose(), getFieldInfo());
 
  //Ball
  Vector2d ballPosition(1000, -2000);
  MODIFY("StaticDebugModelProvider:ballx", ballPosition.x);
  MODIFY("StaticDebugModelProvider:bally", ballPosition.y);

  getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());

  getBallModel().valid = true;
  getBallModel().positionPreview = ballPosition;


  //Draw Roboter
  FIELD_DRAWING_CONTEXT;
  PEN("FFFFFF", 20);
  ROBOT(pose.translation.x, pose.translation.y, pose.rotation);
}