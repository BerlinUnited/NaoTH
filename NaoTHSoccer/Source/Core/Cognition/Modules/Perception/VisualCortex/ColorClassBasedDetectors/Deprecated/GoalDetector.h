/**
* @file GoalDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GoalDetector
*/

#ifndef _GoalDetector_H_
#define _GoalDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/ColoredGrid.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Tools/Math/Polygon.h"
#include "Tools/Math/Moments2.h"

#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/BlobFinder.h"

#include "Tools/DataStructures/OccupancyGrid.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(GoalDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  
  PROVIDE(GoalPercept)
  PROVIDE(GoalPerceptTop)
END_DECLARE_MODULE(GoalDetector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class GoalDetector: public GoalDetectorBase
{

public:
  GoalDetector();
  ~GoalDetector(){};

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Top);
     execute(CameraInfo::Bottom);
  };
 
private:
  CameraInfo::CameraID cameraID;

  class Blob;

  class Candidate
  {
  public:
    double angle;
    double width;
    Vector2i point;
    ColorClasses::Color color;
  };

  static const int maxNumberOfCandidates = 10;

  BlobFinder blobFinder;
  BlobFinder blobFinderBottom;

  /** */
  int scanForCandidates(
    const Vector2i& startPoint, 
    const Vector2i& endPoint, 
    Candidate (&candidates)[maxNumberOfCandidates]);
  
  /** */
  void estimatePostsByScanlines(
    const Candidate (&candidates)[maxNumberOfCandidates],
    int numberOfCandidates,
    vector<GoalPercept::GoalPost>& postvector);

  Vector2i extendCandidate(ColorClasses::Color color, const Vector2i& start);

  /** */
  void estimatePostsByBlobs(
    const Candidate (&candidates)[maxNumberOfCandidates],
    int numberOfCandidates,
    vector<GoalPercept::GoalPost>& postvector);

  /** */
  Vector2i scanColorLine(
    ColorClasses::Color color, 
    const Vector2i& start, 
    const Vector2i& direction);

  /** */
  bool checkIfPostReliable(Vector2i& post);

  /** */
  double getMajorAxis(const ColorClasses::Color goalColor, Vector2d& centroid);

  /** check wether a given pixel (x,y) has a given color */
  bool hasColor (ColorClasses::Color& color, int x, int y);

  /** */
  Blob spiderExpandArea(
    const Vector2i& startingPoint, 
    ColorClasses::Color color, 
    int maxPointsToSkip, 
    int maxLengthOfBeams);

  /** */
  double getPointsAngle(const Vector2i& point);
  double calculateMeanAngle(const double& angle1, const double& angle2);

  class Blob
  {
  public:
    Blob() : greenPointFound(false) {};

    Blob(const Math::Polygon<8>& v, bool greenPoint) : vertices(v), greenPointFound(greenPoint) {};

    Math::Polygon<8> vertices;
    bool greenPointFound;

    void add(Vector2i& point) { vertices.add(point); }
    double getArea() { return vertices.getArea(); }
    const Vector2i& getClosestPoint(const Vector2i& reference) { return vertices.getClosestPoint(reference); }

  };//end class Blob

  BlobFinder& getBlobFinder()
  {
    return cameraID == CameraInfo::Top?blobFinder:blobFinderBottom;
  };

  DOUBLE_CAM_REQUIRE(GoalDetector, Image);
  DOUBLE_CAM_REQUIRE(GoalDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(GoalDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(GoalDetector, ColorClassificationModel);
  DOUBLE_CAM_REQUIRE(GoalDetector, ColoredGrid);

  DOUBLE_CAM_PROVIDE(GoalDetector, GoalPercept);

};//end class GoalDetector

#endif // __GoalDetector_H_
