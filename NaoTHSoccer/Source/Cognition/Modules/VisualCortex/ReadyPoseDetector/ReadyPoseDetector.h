/**
* @file ReadyPoseDetector.h
* 
*/

#ifndef READY_POSE_DETECTOR_H
#define READY_POSE_DETECTOR_H

#include <ModuleFramework/Module.h>

// common tools
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Pose2D.h>

#include <Representations/Infrastructure/FrameInfo.h>

#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Infrastructure/Image.h>

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Modeling/PlayerInfo.h"

#include "tfliteexecuter.h"

// debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"


BEGIN_DECLARE_MODULE(ReadyPoseDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugModify)
  PROVIDE(StopwatchManager)

  REQUIRE(FrameInfo)

  REQUIRE(PlayerInfo)

  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldInfo)
END_DECLARE_MODULE(ReadyPoseDetector)


class ReadyPoseDetector: private ReadyPoseDetectorBase
{
public:
  ReadyPoseDetector();
  virtual ~ReadyPoseDetector();

  virtual void execute();

private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("ReadyPoseDetector")
    {
      PARAMETER_REGISTER(parameter) = 1.0;
      
      syncWithConfig();
    }

    double parameter;
  } params;


private:

  TFLiteExecuter exec;

};//end class CNNBallDetector

#endif // READY_POSE_DETECTOR_H
