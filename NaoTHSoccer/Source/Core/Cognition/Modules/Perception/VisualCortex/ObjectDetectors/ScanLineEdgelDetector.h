/* 
 * File:   ScanLineEdgelDetector.h
 * Author: claas
 *
 * Created on 14. march 2011, 14:22
 */

#ifndef _SCANLINEEDGELDETECTOR_H_
#define _SCANLINEEDGELDETECTOR_H_

#include <ModuleFramework/Module.h>

#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/LineDetectorConstParameters.h"
#include "Tools/ImageProcessing/Edgel.h"

#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/ColorClassificationModel.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/BodyContour.h"

BEGIN_DECLARE_MODULE(ScanLineEdgelDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(CurrentCameraSettingsTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  PROVIDE(ScanLineEdgelPercept)
  PROVIDE(ScanLineEdgelPerceptTop)
END_DECLARE_MODULE(ScanLineEdgelDetector)


class ScanLineEdgelDetector : private ScanLineEdgelDetectorBase
{
public:
  ScanLineEdgelDetector();
  ~ScanLineEdgelDetector();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Bottom);
  };

private:
  CameraInfo::CameraID cameraID;

  void integrated_edgel_detection();
  void iterative_edgel_detection();

  int cameraBrighness;
  unsigned int edgelBrightnessLevel;
  unsigned int edgelGrayThresholdLevel;

  Edgel getEdgel(const Vector2<int>& start, const Vector2<int>& end);
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end);

  double getPointsAngle(Vector2<int>& point);
  double calculateMeanAngle(double angle1,double  angle2);

  const Image& getImage() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getImageTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getImage();
    }
  };
  
  const ColorClassificationModel& getColorClassificationModel() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getColorClassificationModelTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getColorClassificationModel();
    }
  };

  const CameraSettings& getCurrentCameraSettings() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getCurrentCameraSettingsTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getCurrentCameraSettings();
    }
  };

  const CameraMatrix& getCameraMatrix() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getCameraMatrixTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getCameraMatrix();
    }
  };

  const FieldPercept& getFieldPercept() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getFieldPerceptTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getFieldPercept();
    }
  };

  const BodyContour& getBodyContour() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getBodyContourTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getBodyContour();
    }
  };
  
  const ArtificialHorizon& getArtificialHorizon() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getArtificialHorizonTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getArtificialHorizon();
    }
  };

  ScanLineEdgelPercept& getScanLineEdgelPercept()
  {
    if(cameraID == CameraInfo::Top)
    {
      return ScanLineEdgelDetectorBase::getScanLineEdgelPerceptTop();
    }
    else
    {
      return ScanLineEdgelDetectorBase::getScanLineEdgelPercept();
    }
  };

};
#endif  /* _SCANLINEEDGELDETECTOR_H_ */

