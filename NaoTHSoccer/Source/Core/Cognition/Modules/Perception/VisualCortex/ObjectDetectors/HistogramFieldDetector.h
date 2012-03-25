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
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/SpiderScan.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"

#define LINE_THICKNESS 10//4


BEGIN_DECLARE_MODULE(HistogramFieldDetector)
  REQUIRE(Image)
//  REQUIRE(ColorTable64)
  REQUIRE(ColorClassificationModel)
  REQUIRE(CameraMatrix)
  REQUIRE(ColoredGrid)
  REQUIRE(Histogram)

  PROVIDE(FieldPercept)
END_DECLARE_MODULE(HistogramFieldDetector)

class HistogramFieldDetector: private HistogramFieldDetectorBase
{
public:
  HistogramFieldDetector();
  ~HistogramFieldDetector(){}

  void execute();

private:
  void getFieldRectFromHistogram(Vector2<int>& min, Vector2<int>& max);
  FieldPercept::FieldRect largestAreaRectangle;

  ColorClasses::Color fieldColor;
  ColorClasses::Color lineColor;

  const ColorClassificationModel& getColorTable64() const
  {
    return getColorClassificationModel();
  }
};//end class HistogramFieldDetector


#endif // __HistogramFieldDetector_H_
