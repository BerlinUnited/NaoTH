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
  double Robotx = 0;
  double Roboty = 0;
  double Robotrot = 0;
  MODIFY("StaticDebugModelProvider:Robotx", Robotx);
  MODIFY("StaticDebugModelProvider:Roboty", Roboty);
  MODIFY("StaticDebugModelProvider:Robotrot", Robotrot);

  Pose2D pose;
  pose.translation.x = Robotx;
  pose.translation.y = Roboty;
  pose.rotation = Robotrot;

  getRobotPose() = pose;

  // update the goal model based on the robot pose
  getSelfLocGoalModel().update(getRobotPose(), getFieldInfo());
 
  //Ball
  double ballx =1000;
  double bally = -2000;
  MODIFY("StaticDebugModelProvider:ballx", ballx);
  MODIFY("StaticDebugModelProvider:bally", bally);

  getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());

  getBallModel().valid = 1;
  getBallModel().positionPreview.x = ballx;
  getBallModel().positionPreview.y = bally;


  //Draw Roboter
  FIELD_DRAWING_CONTEXT;
  PEN("FFFFFF", 20);
  ROBOT(pose.translation.x, pose.translation.y, pose.rotation);
}