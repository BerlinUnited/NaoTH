#ifndef __FieldAreaDetector_H_
#define __FieldAreaDetector_H_

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

BEGIN_DECLARE_MODULE(FieldAreaDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

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
END_DECLARE_MODULE(FieldAreaDetector)


class FieldAreaDetector : private FieldAreaDetectorBase
{
public:


  FieldAreaDetector();
  virtual ~FieldAreaDetector();

  void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }

  void execute(CameraInfo::CameraID id);

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("FieldAreaDetector")
    {
      PARAMETER_REGISTER(proportion_of_green) = .5;
      PARAMETER_REGISTER(prop_of_green_skip) = .7;
      PARAMETER_REGISTER(grid_size_top) = 80;
      PARAMETER_REGISTER(grid_size_bottom) = 80;
      PARAMETER_REGISTER(refine_cell) = true;
      PARAMETER_REGISTER(refine_point) = true;
      PARAMETER_REGISTER(split_cell) = true;
      PARAMETER_REGISTER(number_of_scan_points_in_cell) = 3;
      syncWithConfig();
    }
    double proportion_of_green;
    double prop_of_green_skip;
    int32_t grid_size_top;
    int32_t grid_size_bottom;
    bool refine_cell;
    bool refine_point;
    bool split_cell;
    int number_of_scan_points_in_cell;
  } params;

private:
  struct Cell
  {
    int32_t minX;
    int32_t minY;
    int32_t maxX;
    int32_t maxY;
    int sum_of_green;

    Cell() {}
    Cell(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY){
      this->minX = minX;
      this->minY = minY;
      this->maxX = maxX;
      this->maxY = maxY;
      sum_of_green = -1;
    }
    Cell(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, int sum_of_green){
      Cell(minX, minY, maxX, maxY);
      this->sum_of_green = sum_of_green;
    }
  };

  struct Endpoint
  {
    CameraInfo::CameraID cameraID;
    Vector2i pos;
  };

  void find_endpoint(const Cell& cell, Endpoint& endpoint, int point_x_offset);
  void refine_cell(Cell& cell);
  void refine_point(Endpoint& endpoint, int32_t minY);
  int32_t split_cell(const Cell& cell, Cell& upper, Cell& lower, int split_x_offset);

  void create_field();

  CameraInfo::CameraID cameraID;
  int horizon_height;
  int pixels_per_cell;
  int min_green;
  int factor;
  int32_t grid_size;

  std::vector<Endpoint> endpoints;

  DOUBLE_CAM_PROVIDE(FieldAreaDetector, DebugImageDrawings);

  // double cam interface
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, Image);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, FieldColorPercept);

  DOUBLE_CAM_REQUIRE(FieldAreaDetector, BallDetectorIntegralImage);
  DOUBLE_CAM_PROVIDE(FieldAreaDetector, FieldPercept);
};

#endif  /* __FieldAreaDetector_H_ */

