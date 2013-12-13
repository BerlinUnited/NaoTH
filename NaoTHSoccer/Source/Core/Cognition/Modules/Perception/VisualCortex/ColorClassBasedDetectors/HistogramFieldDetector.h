/**
* @file HistogramFieldDetector.h
*
* @author <a href="mailto:mohr@informatik.hu-berlin.de">Christian Mohr</a>
* Definition of class HistogramFieldDetector
*/

#ifndef __HistogramFieldDetector_H_
#define __HistogramFieldDetector_H_

#include <ModuleFramework/Module.h>

#include <vector>

#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Polygon.h"

#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/ArtificialHorizon.h"

#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/SpiderScan.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histograms.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"

#include "Tools/DoubleCamHelpers.h"

// TODO: make it parameter or constant
#define LINE_THICKNESS 10//4


BEGIN_DECLARE_MODULE(HistogramFieldDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
  REQUIRE(ColorClassesHistograms)
  REQUIRE(ColorClassesHistogramsTop)

  PROVIDE(FieldPerceptRaw)
  PROVIDE(FieldPerceptRawTop)
END_DECLARE_MODULE(HistogramFieldDetector)

class HistogramFieldDetector: private HistogramFieldDetectorBase
{
public:
  HistogramFieldDetector();
  ~HistogramFieldDetector(){}

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Bottom);
     execute(CameraInfo::Top);
  }

private:
  CameraInfo::CameraID cameraID;

  void getFieldRectFromHistogram(Vector2i& min, Vector2i& max);
  FieldPercept::FieldPoly largestAreaRectangle;

  ColorClasses::Color fieldColor;
  ColorClasses::Color lineColor;

  DOUBLE_CAM_REQUIRE(HistogramFieldDetector, Image);
  DOUBLE_CAM_REQUIRE(HistogramFieldDetector, ColoredGrid);
  DOUBLE_CAM_REQUIRE(HistogramFieldDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(HistogramFieldDetector, ColorClassesHistograms);

  DOUBLE_CAM_PROVIDE(HistogramFieldDetector, FieldPerceptRaw);

};//end class HistogramFieldDetector


#endif // __HistogramFieldDetector_H_
