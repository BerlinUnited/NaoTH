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
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Tools/Math/Polygon.h"
#include "Tools/Math/Moments2.h"

#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/BlobFinder.h"

#include "Tools/DataStructures/OccupancyGrid.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(GoalDetector)
  REQUIRE(Image)
  REQUIRE(CameraMatrix)
//  REQUIRE(ColorTable64)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColoredGrid)
//  REQUIRE(FieldPercept)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  
  PROVIDE(GoalPercept)
END_DECLARE_MODULE(GoalDetector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class GoalDetector: public GoalDetectorBase
{

public:
  GoalDetector();
  ~GoalDetector(){};

  void execute();

private:
  class Blob;

  class Candidate
  {
  public:
    double angle;
    double width;
    Vector2<int> point;
    ColorClasses::Color color;
  };

  static const int maxNumberOfCandidates = 10;

  BlobFinder blobFinder;

  /** */
  int scanForCandidates(
    const Vector2<int>& startPoint, 
    const Vector2<int>& endPoint, 
    Candidate (&candidates)[maxNumberOfCandidates]);
  
  /** */
  void estimatePostsByScanlines(
    const Candidate (&candidates)[maxNumberOfCandidates],
    int numberOfCandidates,
    vector<GoalPercept::GoalPost>& postvector);

  Vector2<int> extendCandidate(ColorClasses::Color color, const Vector2<int>& start);

  /** */
  void estimatePostsByBlobs(
    const Candidate (&candidates)[maxNumberOfCandidates],
    int numberOfCandidates,
    vector<GoalPercept::GoalPost>& postvector);

  /** */
  Vector2<int> scanColorLine(
    ColorClasses::Color color, 
    const Vector2<int>& start, 
    const Vector2<int>& direction);

  /** */
  bool checkIfPostReliable(Vector2<int>& post);

  /** */
  double getMajorAxis(const ColorClasses::Color goalColor, Vector2<double>& centroid);

  /** check wether a given pixel (x,y) has a given color */
  bool hasColor (ColorClasses::Color& color, int x, int y);

  /** */
  Blob spiderExpandArea(
    const Vector2<int>& startingPoint, 
    ColorClasses::Color color, 
    int maxPointsToSkip, 
    int maxLengthOfBeams);

  /** */
  double getPointsAngle(const Vector2<int>& point);
  double calculateMeanAngle(const double& angle1, const double& angle2);

  class Blob
  {
  public:
    Blob() : greenPointFound(false) {};

    Blob(const Math::Polygon<8>& v, bool greenPoint) : vertices(v), greenPointFound(greenPoint) {};

    Math::Polygon<8> vertices;
    bool greenPointFound;

    void add(Vector2<int>& point) { vertices.add(point); }
    double getArea() { return vertices.getArea(); }
    const Vector2<int>& getClosestPoint(const Vector2<int>& reference) { return vertices.getClosestPoint(reference); }

  };//end class Blob


  const ColorClassificationModel& getColorTable64() const
  {
    return getColorClassificationModel();
  }

};//end class GoalDetector

#endif // __GoalDetector_H_
