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
  getObstacleModel().blockedTime = 0;
  getObstacleModel().frontDistance = 2550.0;

  //Roboter
  Pose2D pose;
  double rotation = 0;
  pose.translation.x = 1000.0;
  pose.translation.y = -2000.0;
  MODIFY("StaticDebugModelProvider:Robotx", pose.translation.x);
  MODIFY("StaticDebugModelProvider:Roboty", pose.translation.y);
  MODIFY("StaticDebugModelProvider:Robotrot", rotation);
  pose.rotation = Math::fromDegrees(rotation);
  getRobotPose() = pose;

  // update the goal model based on the robot pose
  getSelfLocGoalModel().update(getRobotPose(), getFieldInfo());
 
  //Ball
  Vector2d ballPosition(0.0, 0.0);
  MODIFY("StaticDebugModelProvider:ballx", ballPosition.x);
  MODIFY("StaticDebugModelProvider:bally", ballPosition.y);

  getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());

  getBallModel().valid = true;
  getBallModel().positionPreview = ballPosition;

  //Set Obstacle Model
  double obstacles = 0.0;
  MODIFY("StaticDebugModelProvider:Obstacles", obstacles);
  if(obstacles != 0.0){
    getObstacleModel().frontDistance = 399;
    getObstacleModel().blockedTime = 101;
  }
  //Draw Roboter
  FIELD_DRAWING_CONTEXT;
  PEN("FFFFFF", 20);
  ROBOT(pose.translation.x, pose.translation.y, pose.rotation);
}