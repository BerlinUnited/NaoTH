
#ifndef _IntegralFieldDetector_H_
#define _IntegralFieldDetector_H_

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
  // TODO: why is the image needed?
  // getImage().width() ...
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
      PARAMETER_REGISTER(column_count_top) = 32;
      PARAMETER_REGISTER(row_count_top) = 24;
      PARAMETER_REGISTER(column_count_bottom) = 22;
      PARAMETER_REGISTER(row_count_bottom) = 14;

      PARAMETER_REGISTER(set_whole_image_as_field_bottom) = false;
      PARAMETER_REGISTER(set_whole_image_as_field_top) = false;
      syncWithConfig();
    }
    int min_field_area;

    double proportion_of_green;
    double end_proportion_of_green;

    int max_skip_cells;
    int min_successive_green;

    double positive_score;
    double negative_score;

    int column_count_top;
    int column_count_bottom;
    int row_count_top;
    int row_count_bottom;

    bool set_whole_image_as_field_bottom;
    bool set_whole_image_as_field_top;
  } params;

private:
  class Cell {
    public:
      int minX;
      int minY;
      int maxX;
      int maxY;

      inline int get_green_sum(const BallDetectorIntegralImage& integralImage) {
          return integralImage.getSumForRect(minX, minY, maxX, maxY, 1);
      }

      inline int count_pixel() {
          return (maxX - minX) * (maxY - minY);
      }
  };

  CameraInfo::CameraID cameraID;
  int factor;
  std::vector<Vector2i> endpoints;

  /**
  converts integral image coordinates to image coordinates
  */
  inline int toImage(int i) {
    return i * factor;
  }

  /**
  converts image coordinates to integral image coordinates
  */
  inline int toIntegral(int i) {
    return i / factor;
  }

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

#endif  /* _IntegralFieldDetector_H_ */

