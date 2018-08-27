#ifndef __IntegralFieldDetector_H_
#define __IntegralFieldDetector_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/FieldColorPercept.h"

#include "Representations/Perception/FieldPercept.h"
#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/Debug/DebugDrawings.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"

#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(IntegralFieldDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)

  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

  PROVIDE(FieldPercept)
  PROVIDE(FieldPerceptTop)
END_DECLARE_MODULE(IntegralFieldDetector)


class IntegralFieldDetector : private IntegralFieldDetectorBase
{
public:


  IntegralFieldDetector();
  virtual ~IntegralFieldDetector();

  void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }

  void execute(CameraInfo::CameraID id);

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("IntegralFieldDetector")
    {
      PARAMETER_REGISTER(proportion_of_green) = .5;
      PARAMETER_REGISTER(grid_size_top) = 80;
      PARAMETER_REGISTER(grid_size_bottom) = 80;
      syncWithConfig();
    }
    double proportion_of_green;
    int grid_size_top;
    int grid_size_bottom;
  } params;

private:
  class Cell{
    public:
      int minX;
      int minY;
      int maxX;
      int maxY;
      int sum_of_green;
  };

  CameraInfo::CameraID cameraID;
  int factor;

  DOUBLE_CAM_PROVIDE(IntegralFieldDetector, DebugImageDrawings);

  // double cam interface
  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, Image);
  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, FieldColorPercept);

  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, BallDetectorIntegralImage);
  DOUBLE_CAM_PROVIDE(IntegralFieldDetector, FieldPercept);
};

#endif  /* __IntegralFieldDetector_H_ */

