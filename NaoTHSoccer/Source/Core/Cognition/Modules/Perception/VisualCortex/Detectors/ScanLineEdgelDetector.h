/* 
 * File:   ScanLineEdgelDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _ScanLineEdgelDetector_H_
#define _ScanLineEdgelDetector_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/ColorClasses.h"
#include <Tools/Math/Vector2.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include <Representations/Perception/FieldColorPercept.h>
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(ScanLineEdgelDetector)
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
      PARAMETER_REGISTER(brightness_threshold) = 6;
      PARAMETER_REGISTER(scanline_count) = 31;
      PARAMETER_REGISTER(pixel_border_y) = 3;
      PARAMETER_REGISTER(double_edgel_angle_threshold) = 0.2;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int brightness_threshold; // threshold for detection of the jumps in the Y channel
    int scanline_count; // number of scanlines
    int pixel_border_y; // don't scan the lower lines in the image

    double double_edgel_angle_threshold;
  } theParameters;

private:
  CameraInfo::CameraID cameraID;

  void add_edgel(int x, int y) {
    Edgel edgel;
    edgel.point.x = x;
    edgel.point.y = y;
    edgel.angle = calculateGradient(edgel.point).angle();
    getScanLineEdgelPercept().edgels.push_back(edgel);
  }


  void add_edgel(int x0, int y0, int x1, int y1, int id) {
    DoubleEdgel double_edgel;
    double_edgel.begin.x = x0;
    double_edgel.begin.y = y0;
    double_edgel.begin_angle = calculateGradient(double_edgel.begin).angle();

    double_edgel.end.x = x1;
    double_edgel.end.y = y1;
    double_edgel.end_angle = Math::normalizeAngle(calculateGradient(double_edgel.end).angle() + Math::pi);

    double_edgel.center = (double_edgel.end + double_edgel.begin) / 2;
    double_edgel.center_angle = calculateMeanAngle(double_edgel.begin_angle, double_edgel.end_angle);

    double_edgel.ScanLineID = id;

    if(fabs(Math::normalizeAngle(double_edgel.begin_angle - double_edgel.end_angle)) <= theParameters.double_edgel_angle_threshold)
    {
      double_edgel.valid = true;
      getScanLineEdgelPercept().scanLineEdgels.push_back(double_edgel);
    }
  }


  /** scans at given x-coordinate to the top & cancels at field end. Starts at bottom line. */
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2i& start, const Vector2i& end) ;

  /** */
  ColorClasses::Color estimateColorOfSegment(const Vector2i& begin, const Vector2i& end) const;

  /** Estimates the gradient of the gray-gradient at the point by a Sobel Operator. */
  Vector2d calculateGradient(const Vector2i& point) const;

  /** */
  inline double calculateMeanAngle(double a, double b) const {
    return atan2( sin(a)+sin(b), cos(a)+cos(b) );
  }

  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, Image);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, BodyContour);
  
  DOUBLE_CAM_PROVIDE(ScanLineEdgelDetector, ScanLineEdgelPercept);
};

#endif  /* _ScanLineEdgelDetector_H_ */

