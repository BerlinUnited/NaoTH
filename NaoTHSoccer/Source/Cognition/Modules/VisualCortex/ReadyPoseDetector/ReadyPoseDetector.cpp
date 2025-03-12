
#include "ReadyPoseDetector.h"
#include "Tools/CameraGeometry.h"


using namespace std;

ReadyPoseDetector::ReadyPoseDetector()
{
  DEBUG_REQUEST_REGISTER("ReadyPoseDetector:draw_robot_pose_on_field", "draw pose", false);
  DEBUG_REQUEST_REGISTER("ReadyPoseDetector:draw_pose_in_image", "draw pose", false);
  DEBUG_REQUEST_REGISTER("ReadyPoseDetector:draw_detection_area", "draw pose", false);
  

  getDebugParameterList().add(&params);
}

ReadyPoseDetector::~ReadyPoseDetector()
{
  getDebugParameterList().remove(&params);
}


void ReadyPoseDetector::execute()
{

  /*
  // NOTE: maybe it's better to do it in behaior?
  if(getPlayerInfo().playerNumber != 4) {
    return;
  }

  if(getPlayerInfo().robotState != PlayerInfo::RobotState::standby) {
    return;
  }
  */

  // default position of the player 4
  Pose2D robotPose (-Math::pi_2, -750, 3050);

  DEBUG_REQUEST("ReadyPoseDetector:draw_robot_pose_on_field",
    FIELD_DRAWING_CONTEXT;
    ROBOT(robotPose.translation.x, robotPose.translation.y, robotPose.rotation);
  );

  // position of the referee
  Vector3d pointInField (0.0, -3250.0, 1000);
  Vector2i pointInImage;
  if (CameraGeometry::relativePointToImage(getCameraMatrixTop(), getCameraInfo(), pointInField, pointInImage))
  {

    DEBUG_REQUEST("ReadyPoseDetector:draw_pose_in_image",
      IMAGE_DRAWING_CONTEXT;
      CANVAS("ImageTop");
      PEN("FF0000", 1);
      CIRCLE(pointInImage.x, pointInImage.y, 2);
    );


  }
  
  // 192, 192 rgb
}

