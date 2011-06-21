/**
* @file RobotDetector.h
*
* @author Kirill Yasinovskiy
* Definition of class RobotDetector
*/

#ifndef _RobotDetector_h_
#define _RobotDetector_h_

#include <vector> 

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/ImageProcessing/SpiderScan.h"
#include "Tools/Math/PointList.h"
#include "Tools/DataStructures/OccupancyGrid.h"
#include "Tools/DataStructures/Area.h"
#include "Tools/Math/Polygon.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Moments2.h"

//Perception
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/BlobFinder.h"
#include "Tools/ImageProcessing/BlobList.h"


//Cognition
#include "Cognition/Cognition.h"

//Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Perception/CameraMatrix.h"


// Module-Declaration

BEGIN_DECLARE_MODULE(RobotDetector)
  REQUIRE(Image)
  REQUIRE(ColorTable64)
  REQUIRE(CameraMatrix)
  REQUIRE(FieldPercept) 
  REQUIRE(ColoredGrid)
  REQUIRE(FieldInfo)

  PROVIDE(PlayersPercept)
END_DECLARE_MODULE(RobotDetector)

//Constraints definition section:
//Please, don't change this parameters,
//if you don't know what they mean
#define MAX_MARKER_NUMBER 20
#define MARKER_MIN_SIZE 30
#define BLOB_MIN_MOMENT 3
#define ABOVE_WHITE_RATIO 0.6f
#define BELOW_WHITE_RATIO 0.6f
#define GREEN_GROUND_RATIO 0.8f

class RobotDetector: public RobotDetectorBase
{
public:
  // default constructor
  RobotDetector();
  // default destructor
  virtual ~RobotDetector(){};
  virtual void execute();
protected:
private:
  //variables
  bool redColors[ColorClasses::numOfColors];
  bool blueColors[ColorClasses::numOfColors];
  bool searchColors[ColorClasses::numOfColors];
  BlobList redBlobs;
  BlobList blueBlobs;
  BlobList blobs;
  BlobFinder theBlobFinder;
  ColoredGrid coloredGrid;
  Math::Polygon<4> searchArea;
  typedef Math::Polygon<MARKER_MIN_SIZE> poly;
  //define Marker class
  class Marker
  {
  public:
    //default constructor
    Marker()
      :
    area(0),
    eccentricity(0),
    angle(0)
      {};
    //default destructor
    ~Marker(){};
    poly polygon;
    double area;
    double eccentricity;
    double angle;
    Vector2<double> majorAxis;
    Vector2<double> minorAxis;
    Vector2<double> cog;
    Moments2<2> moments;
    GameData::TeamColor color;
  }; // end class Marker

  unsigned int resolutionWidth;
  unsigned int resolutionHeight;
  std::vector<Marker> blueMarkers;
  std::vector<Marker> redMarkers;
  



  bool drawScanLinesMarker;
  bool drawScanLinesRobot;
  int maxColorPointsToSkip; // maximum number of non search color ...

  //detections functions
  inline void detectRobotMarkers();
  inline void findMarkerPoly(Vector2<int> cog, ColorClasses::Color color);
  inline void findBlobs();
  inline bool checkMarkerPoly(Marker& marker);
  inline bool evaluateMarkerEnvironment(Marker& marker);
  inline void detectRobots();

  //help functions
  inline void estimateArea(Marker& marker);
  inline void estimateMoments(Marker& marker);
  inline double findGreenRatio(int yCoord, int xStart, int xEnd, int stepSize);

  //scan functions
  inline void scanLine(Vector2<int> start, Vector2<int>& direction, int maxColorPointsToSkip, ColorClasses::Color searchColor, Vector2<int>& point, bool draw);
  inline bool isSearchColor(ColorClasses::Color color, ColorClasses::Color searchColor);
  inline bool pixelInSearchArea(Vector2<int>& pixel);

};

#endif // RobotDetector_h_