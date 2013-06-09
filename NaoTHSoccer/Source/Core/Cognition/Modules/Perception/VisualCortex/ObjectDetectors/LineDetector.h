/**
* @file LineDetector.h
*
* @author <a href="mailto:mohr@informatik.hu-berlin.de">Christian Mohr</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of class LineDetector
*/

#ifndef _LineDetector_H_
#define _LineDetector_H_

#include <ModuleFramework/Module.h>

#include <vector>

#include "Tools/Math/Line.h"
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Matrix_mxn.h"
#include "Tools/Math/Geometry.h"

#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/LineDetectorConstParameters.h"
#include "Tools/ImageProcessing/ClusteredLine.h"
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/ImageProcessing/ColoredGrid.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/ObjectPercept.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Modeling/ColorClassificationModel.h"

BEGIN_DECLARE_MODULE(LineDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
//  REQUIRE(ColorTable64)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(CurrentCameraSettingsTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
  REQUIRE(FieldInfo)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(FrameInfo)

  PROVIDE(LinePercept)
  PROVIDE(LinePerceptTop)
  PROVIDE(ObjectPercept)
  PROVIDE(ObjectPerceptTop)
END_DECLARE_MODULE(LineDetector)


class LineDetector: private LineDetectorBase
{
public:
  LineDetector();
  ~LineDetector(){};

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Bottom);
  };
 
private:
  CameraInfo::CameraID cameraID;
  vector<LinePercept::LineSegmentImage> lineSegments;

  /** estimate the line segments from edgels */
  void clusterEdgels(const vector<Edgel>& edgelList);

  /** */
  void expandLines();

  /** */
  void estimateCorners();
  
  /** */
  void classifyIntersections();

  /** */
  void setLinePercepts();
  

  // TEST
  void analyzeEndPoints();

  /** scan helpers */
  void scanAlongLine(Vector2<int>& linePoint, const Vector2<int>& direction, ColorClasses::Color markColor);
  void scanAlongLine(Vector2<int>& linePoint, BresenhamLineScan& scanLine, ColorClasses::Color markColor);

  int cameraBrighness;
  unsigned int edgelBrightnessLevel;

  const Image& getImage() const
  {
    if(cameraID == CameraInfo::Top) {
      return LineDetectorBase::getImageTop();
    } else {
      return LineDetectorBase::getImage();
    }
  }

  const ColorClassificationModel& getColorTable64() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getColorClassificationModelTop();
    }
    else
    {
      return LineDetectorBase::getColorClassificationModel();
    }
  };

  const CameraSettings& getCurrentCameraSettings() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getCurrentCameraSettingsTop();
    }
    else
    {
      return LineDetectorBase::getCurrentCameraSettings();
    }
  };

  const CameraMatrix& getCameraMatrix() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getCameraMatrixTop();
    }
    else
    {
      return LineDetectorBase::getCameraMatrix();
    }
  };

  const ArtificialHorizon& getArtificialHorizon() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getArtificialHorizonTop();
    }
    else
    {
      return LineDetectorBase::getArtificialHorizon();
    }
  };

  const FieldPercept& getFieldPercept() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getFieldPerceptTop();
    }
    else
    {
      return LineDetectorBase::getFieldPercept();
    }
  };

  const ColoredGrid& getColoredGrid() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getColoredGridTop();
    }
    else
    {
      return LineDetectorBase::getColoredGrid();
    }
  };

  const ScanLineEdgelPercept& getScanLineEdgelPercept() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getScanLineEdgelPerceptTop();
    }
    else
    {
      return LineDetectorBase::getScanLineEdgelPercept();
    }
  };

  LinePercept& getLinePercept() 
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getLinePerceptTop();
    }
    else
    {
      return LineDetectorBase::getLinePercept();
    }
  };

  ObjectPercept& getObjectPercept() 
  {
    if(cameraID == CameraInfo::Top)
    {
      return LineDetectorBase::getObjectPerceptTop();
    }
    else
    {
      return LineDetectorBase::getObjectPercept();
    }
  };

};//end class LineDetector

#endif // __LineDetector_H_
