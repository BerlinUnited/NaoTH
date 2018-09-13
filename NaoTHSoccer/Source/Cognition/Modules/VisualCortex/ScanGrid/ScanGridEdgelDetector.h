#ifndef _ScanGridEdgelDetector_H_
#define _ScanGridEdgelDetector_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include <Representations/Perception/FieldColorPercept.h>
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
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
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
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


  template<typename T, typename V>
  class MaximumScan
  {
  private:
    V threshhold;
    V value_max;

  public:
    V maxValue;
    T peakPoint;

    MaximumScan(T startPoint, V threshhold)
    {
      this->threshhold = threshhold;
      value_max = threshhold;
      peakPoint = startPoint;
      maxValue = threshhold;
    }
    virtual inline bool add(T point, V value)
    {
      if(value > value_max)
      {
        value_max = value;
        peakPoint = point;
      } else if(value_max > threshhold && value < threshhold) {
        maxValue = value_max;
        value_max = threshhold;
        return true;
      }
      return false;
    }

    inline void clean(T startPoint) {
      this->peakPoint = startPoint;
      value_max = threshhold;
    }
  };

  template<typename T, typename V>
  class MinimumScan: public MaximumScan<T, V>
  {
    using MaximumScan<T, V>::MaximumScan;
  public:
    inline bool add(T point, V value)
    {
      return MaximumScan<T, V>::add(point, -value);
    }
  };



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

  inline void refine_edge(MaximumScan<int,int>& refinedScan, int x, int start_y, int end_y);

  /** Estimates the gradient of the gray-gradient at the point by a Sobel Operator. */
  Vector2d calculateGradient(const Vector2i& point) const;

  DOUBLE_CAM_PROVIDE(ScanGridEdgelDetector, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, Image);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, ScanGrid);
  DOUBLE_CAM_REQUIRE(ScanGridEdgelDetector, FieldPercept);

  DOUBLE_CAM_PROVIDE(ScanGridEdgelDetector, ScanLineEdgelPercept);
};

#endif  /* _ScanGridEdgelDetector_H_ */
