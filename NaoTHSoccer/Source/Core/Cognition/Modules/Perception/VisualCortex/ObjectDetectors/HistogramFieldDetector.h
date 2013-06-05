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
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/SpiderScan.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histograms.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"

#define LINE_THICKNESS 10//4


BEGIN_DECLARE_MODULE(HistogramFieldDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
//  REQUIRE(ColorTable64)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
  REQUIRE(Histograms)
  REQUIRE(HistogramsTop)

  PROVIDE(FieldPercept)
  PROVIDE(FieldPerceptTop)
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
  };

private:
 CameraInfo::CameraID cameraID;

 void getFieldRectFromHistogram(Vector2<int>& min, Vector2<int>& max);
  FieldPercept::FieldRect largestAreaRectangle;

  ColorClasses::Color fieldColor;
  ColorClasses::Color lineColor;

  const ColorClassificationModel& getColorClassificationModel() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getColorClassificationModelTop();
    }
    else
    {
      return getColorClassificationModel();
    }
  };

  const Image& getImage_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getImageTop();
    }
    else
    {
      return getImage();
    }
  };
  
  const ColoredGrid& getColoredGrid_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getColoredGridTop();
    }
    else
    {
      return getColoredGrid();
    }
  };

  const CameraMatrix& getCameraMatrix_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getCameraMatrixTop();
    }
    else
    {
      return getCameraMatrix();
    }
  };

  const ArtificialHorizon& getArtificialHorizon_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getArtificialHorizonTop();
    }
    else
    {
      return getArtificialHorizon();
    }
  };

  const Histograms& getHistograms_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getHistogramsTop();
    }
    else
    {
      return getHistograms();
    }
  };

  FieldPercept& getFieldPercept_()
  {
    if(cameraID == CameraInfo::Top)
    {
      return getFieldPerceptTop();
    }
    else
    {
      return getFieldPercept();
    }
  };
};//end class HistogramFieldDetector


#endif // __HistogramFieldDetector_H_
