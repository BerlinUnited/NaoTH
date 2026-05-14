#ifndef HEAD_POSE_CAMERA_MATRIX_H
#define HEAD_POSE_CAMERA_MATRIX_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Body/HeadPose.h>
#include <Representations/Perception/CameraMatrix.h>


#include <Tools/Debug/DebugModify.h>
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugDrawings.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(HeadPoseCameraMatrix)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)

  REQUIRE(HeadPose)

  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  PROVIDE(CameraMatrix)
  PROVIDE(CameraMatrixTop)
END_DECLARE_MODULE(HeadPoseCameraMatrix)


class HeadPoseCameraMatrix : public HeadPoseCameraMatrixBase
{
public:
  HeadPoseCameraMatrix();
  virtual ~HeadPoseCameraMatrix(){};

  virtual void execute();
};

#endif // HEAD_POSE_CAMERA_MATRIX_H
