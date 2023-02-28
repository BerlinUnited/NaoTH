/**
* @file IntegralSubsamplingTest.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* 
* A test for interpolated subsampling of the image based on the integral image.
* 
* @see Parts were copied from the IntegralFieldDetector.h.
*/

#ifndef INTEGRAL_SUBSAMPLING_TEST_H
#define INTEGRAL_SUBSAMPLING_TEST_H

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"

#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/Debug/DebugDrawings.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>

#include "Tools/DoubleCamHelpers.h"
#include <Tools/Math/Vector2.h>

BEGIN_DECLARE_MODULE(IntegralSubsamplingTest)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)

  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

END_DECLARE_MODULE(IntegralSubsamplingTest)


class IntegralSubsamplingTest : private IntegralSubsamplingTestBase
{
public:

  IntegralSubsamplingTest();
  virtual ~IntegralSubsamplingTest();

  void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }

  void execute(CameraInfo::CameraID id);

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("IntegralSubsamplingTest")
    {
      // number of columns and rows in the grid
      PARAMETER_REGISTER(top.column_count) = 32;
      PARAMETER_REGISTER(top.row_count) = 24;
      PARAMETER_REGISTER(bottom.column_count) = 22;
      PARAMETER_REGISTER(bottom.row_count) = 14;

      syncWithConfig();
    }

    struct cam_params {
        int column_count;
        int row_count;
    } ;
    cam_params top, bottom;
  } params;

private:
  class Cell {
    public:
      Vector2i min;
      Vector2i max;

      inline double calc_green_density(const BallDetectorIntegralImage& integralImage) 
      {
          // calculate coordinates in the integral image
          const int factor = integralImage.FACTOR;

          const int minX = min.x/factor;
          const int minY = min.y/factor;
          const int maxX = max.x/factor;
          const int maxY = max.y/factor;

          const double density = integralImage.getDensityForRect(minX, minY, maxX, maxY, 1);
          ASSERT(density <= 1.0);
          return density;
      }
  };

  CameraInfo::CameraID cameraID;

  // double cam interface
  DOUBLE_CAM_PROVIDE(IntegralSubsamplingTest, DebugImageDrawings);
  DOUBLE_CAM_REQUIRE(IntegralSubsamplingTest, Image);
  DOUBLE_CAM_REQUIRE(IntegralSubsamplingTest, BallDetectorIntegralImage);
};

#endif  /* INTEGRAL_SUBSAMPLING_TEST_H */

