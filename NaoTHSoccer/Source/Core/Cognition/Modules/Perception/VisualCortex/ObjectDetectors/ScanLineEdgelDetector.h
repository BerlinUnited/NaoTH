/* 
 * File:   ScanLineEdgelDetector.h
 * Author: claas
 *
 * Created on 14. März 2011, 14:22
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
  REQUIRE(ColorClassificationModel)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(CameraMatrix)
  REQUIRE(FieldPercept)
  REQUIRE(BodyContour)
  REQUIRE(ArtificialHorizon)

  PROVIDE(ScanLineEdgelPercept)
END_DECLARE_MODULE(ScanLineEdgelDetector)


class ScanLineEdgelDetector : private ScanLineEdgelDetectorBase
{
public:
  ScanLineEdgelDetector();
  ~ScanLineEdgelDetector();

  void execute();

private:

  void integrated_edgel_detection();
  void iterative_edgel_detection();

  int cameraBrighness;
  unsigned int edgelBrightnessLevel;
  unsigned int edgelGrayThresholdLevel;

  Edgel getEdgel(const Vector2<int>& start, const Vector2<int>& end);
  ScanLineEdgelPercept::EndPoint scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end);

  double getPointsAngle(Vector2<int>& point);
  double calculateMeanAngle(double angle1,double  angle2);

};
#endif  /* _SCANLINEEDGELDETECTOR_H_ */

