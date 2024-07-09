/**
* @file StaticDebugModelProvider.cpp
*/

#include "StaticDebugModelProvider.h"

StaticDebugModelProvider::StaticDebugModelProvider()
{
  DEBUG_REQUEST_REGISTER("StaticDebugModelProvider:drawBallOnFieldGlobal", "", false);
  DEBUG_REQUEST_REGISTER("StaticDebugModelProvider:drawRobotOnField", "", false);
}

void StaticDebugModelProvider::execute()
{
  getObstacleModel().blockedTime = 0;
  getObstacleModel().frontDistance = 2550.0;

  //Roboter
  Pose2D pose(0.0, 1000.0, -2000.0);
  pose.translation.x = 1000.0;
  pose.translation.y = -2000.0;
  MODIFY("StaticDebugModelProvider:RobotPose:x", pose.translation.x);
  MODIFY("StaticDebugModelProvider:RobotPose:y", pose.translation.y);

  double rotation = 0;
  MODIFY("StaticDebugModelProvider:RobotPose:rotation", rotation);
  pose.rotation = Math::fromDegrees(rotation);

  getRobotPose() = pose;

  // update the goal model based on the robot pose
  getSelfLocGoalModel().update(getRobotPose(), getFieldInfo());
 
  //Ball
  Vector2d ballPosition(0.0, 0.0);
  MODIFY("StaticDebugModelProvider:Ball:x", ballPosition.x);
  MODIFY("StaticDebugModelProvider:Ball:y", ballPosition.y);

  getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());
  getBallModel().valid = true;

  // set ball model representation
  getBallModel().position = ballPosition;

  //set preview ball model representation
  getBallModel().positionPreview = ballPosition;

  const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;

  // transform ball model into feet coordinates
  Vector2d ballLeftFoot = lFoot.projectXY() / getBallModel().position;
  Vector2d ballRightFoot = rFoot.projectXY() / getBallModel().position;

  getBallModel().positionPreviewInLFoot = ballLeftFoot;
  getBallModel().positionPreviewInRFoot = ballRightFoot;


  DEBUG_REQUEST("StaticDebugModelProvider:drawBallOnFieldGlobal", 
    FIELD_DRAWING_CONTEXT;

    const Vector2d ballPositionField = getRobotPose()*getBallModel().position;
    const double lineWidth = 10;
    PEN("FF9900", lineWidth);
    FILLOVAL(ballPositionField.x, ballPositionField.y, getFieldInfo().ballRadius-lineWidth/2, getFieldInfo().ballRadius-lineWidth/2);
    PEN("000000", lineWidth);
    CIRCLE( ballPositionField.x, ballPositionField.y, getFieldInfo().ballRadius-lineWidth/2);
  );

  //Set Obstacle Model
  double obstacles = 0.0;
  MODIFY("StaticDebugModelProvider:Obstacles", obstacles);
  if(obstacles != 0.0) {
    getObstacleModel().frontDistance = 399;
    getObstacleModel().blockedTime = 101;
  }

  DEBUG_REQUEST("StaticDebugModelProvider:drawRobotOnField", 
    FIELD_DRAWING_CONTEXT;
    PEN("FFFFFF", 20);
    ROBOT(getRobotPose().translation.x, getRobotPose().translation.y, getRobotPose().rotation);
  );
  
}
