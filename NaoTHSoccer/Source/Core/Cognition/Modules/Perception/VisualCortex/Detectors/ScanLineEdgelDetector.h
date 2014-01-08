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
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include <Representations/Perception/FieldColorPercept.h>
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"
//#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(ScanLineEdgelDetector)
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
  //REQUIRE(FieldPerceptRaw)
  //REQUIRE(FieldPerceptRawTop)
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

    // threshold for detection of the jumps in the Y channel
    int brightness_threshold;

    // number of scanlines
    int scanline_count;

    // don't scan the lower lines in the image
    int pixel_border_y;

    double double_edgel_angle_threshold;
  } theParameters;

private:
  CameraInfo::CameraID cameraID;
  int current_scanlineID;
  std::vector<double> vertical_confidence;

  void add_edgel(int x, int y) {
    Edgel edgel;
    edgel.point.x = x;
    edgel.point.y = y;
    edgel.angle = getPointsAngle(edgel.point);
    getScanLineEdgelPercept().edgels.push_back(edgel);
  }

  /** scans at given x-coordinate to the top & cancels at field end. Starts at bottom line. */
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2i& start, const Vector2i& end) ;

  /** */
  ColorClasses::Color estimateColorOfSegment(const Vector2i& begin, const Vector2i& end) const;

  /** Estimates the angle of the gray-gradient at the point by a Sobel Operator. */
  double getPointsAngle(const Vector2i& point) const;

  /** */
  double calculateMeanAngle(double angle1, double angle2) const;

  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, Image);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(ScanLineEdgelDetector, BodyContour);
  
  DOUBLE_CAM_PROVIDE(ScanLineEdgelDetector, ScanLineEdgelPercept);
};

#endif  /* _ScanLineEdgelDetector_H_ */

