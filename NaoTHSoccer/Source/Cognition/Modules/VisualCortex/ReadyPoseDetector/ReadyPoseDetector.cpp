
#include "ReadyPoseDetector.h"
#include "Tools/CameraGeometry.h"
#include <Tools/ImageProcessing/ColorModelConversions.h>


using namespace std;

ReadyPoseDetector::ReadyPoseDetector()
{
  DEBUG_REQUEST_REGISTER("ReadyPoseDetector:draw_robot_pose_on_field", "draw pose", false);
  DEBUG_REQUEST_REGISTER("ReadyPoseDetector:draw_pose_in_image", "draw pose", false);
  DEBUG_REQUEST_REGISTER("ReadyPoseDetector:draw_detection_area", "draw pose", false);
  
  exec.loadModelFromFile("Config/movenet_lightning.tflite", {1, 192, 192, 3}, 1);

  getDebugParameterList().add(&params);
}

ReadyPoseDetector::~ReadyPoseDetector()
{
  getDebugParameterList().remove(&params);
}


void ReadyPoseDetector::execute()
{

  if(getPlayerInfo().robotState != PlayerInfo::RobotState::standby) {
    return;
  }

  // NOTE: maybe it's better to do it in behaior?
  if(getPlayerInfo().playerNumber != 4 && getPlayerInfo().playerNumber != 7) {
    return;
  }

  /*
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
  */


  Vector2d poseInImage(132, 110);
  MODIFY("ReadyPoseDetector:poseInImage.x", poseInImage.x);
  MODIFY("ReadyPoseDetector:poseInImage.y", poseInImage.y);

  DEBUG_REQUEST("ReadyPoseDetector:draw_detection_area",
    IMAGE_DRAWING_CONTEXT;
    CANVAS("ImageTop");
    PEN("FF0000", 1);

    BOX(poseInImage.x, poseInImage.y, poseInImage.x + 192, poseInImage.y + 192);
  );


  float (*inputTensor)[1][192][192][3] = reinterpret_cast<float(*)[1][192][192][3]>(exec.getInputTensor());


  // create input tensor
  Pixel pixel;
  Pixel pixelRGB;
  Vector2i point;
  for(int x = 0; x < 192; ++x) {
    for(int y = 0; y < 192; ++y) {

      point = poseInImage + Vector2i(x,y);
      if(getImageTop().isInside(point)) {
        getImageTop().get_direct(point.x, point.y, pixel);
      }

      ColorModelConversions::fromYCbCrToRGB(pixel.y, pixel.cb, pixel.cr, pixelRGB.a, pixelRGB.b, pixelRGB.c);
      
      
      (*inputTensor)[0][y][x][0] = ((float)pixelRGB.a);
      (*inputTensor)[0][y][x][1] = ((float)pixelRGB.b);
      (*inputTensor)[0][y][x][2] = ((float)pixelRGB.c);
    }
  }


  exec.execute();


  // draw output
  const float (*output)[1][1][17][3] = reinterpret_cast<const float(*)[1][1][17][3]>(exec.getOutputTensor());

  /*
  for(int i = 0; i < 17; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << (*output)[0][0][i][j] << "\t";
    }
    std::cout << std::endl;
  }
  */


  // eyes
  float y1 = (*output)[0][0][1][0] * 192.0;
  float x1 = (*output)[0][0][1][1] * 192.0;

  float y2 = (*output)[0][0][2][0] * 192.0;
  float x2 = (*output)[0][0][2][1] * 192.0;

  // hands
  float y3 = (*output)[0][0][9][0] * 192.0;
  float x3 = (*output)[0][0][9][1] * 192.0;

  float y4 = (*output)[0][0][10][0] * 192.0;
  float x4 = (*output)[0][0][10][1] * 192.0;


  //y,x und conf. y
  DEBUG_REQUEST("ReadyPoseDetector:draw_detection_area",
    IMAGE_DRAWING_CONTEXT;
    CANVAS("ImageTop");
    PEN("FF0000", 1);

    CIRCLE(poseInImage.x + (int)x1, poseInImage.y + (int)y1, 3);
    CIRCLE(poseInImage.x + (int)x2, poseInImage.y + (int)y2, 3);
    CIRCLE(poseInImage.x + (int)x3, poseInImage.y + (int)y3, 3);
    CIRCLE(poseInImage.x + (int)x4, poseInImage.y + (int)y4, 3);
  );


  bool readyRefereePoseDetected = false;

  if( (y3 + y4) * 0.5 < (y1 + y1) * 0.5 - 5) {
    readyRefereePoseDetected = true;
  }

  DEBUG_REQUEST("ReadyPoseDetector:draw_detection_area",
    IMAGE_DRAWING_CONTEXT;
    CANVAS("ImageTop");
    PEN("00FF00", 3);

    if(readyRefereePoseDetected) {
      BOX(poseInImage.x, poseInImage.y, poseInImage.x + 192, poseInImage.y + 192);
    }
  );

  
}

