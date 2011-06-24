/**
* @file LineDetector.h
*
* @author <a href="mailto:mohr@informatik.hu-berlin.de">Christian Mohr</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of class LineDetector
*/

#ifndef __LineDetector_H_
#define __LineDetector_H_

#include <vector>

#include "Cognition/Cognition.h"

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

#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/ObjectPercept.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

BEGIN_DECLARE_MODULE(LineDetector)
  REQUIRE(Image)
  REQUIRE(ColorTable64)
  REQUIRE(CurrentCameraSettings)
  REQUIRE(CameraMatrix)
  REQUIRE(FieldPercept)
  REQUIRE(ColoredGrid)
  REQUIRE(FieldInfo)
  REQUIRE(ScanLineEdgelPercept)

  PROVIDE(LinePercept)
  PROVIDE(ObjectPercept)
END_DECLARE_MODULE(LineDetector)


class LineDetector: private LineDetectorBase
{
public:
  LineDetector();
  ~LineDetector(){};

  void execute();
 
private:

  vector<LinePercept::FieldLineSegment> getLines(vector<Edgel>& edgelList);
  void estimateCorners(vector<LinePercept::FieldLineSegment>& lineSegments);
  void expandLines(vector<LinePercept::FieldLineSegment>& lineSegments);
  bool scanAlongLine(Vector2<int>& linePoint, BresenhamLineScan& scanLine, ColorClasses::Color markColor);
  void classifyIntersections(vector<LinePercept::FieldLineSegment>& lineSegments);

  void setLinePercepts(vector<LinePercept::FieldLineSegment>& lineSegments);
  
  // TEST
  void analyzeEndPoints();

  double calculateListMeanAngle(double list[], int numberOfValues);

  int cameraBrighness;
  unsigned int edgelBrightnessLevel;
};//end class LineDetector

#endif // __LineDetector_H_
