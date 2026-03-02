
#include "HeadPoseCameraMatrix.h"

#include <Tools/NaoInfo.h>
#include <Tools/CameraGeometry.h>

HeadPoseCameraMatrix::HeadPoseCameraMatrix()
{
    DEBUG_REQUEST_REGISTER("HeadPoseCameraMatrix:project_image_borders",
                           "projects the borders of the image onto the ground",
                           false);
}

void HeadPoseCameraMatrix::execute()
{
  RotationMatrix rot(
    Vector3d( 0.0927495956,   -0.995669305,   -0.00634055864),  // column 0
    Vector3d( 0.00744829793,   0.00706163328, -0.999947429),    // column 1
    Vector3d( 0.995661557,     0.0926974788,   0.00807100441)   // column 2
  );
  Vector3d trans(-0.00978016853, 0.0303872209, -0.00966392457);
  trans *= 1000; // m to mm
  // Pose
  Pose3D headCameraTransform(rot.transpose(), trans);


  RotationMatrix link_R_opt(
    Vector3d( 0,  0,  1),   // col0
    Vector3d(-1,  0,  0),   // col1
    Vector3d( 0, -1,  0)    // col2
  );
  RotationMatrix opt_R_link = link_R_opt.transpose();
  Pose3D opt_T_link(opt_R_link, Vector3d(0,0,0));

  headCameraTransform = headCameraTransform * opt_T_link;

  Pose3D headCameraTransformLeft(headCameraTransform);
  headCameraTransformLeft.translation.y = -headCameraTransformLeft.translation.y;

  //const Pose3D& head = getHeadPose().pose;
  Pose3D head = getHeadPose().pose;

  /*
  pitch_compensation: 0.8
  yaw_compensation: 1.8
  z_compensation: 0

  p_headprime2head_ = Pose(0, 0, z_comp, 0, pitch_comp * M_PI / 180, yaw_comp * M_PI / 180);
  Pose p_eye2base = p_head2base * p_headprime2head_ * p_eye2head_;
  */


  getCameraMatrix() = head*headCameraTransform;
  //getCameraMatrix().translate(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Bottom].offset);
  //getCameraMatrix().rotateY(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Bottom].rotationY);

  getCameraMatrixTop() = head*headCameraTransformLeft;
  //getCameraMatrixTop().translate(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Top].offset);
  //getCameraMatrixTop().rotateY(NaoInfo::robotDimensions.cameraTransform[naoth::CameraInfo::Top].rotationY);

  getCameraMatrix().valid = true;
  getCameraMatrixTop().valid = true;


  DEBUG_REQUEST("HeadPoseCameraMatrix:project_image_borders",
    // TODO: make it better
    Vector2d p1; Vector2d p2;
    Vector2d r1; Vector2d r2; Vector2d r3; Vector2d r4;

    CameraGeometry::calculateArtificialHorizon(getCameraMatrix(), getCameraInfo(), p1, p2);
    p1.y++;
    p2.y++;

    // clamp horizon to image borders
    p1.x = Math::clamp(p1.x, 0.0, static_cast<double>(getCameraInfo().resolutionWidth));
    p1.y = Math::clamp(p1.y, 0.0, static_cast<double>(getCameraInfo().resolutionHeight));
    p2.x = Math::clamp(p2.x, 0.0, static_cast<double>(getCameraInfo().resolutionWidth));
    p2.y = Math::clamp(p2.y, 0.0, static_cast<double>(getCameraInfo().resolutionHeight));

    // transform horizon and image borders to ground
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), p1, 0, r1);
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), p2, 0, r2);
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), 0, getCameraInfo().resolutionHeight, 0, r3);
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(), getCameraInfo(), getCameraInfo().resolutionWidth, getCameraInfo().resolutionHeight, 0, r4);
    FIELD_DRAWING_CONTEXT;
    LINE(r3.x, r3.y, r4.x, r4.y);
    LINE(r4.x, r4.y, r2.x, r2.y);
    LINE(r2.x, r2.y, r1.x, r1.y);
    LINE(r1.x, r1.y, r3.x, r3.y);

    CameraGeometry::calculateArtificialHorizon(getCameraMatrixTop(), getCameraInfoTop(), p1, p2);
    p1.y++;
    p2.y++;

    // clamp horizon to image borders
    p1.x = Math::clamp(p1.x, 0.0, static_cast<double>(getCameraInfoTop().resolutionWidth));
    p1.y = Math::clamp(p1.y, 0.0, static_cast<double>(getCameraInfoTop().resolutionHeight));
    p2.x = Math::clamp(p2.x, 0.0, static_cast<double>(getCameraInfoTop().resolutionWidth));
    p2.y = Math::clamp(p2.y, 0.0, static_cast<double>(getCameraInfoTop().resolutionHeight));

    // transform horizon and image borders to ground
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), p1, 0, r1);
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), p2, 0, r2);
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), 0, getCameraInfoTop().resolutionHeight, 0, r3);
    CameraGeometry::imagePixelToFieldCoord(getCameraMatrixTop(), getCameraInfoTop(), getCameraInfoTop().resolutionWidth, getCameraInfoTop().resolutionHeight, 0, r4);
    FIELD_DRAWING_CONTEXT;
    LINE(r3.x, r3.y, r4.x, r4.y);
    LINE(r4.x, r4.y, r2.x, r2.y);
    LINE(r2.x, r2.y, r1.x, r1.y);
    LINE(r1.x, r1.y, r3.x, r3.y);
  );
}


