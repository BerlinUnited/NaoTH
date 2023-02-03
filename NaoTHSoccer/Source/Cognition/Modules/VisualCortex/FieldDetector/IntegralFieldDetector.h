
#ifndef IntegralFieldDetector_H
#define IntegralFieldDetector_H

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/FieldColorPercept.h"

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
      PARAMETER_REGISTER(min_field_area) = 5600;

      PARAMETER_REGISTER(max_skip_cells) = 1;
      PARAMETER_REGISTER(min_successive_green) = 3;

      PARAMETER_REGISTER(proportion_of_green) = .5;
      PARAMETER_REGISTER(end_proportion_of_green) = .3;

      PARAMETER_REGISTER(positive_score) = 1;
      PARAMETER_REGISTER(negative_score) = -1;

      // number of columns and rows in the grid
      PARAMETER_REGISTER(top.column_count) = 32;
      PARAMETER_REGISTER(top.row_count) = 24;
      PARAMETER_REGISTER(bottom.column_count) = 22;
      PARAMETER_REGISTER(bottom.row_count) = 14;

      PARAMETER_REGISTER(top.set_image_as_field) = false;
      PARAMETER_REGISTER(bottom.set_image_as_field) = false;
      syncWithConfig();
    }
    int min_field_area;

    double proportion_of_green;
    double end_proportion_of_green;

    int max_skip_cells;
    int min_successive_green;

    double positive_score;
    double negative_score;

    struct cam_params {
        int column_count;
        int row_count;

        bool set_image_as_field;
    } ;
    cam_params top, bottom;
  } params;

private:
  class Cell {
    public:
      int minX;
      int minY;
      int maxX;
      int maxY;

      inline double calc_green_density(const BallDetectorIntegralImage& integralImage) {
          const int factor = integralImage.FACTOR;

          const int minX = this->minX/factor;
          const int minY = this->minY/factor;
          const int maxX = this->maxX/factor;
          const int maxY = this->maxY/factor;

          const double sum = integralImage.getSumForRect(minX, minY, maxX, maxY, 1);
          const double density = sum / ((maxX - minX + 1) * (maxY - minY + 1));
          ASSERT(density <= 1.);
          return density;
      }

      inline int count_pixel() {
          return (maxX - minX + 1) * (maxY - minY + 1);
      }
  };

  CameraInfo::CameraID cameraID;
  std::vector<Vector2i> endpoints;

  void find_endpoint(int x, const Cell& cell, Vector2i& endpoint);
  void create_field();

  DOUBLE_CAM_PROVIDE(IntegralFieldDetector, DebugImageDrawings);

  // double cam interface
  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, Image);
  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, FieldColorPercept);

  DOUBLE_CAM_REQUIRE(IntegralFieldDetector, BallDetectorIntegralImage);
  DOUBLE_CAM_PROVIDE(IntegralFieldDetector, FieldPercept);
};

#endif  /* IntegralFieldDetector_H */

