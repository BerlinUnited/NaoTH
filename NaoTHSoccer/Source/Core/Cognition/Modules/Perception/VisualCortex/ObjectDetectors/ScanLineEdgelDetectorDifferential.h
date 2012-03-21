/* 
 * File:   ScanLineEdgelDetectorDifferentialDifferential.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _ScanLineEdgelDetectorDifferentialDifferential_H_
#define _ScanLineEdgelDetectorDifferentialDifferential_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
//#include "Tools/ImageProcessing/LineDetectorConstParameters.h"
#include "Tools/ImageProcessing/Edgel.h"

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/ColorClassificationModel.h"
//#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/BodyContour.h"


BEGIN_DECLARE_MODULE(ScanLineEdgelDetectorDifferential)
  REQUIRE(Image)
  REQUIRE(ColorClassificationModel)
  //REQUIRE(CurrentCameraSettings)
  REQUIRE(CameraMatrix)
  REQUIRE(FieldPercept)
  REQUIRE(BodyContour)

  PROVIDE(ScanLineEdgelPercept)
END_DECLARE_MODULE(ScanLineEdgelDetectorDifferential)


class ScanLineEdgelDetectorDifferential : private ScanLineEdgelDetectorDifferentialBase
{
public:
  ScanLineEdgelDetectorDifferential();
  ~ScanLineEdgelDetectorDifferential();

  void execute();


  class Parameters
  {
  public:
    Parameters()
    {
      brightness_threshold = 6;
      scanline_count = 22;
      pixel_border_y = 3;
    }

    // threshold for detection of the jumps in the Y channel
    int brightness_threshold;

    // number of scanlines
    int scanline_count;

    // don't scan the lower lines in the image
    int pixel_border_y;
  } theParameters;

private:

  /** scans at given x-coordinate to the top & cancels at field end. Starts at bottom line. */
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end) const;

  /** */
  ColorClasses::Color estimateColorOfSegment(const Vector2<int>& begin, const Vector2<int>& end) const;

  /** Estimates the angle of the gray-gradient at the point by a Sobel Operator. */
  double getPointsAngle(const Vector2<int>& point) const;

  /** */
  double calculateMeanAngle(double angle1, double angle2) const;

};

#endif  /* _ScanLineEdgelDetectorDifferential_H_ */

