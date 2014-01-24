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

#include "LineDetectorConstParameters.h"

#include "Tools/Math/Line.h"
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Matrix_mxn.h"
#include "Tools/Math/Geometry.h"

#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/ClusteredLine.h"
#include "Tools/ImageProcessing/Edgel.h"

#include "Representations/Infrastructure/ColoredGrid.h"
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

#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(LineDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
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
     execute(CameraInfo::Top);
  }
 
private:
  CameraInfo::CameraID cameraID;
  std::vector<LinePercept::LineSegmentImage> lineSegments;

  /** estimate the line segments from edgels */
  void clusterEdgels(const std::vector<DoubleEdgel>& edgelList);

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
  void scanAlongLine(Vector2i& linePoint, const Vector2i& direction, ColorClasses::Color markColor);
  void scanAlongLine(Vector2i& linePoint, BresenhamLineScan& scanLine, ColorClasses::Color markColor);

  int cameraBrighness;
  unsigned int edgelBrightnessLevel;


  // double cam interface
  inline const CameraSettings& getBlobFinder() const {
    if(cameraID == CameraInfo::Top) {
      return getCurrentCameraSettingsTop();
    } else { 
      return getCurrentCameraSettings();
    }
  }

  DOUBLE_CAM_REQUIRE(LineDetector,Image);
  DOUBLE_CAM_REQUIRE(LineDetector,ColoredGrid);
  DOUBLE_CAM_REQUIRE(LineDetector,ColorClassificationModel);
  DOUBLE_CAM_REQUIRE(LineDetector,CameraMatrix);
  DOUBLE_CAM_REQUIRE(LineDetector,ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(LineDetector,FieldPercept);
  DOUBLE_CAM_REQUIRE(LineDetector,ScanLineEdgelPercept);
  
  DOUBLE_CAM_PROVIDE(LineDetector,LinePercept);
  DOUBLE_CAM_PROVIDE(LineDetector,ObjectPercept);

};//end class LineDetector

#endif // _LineDetector_H_
