#ifndef POSEDETECTOR_H
#define POSEDETECTOR_H

#include <iostream>

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/DoubleCamHelpers.h"

// Representations
//#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/ObstaclePercept.h"
#include "Representations/Motion/MotionStatus.h"

// Tools
#include "Tools/CameraGeometry.h"

BEGIN_DECLARE_MODULE(PoseDetector)
  REQUIRE(CameraInfo)
  REQUIRE(CameraMatrix)
  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(MotionStatus)

  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
END_DECLARE_MODULE(PoseDetector)

class PoseDetector: public PoseDetectorBase
{
public:
  CameraInfo::CameraID cameraID;

  PoseDetector();
  ~PoseDetector();

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("PoseDetector")
    {

      syncWithConfig();
    }

  } params;
};

#endif // POSEDETECTOR_H
