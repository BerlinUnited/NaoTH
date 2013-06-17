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
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/BodyContour.h"


#include <Tools/DataStructures/ParameterList.h>


BEGIN_DECLARE_MODULE(ScanLineEdgelDetectorDifferential)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  //REQUIRE(CurrentCameraSettings)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

  PROVIDE(ScanLineEdgelPercept)
  PROVIDE(ScanLineEdgelPerceptTop)
END_DECLARE_MODULE(ScanLineEdgelDetectorDifferential)


class ScanLineEdgelDetectorDifferential : private ScanLineEdgelDetectorDifferentialBase
{
public:
  ScanLineEdgelDetectorDifferential();
  virtual ~ScanLineEdgelDetectorDifferential();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);

    CANVAS_PX_BOTTOM;
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("ScanLineParameters")
    {
      PARAMETER_REGISTER(brightness_threshold) = 6;
      PARAMETER_REGISTER(scanline_count) = 31;
      PARAMETER_REGISTER(pixel_border_y) = 3;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    // threshold for detection of the jumps in the Y channel
    int brightness_threshold;

    // number of scanlines
    int scanline_count;

    // don't scan the lower lines in the image
    int pixel_border_y;
  } theParameters;

private:
  CameraInfo::CameraID cameraID;
  double vertical_confidence[naoth::IMAGE_HEIGHT];

  void add_edgel(int x, int y) {
    Edgel edgel;
    edgel.point.x = x;
    edgel.point.y = y;
    edgel.angle = getPointsAngle(edgel.point);
    getScanLineEdgelPercept().edgels.push_back(edgel);
  }

  /** scans at given x-coordinate to the top & cancels at field end. Starts at bottom line. */
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end) ;

  /** */
  ColorClasses::Color estimateColorOfSegment(const Vector2<int>& begin, const Vector2<int>& end) const;

  /** Estimates the angle of the gray-gradient at the point by a Sobel Operator. */
  double getPointsAngle(const Vector2<int>& point) const;

  /** */
  double calculateMeanAngle(double angle1, double angle2) const;




  const Image& getImage() const
  {
    if(cameraID == CameraInfo::Top) {
      return ScanLineEdgelDetectorDifferentialBase::getImageTop();
    } else {
      return ScanLineEdgelDetectorDifferentialBase::getImage();
    }
  }

  const ColorClassificationModel& getColorClassificationModel() const
  {
    if(cameraID == CameraInfo::Top) {
      return ScanLineEdgelDetectorDifferentialBase::getColorClassificationModelTop();
    } else {
      return ScanLineEdgelDetectorDifferentialBase::getColorClassificationModel();
    }
  }

  const CameraMatrix& getCameraMatrix() const
  {
    if(cameraID == CameraInfo::Top) {
      return ScanLineEdgelDetectorDifferentialBase::getCameraMatrixTop();
    } else {
      return ScanLineEdgelDetectorDifferentialBase::getCameraMatrix();
    }
  }

  const ArtificialHorizon& getArtificialHorizon() const
  {
    if(cameraID == CameraInfo::Top) {
      return ScanLineEdgelDetectorDifferentialBase::getArtificialHorizonTop();
    } else {
      return ScanLineEdgelDetectorDifferentialBase::getArtificialHorizon();
    }
  }

  const FieldPercept& getFieldPercept() const
  {
    if(cameraID == CameraInfo::Top) {
      return ScanLineEdgelDetectorDifferentialBase::getFieldPerceptTop();
    } else {
      return ScanLineEdgelDetectorDifferentialBase::getFieldPercept();
    }
  }

  const BodyContour& getBodyContour() const
  {
    if(cameraID == CameraInfo::Top) {
      return ScanLineEdgelDetectorDifferentialBase::getBodyContourTop();
    } else {
      return ScanLineEdgelDetectorDifferentialBase::getBodyContour();
    }
  }

  ScanLineEdgelPercept& getScanLineEdgelPercept()
  {
    if(cameraID == CameraInfo::Top) {
      return ScanLineEdgelDetectorDifferentialBase::getScanLineEdgelPerceptTop();
    } else {
      return ScanLineEdgelDetectorDifferentialBase::getScanLineEdgelPercept();
    }
  }

};

#endif  /* _ScanLineEdgelDetectorDifferential_H_ */

