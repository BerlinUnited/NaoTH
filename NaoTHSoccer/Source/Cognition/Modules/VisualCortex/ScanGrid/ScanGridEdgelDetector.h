#ifndef _ScanGridEdgelDetector_H_
#define _ScanGridEdgelDetector_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include <Representations/Perception/FieldColorPercept.h>
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/ScanGrid.h"
#include "Representations/Perception/FieldPercept.h"

// Tools
#include "Tools/DoubleCamHelpers.h"
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/CameraGeometry.h"
#include "Tools/ColorClasses.h"
#include <Tools/Math/Vector2.h>

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugPlot.h"
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(ScanGridEdgelDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugPlot)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)

  REQUIRE(ScanGrid)
  REQUIRE(ScanGridTop)

  PROVIDE(ScanLineEdgelPercept)
  PROVIDE(ScanLineEdgelPerceptTop)
END_DECLARE_MODULE(ScanGridEdgelDetector)


class ScanGridEdgelDetector : private ScanGridEdgelDetectorBase
{
public:
  ScanGridEdgelDetector();
  virtual ~ScanGridEdgelDetector();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("ScanGridEdgelDetector")
    {
      PARAMETER_REGISTER(brightness_threshold_top) = 6*2;
      PARAMETER_REGISTER(brightness_threshold_bottom) = 6*4;
      PARAMETER_REGISTER(double_edgel_angle_threshold) = 0.2;

      syncWithConfig();
      //DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      //DebugParameterList::getInstance().remove(this);
    }

    int brightness_threshold_top; // threshold for detection of the jumps in the Y channel
    int brightness_threshold_bottom;

    double double_edgel_angle_threshold;
  } parameters;

private:
  CameraInfo::CameraID cameraID;

  class MaxPeakScan
  {
  private:
    int threshold;
  public:
    int point;
    int prev_point;
    bool found;
    int maxValue;

    MaxPeakScan(int threshold)
    {
      this->threshold = threshold;
      this->maxValue = threshold;
      this->point = 0;
      this->prev_point = 0;
      this->found = false;
    }

    virtual inline bool add(int point, int prev_point, int value) {
      if(check(point, value)) {
        this->prev_point = prev_point;
      } else {
        if(this->found) {
          return true;
        }
        reset();
      }
      return false;
    }

    virtual inline bool check(int point, int value) {
      if(value > this->maxValue) {
        this->found = true;
        this->point = point;
        this->maxValue = value;
        return true;
      }
      return false;
    }

    inline void reset() {
      this->found = false;
      this->maxValue = this->threshold;
    }
  };

  class MinPeakScan: public MaxPeakScan
  {
    using MaxPeakScan::MaxPeakScan;
  public:
    inline bool check(int point, int value) override
    {
      return MaxPeakScan::check(point, -value);
    }
  };

  inline size_t find_min_point(const FieldPercept::FieldPoly& poly) {
    const std::vector<Vector2i>& points = poly.getPoints();

    size_t min = 0;
    for(size_t i=1; i<points.size(); i++) {
      if(points[i].x < points[min].x ||
         (points[i].x == points[min].x && points[i].y < points[min].y)) {
        min = i;
      }
    }
    return min;
  }

  inline void get_poly_line(const FieldPercept::FieldPoly& poly,
                         size_t idx, Math::LineSegment& result) {
    const std::vector<Vector2i>& points = poly.getPoints();

    Vector2i begin = points[idx % points.size()];
    Vector2i end = points[idx+1 % points.size()];

    result = Math::LineSegment(begin, end);
    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_field_intersections",
      //LINE_PX(ColorClasses::red, begin.x, begin.y, end.x, end.y);
      CIRCLE_PX(ColorClasses::red, begin.x, begin.y, 2);
    );
  }

  void add_edgel(const Vector2i& point) {
    Edgel edgel;
    edgel.point = point;
    edgel.direction = calculateGradient(point);
    getScanLineEdgelPercept().edgels.push_back(edgel);
  }

  void add_edgel(int x, int y) {
    Edgel edgel;
    edgel.point.x = x;
    edgel.point.y = y;
    edgel.direction = calculateGradient(edgel.point);
    getScanLineEdgelPercept().edgels.push_back(edgel);
  }

  void add_double_edgel(int scan_line_id)
  {
    ASSERT(getScanLineEdgelPercept().edgels.size() > 1);

    // use the last two edgels
    int i_end = ((int) getScanLineEdgelPercept().edgels.size())-1;
    int i_begin = i_end - 1;
    const Edgel& end = getScanLineEdgelPercept().edgels[i_end];
    const Edgel& begin = getScanLineEdgelPercept().edgels[i_begin];

    if(-(begin.direction*end.direction) < parameters.double_edgel_angle_threshold) {
      return; // false
    }

    ScanLineEdgelPercept::EdgelPair pair;
    pair.begin = i_begin;
    pair.end = i_end;
    pair.id = scan_line_id;

    pair.point.x = (begin.point.x + end.point.x)*0.5;
    pair.point.y = (begin.point.y + end.point.y)*0.5;
    pair.direction = (begin.direction - end.direction).normalize();

    getScanLineEdgelPercept().pairs.push_back(pair);
  }

  inline bool refine_vertical(MaxPeakScan& maximumPeak, int x);
  inline bool refine_horizontal(MaxPeakScan& maximumPeak, int y);

  /** Estimates the gradient of the gray-gradient at the point by a Sobel Operator. */
  Vector2d calculateGradient(const Vector2i& point) const;

  DOUBLE_CAM_PROVIDE(ScanGridEdgelDetector, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, Image);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, ScanGrid);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, FieldPercept);

  DOUBLE_CAM_PROVIDE(ScanGridEdgelDetector, ScanLineEdgelPercept);
};

#endif  /* _ScanGridEdgelDetector_H_ */
