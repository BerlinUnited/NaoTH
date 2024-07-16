
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
  Pose2D robotPose (-Math::pi_2, 750, -3050);

  DEBUG_REQUEST("ReadyPoseDetector:draw_robot_pose_on_field",
    ROBOT(robotPose.translation.x, robotPose.translation.y, robotPose.rotation);
  );

  /*
  Vector3d pointInField (0.0, 3250.0, );
  Vector2i pointInImage;
  if (CameraGeometry::relativePointToImage(getCameraMatrixTop(), getCameraInfo(), ballInField, ballInImage))
  {
  }
  */


}

