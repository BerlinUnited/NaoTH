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
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
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
      PARAMETER_REGISTER(brightness_threshold_top) = 20;
      PARAMETER_REGISTER(brightness_threshold_bottom) = 24;
      PARAMETER_REGISTER(double_edgel_angle_threshold) = 0.2;

      PARAMETER_REGISTER(gradient_offset) = 1;

      PARAMETER_REGISTER(scan_vertical) = true;
      PARAMETER_REGISTER(scan_horizontal) = true;

      PARAMETER_REGISTER(full_refinement) = true;

      syncWithConfig();
      //DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      //DebugParameterList::getInstance().remove(this);
    }

    bool scan_vertical;
    bool scan_horizontal;

    bool full_refinement;

    int brightness_threshold_top; // threshold for detection of the jumps in the Y channel
    int brightness_threshold_bottom;
    double double_edgel_angle_threshold;

    int gradient_offset;
  } params;

private:
  CameraInfo::CameraID cameraID;
  int scan_id;

  class MaxPeakScan
  {
  private:
    int threshold;
  public:
    int point;
    int prev_point;
    int next_point;
    bool found;
    int maxValue;

    explicit MaxPeakScan(int threshold)
    {
      this->threshold = threshold;
      this->maxValue = threshold;
      this->point = 0;
      this->prev_point = 0;
      this->next_point = 0;
      this->found = false;
    }

    virtual inline bool add(int between_point, int prev_point, int next_point, int value) {
      if(check(between_point, value)) {
        this->prev_point = prev_point;
        this->next_point = next_point;
        return false;
      } else {
        return this->found;
      }
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
      this->maxValue = threshold;
      this->point = 0;
      this->prev_point = 0;
      this->next_point = 0;
      this->found = false;
    }
  };

  class MinPeakScan: public MaxPeakScan
  {
  public:
    explicit MinPeakScan(int threshold) : MaxPeakScan(threshold) {}

    inline bool check(int point, int value) override
    {
      return MaxPeakScan::check(point, -value);
    }
  };

  void scan_vertical(MaxPeakScan& maximumPeak, MinPeakScan& minimumPeak);

  void scan_horizontal(MaxPeakScan& maximumPeak, MinPeakScan& minimumPeak);

  /**
    Finds the index of the minimum x point in the field poly.
    If there is a tie, the point with the smaller y value is chosen.

    @param points of of field polygon
  */
  inline size_t find_min_point_x(const std::vector<Vector2i>& points) {
    // TODO: More intelligent search
    size_t min = 0;
    for(size_t i=1; i<points.size(); i++) {
      if(points[i].x < points[min].x ||
         (points[i].x == points[min].x && points[i].y < points[min].y)) {
        min = i;
      }
    }
    return min;
  }

  /**
    Finds the index of the minimum y point in the field poly.
    If there is a tie, the point with the smaller x value is chosen.

    @param points of of field polygon
  */
  inline size_t find_min_point_y(const std::vector<Vector2i>& points) {
    // TODO: More intelligent search
    size_t min = 0;
    for(size_t i=1; i<points.size(); i++) {
      if(points[i].y < points[min].y ||
         (points[i].y == points[min].y && points[i].x < points[min].x)) {
        min = i;
      }
    }
    return min;
  }

  inline size_t next_poly_line(size_t idx, Math::LineSegment& result,
                               const std::vector<Vector2i>& poly_points)
  {
    // BEWARE: Requires poly_points to be in clockwise order
    size_t end_idx = (idx+1) % poly_points.size();
    if(poly_points[idx] == poly_points[end_idx]) {
      idx = end_idx;
      end_idx = (idx+1) % poly_points.size();
    }
    result = Math::LineSegment(
      poly_points[idx],
      poly_points[end_idx]
    );
    return end_idx;
  }

  inline size_t prev_poly_line(size_t idx, Math::LineSegment& result,
                               const std::vector<Vector2i>& poly_points)
  {
    // BEWARE: Requires poly_points to be in clockwise order
    size_t end_idx = (idx==0)? poly_points.size()-1: idx-1;
    if(poly_points[idx] == poly_points[end_idx]) {
      idx = end_idx;
      end_idx = (idx==0)? poly_points.size()-1: idx-1;
    }
    result = Math::LineSegment(
      poly_points[idx],
      poly_points[end_idx]
    );
    return end_idx;
  }

  /*
  bool add_edgel(const Vector2i& point, Edgel::Type type) {
    if( point.x < params.gradient_offset || point.x + params.gradient_offset + 1 > (int)getImage().width() ||
        point.y < params.gradient_offset || point.y + params.gradient_offset + 1 > (int)getImage().height() ) {
      // Cannot extract direction on image border
      // In the future the edgel detector should make sure not to scan for points on the image border (parameters.gradient_offset)
      return false;
    }

    Edgel edgel;
    edgel.type = type;
    edgel.point = point;
    edgel.direction = calculateGradient(point);
    getScanLineEdgelPercept().edgels.push_back(edgel);
    return true;
  }*/

  inline void add_edgel(int x, int y, Edgel::Type type) {
    Edgel edgel;
    edgel.type = type;
    edgel.point.x = x;
    edgel.point.y = y;
    edgel.direction = calculateGradient(edgel.point);

    getScanLineEdgelPercept().edgels.push_back(edgel);
  }

  inline void add_double_edgel(int scan_line_id, bool adaptive = false)
  {
    ASSERT(getScanLineEdgelPercept().edgels.size() > 1);

    // use the last two edgels
    int i_end = ((int) getScanLineEdgelPercept().edgels.size())-1;
    int i_begin = i_end - 1;
    const Edgel& end = getScanLineEdgelPercept().edgels[i_end];
    const Edgel& begin = getScanLineEdgelPercept().edgels[i_begin];

    double cos_alpha = begin.direction*end.direction;

    /*
    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_double_edgels",
      ColorClasses::Color color = ColorClasses::black;
      if(begin.type == Edgel::positive) {
        color = ColorClasses::blue;
      } else if(begin.type == Edgel::negative) {
        color = ColorClasses::red;
      }
      LINE_PX(color, begin.point.x, begin.point.y,
                     begin.point.x + (int)(begin.direction.x*5),
                     begin.point.y + (int)(begin.direction.y*5));
      color = ColorClasses::black;
      if(end.type == Edgel::positive) {
        color = ColorClasses::blue;
      } else if(end.type == Edgel::negative) {
        color = ColorClasses::red;
      }
      LINE_PX(color, end.point.x, end.point.y,
                     end.point.x + (int)(end.direction.x*5),
                     end.point.y + (int)(end.direction.y*5));
    );
    */

    if(-cos_alpha < params.double_edgel_angle_threshold) {
    //if(-(begin.direction*end.direction) < params.double_edgel_angle_threshold) {
      return; // false
    }

    ScanLineEdgelPercept::EdgelPair pair;
    pair.begin = i_begin;
    pair.end = i_end;
    pair.id = scan_line_id;

    pair.point.x = (begin.point.x + end.point.x)*0.5;
    pair.point.y = (begin.point.y + end.point.y)*0.5;
    pair.direction = (begin.direction - end.direction).normalize();
    pair.adaptive = adaptive;
    /*
    pair.direction = begin.direction;
    if(cos_alpha > 0) {
      pair.direction.rotate(std::acos(std::fabs(cos_alpha))/2);
    } else {
      pair.direction.rotate(-std::acos(std::fabs(cos_alpha))/2);
    }*/

    getScanLineEdgelPercept().pairs.push_back(pair);
  }

  inline bool refine_vertical(MaxPeakScan& maximumPeak, int x);

  inline bool refine_horizontal(MaxPeakScan& maximumPeak, int y);

  /** Estimates the gradient of the gray-gradient at the point by a Sobel Operator. */
  Vector2d calculateGradient(const Vector2i& point) const;

  DOUBLE_CAM_PROVIDE(ScanGridEdgelDetector, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, Image);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, ScanGrid);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, FieldPercept);

  DOUBLE_CAM_PROVIDE(ScanGridEdgelDetector, ScanLineEdgelPercept);
};

#endif  /* _ScanGridEdgelDetector_H_ */
