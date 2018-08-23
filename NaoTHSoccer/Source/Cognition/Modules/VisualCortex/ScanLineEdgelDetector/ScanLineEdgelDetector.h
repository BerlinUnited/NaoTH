/* 
 * File:   ScanLineEdgelDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _ScanLineEdgelDetector_H_
#define _ScanLineEdgelDetector_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include <Representations/Perception/FieldColorPercept.h>
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Infrastructure/FieldInfo.h"

// Tools
#include "Tools/DoubleCamHelpers.h"
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/ImageProcessing/MaximumScan.h"
#include "Tools/CameraGeometry.h"
#include "Tools/ColorClasses.h"
#include <Tools/Math/Vector2.h>

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(ScanLineEdgelDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
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

  REQUIRE(RobotPose)
  REQUIRE(FieldInfo)

  PROVIDE(ScanLineEdgelPercept)
  PROVIDE(ScanLineEdgelPerceptTop)
END_DECLARE_MODULE(ScanLineEdgelDetector)


class ScanLineEdgelDetector : private ScanLineEdgelDetectorBase
{
public:
  ScanLineEdgelDetector();
  virtual ~ScanLineEdgelDetector();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("ScanLineParameters")
    {
      PARAMETER_REGISTER(brightness_threshold_top) = 6*2;
      PARAMETER_REGISTER(brightness_threshold_bottom) = 6*4;
      PARAMETER_REGISTER(scanline_count) = 31;
      PARAMETER_REGISTER(pixel_border_y) = 3;
      PARAMETER_REGISTER(green_sampling_points) = 3;
      PARAMETER_REGISTER(double_edgel_angle_threshold) = 0.2;
      PARAMETER_REGISTER(minEndPointGreenDensity) = 0.3;

      PARAMETER_REGISTER(checkSelfloc) = true;
      PARAMETER_REGISTER(dynamicThreshold) = true;

      syncWithConfig();
      //DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      //DebugParameterList::getInstance().remove(this);
    }

    int brightness_threshold_top; // threshold for detection of the jumps in the Y channel
    int brightness_threshold_bottom;
    int scanline_count; // number of scanlines
    int pixel_border_y; // don't scan the lower lines in the image
    int green_sampling_points; // number of the random samples to determine whether a segment is green 

    bool dynamicThreshold;

    double double_edgel_angle_threshold;
    double minEndPointGreenDensity;

    bool checkSelfloc;
  } theParameters;

private:
  CameraInfo::CameraID cameraID;

  void add_edgel(const Vector2i& point, Edgel::Type type) {
    /*
    Edgel edgel;
    edgel.point = point;
    edgel.direction = calculateGradient(point);
    getScanLineEdgelPercept().edgels.push_back(edgel);
    */
    getScanLineEdgelPercept().edgels.emplace_back(point, calculateGradient(point), type);
  }

  void add_double_edgel(int scan_line_id) 
  {
    ASSERT(getScanLineEdgelPercept().edgels.size() > 1);
    
    // use the last two edgels
    int i_end = ((int) getScanLineEdgelPercept().edgels.size())-1;
    int i_begin = i_end - 1;
    const Edgel& end = getScanLineEdgelPercept().edgels[i_end];
    const Edgel& begin = getScanLineEdgelPercept().edgels[i_begin];

    if(-(begin.direction*end.direction) < theParameters.double_edgel_angle_threshold) {
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

  /** scans at given x-coordinate to the top & cancels at field end. Starts at bottom line. */
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2i& start, const Vector2i& end) ;

  /** */
  ColorClasses::Color estimateColorOfSegment(const Vector2i& begin, const Vector2i& end) const;

  bool validDistance(const Vector2i& pointOne, const Vector2i& pointTwo) const;
  bool onTheField(const Vector2i& point) const;

  /** Estimates the gradient of the gray-gradient at the point by a Sobel Operator. */
  Vector2d calculateGradient(const Vector2i& point) const;

  DOUBLE_CAM_PROVIDE(ScanLineEdgelDetector, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, Image);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, BodyContour);
  
  DOUBLE_CAM_PROVIDE(ScanLineEdgelDetector, ScanLineEdgelPercept);
};

#endif  /* _ScanLineEdgelDetector_H_ */

