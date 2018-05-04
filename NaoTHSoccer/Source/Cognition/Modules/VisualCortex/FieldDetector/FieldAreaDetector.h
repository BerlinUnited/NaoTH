#ifndef __FieldAreaDetector_H_
#define __FieldAreaDetector_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/Image.h"
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
END_DECLARE_MODULE(FieldAreaDetector)


class FieldAreaDetector : private FieldAreaDetectorBase
{
public:


  FieldAreaDetector();
  virtual ~FieldAreaDetector();

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  void execute(CameraInfo::CameraID id);

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("FieldAreaDetector")
    {
      PARAMETER_REGISTER(proportion_of_green) = .5;
      PARAMETER_REGISTER(grid_size) = 60;
      PARAMETER_REGISTER(refine_cell) = true;
      PARAMETER_REGISTER(refine_point) = true;
      syncWithConfig();
    }
    double proportion_of_green;
    int32_t grid_size;
    bool refine_cell;
    bool refine_point;
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

  void find_endpoint(const Cell& cell, Vector2i& endpoint);
  void refine_cell(Cell& cell);
  void refine_point(Vector2i& endpoint, int32_t minY);
  int32_t split_cell(const Cell& cell, Cell& upper, Cell& lower);

  CameraInfo::CameraID cameraID;
  int horizon_height;
  int pixels_per_cell;
  int min_green;
  int factor;
  int32_t grid_size;

  DOUBLE_CAM_PROVIDE(FieldAreaDetector, DebugImageDrawings);

  // double cam interface
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, Image);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, FieldColorPercept);

  DOUBLE_CAM_REQUIRE(FieldAreaDetector, BallDetectorIntegralImage);
};

#endif  /* __FieldAreaDetector_H_ */

