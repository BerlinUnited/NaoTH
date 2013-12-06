/**
* @file RobotDetector.h
*
* @author Kirill Yasinovskiy
* Definition of class RobotDetector
*/

#ifndef _RobotDetector_h_
#define _RobotDetector_h_

#include <ModuleFramework/Module.h>

#include <vector> 

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/ImageProcessing/SpiderScan.h"
#include "Tools/Math/PointList.h"
#include "Tools/DataStructures/OccupancyGrid.h"
#include "Tools/DataStructures/Area.h"
#include "Tools/Math/Polygon.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Moments2.h"
#include "Tools/DoubleCamHelpers.h"

//Perception
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/BlobFinder.h"
#include "Tools/ImageProcessing/BlobList.h"

//Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/ColorClassificationModel.h"


// Module-Declaration

BEGIN_DECLARE_MODULE(RobotDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldPercept) 
  REQUIRE(FieldPerceptTop) 
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  REQUIRE(FieldInfo)

  PROVIDE(PlayersPercept)
  PROVIDE(PlayersPerceptTop)
END_DECLARE_MODULE(RobotDetector)

class RobotDetector: public RobotDetectorBase
{
public:
  // default constructor
  RobotDetector();
  // default destructor
  virtual ~RobotDetector(){};
  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);

    //// reset the debug drawing canvas to bottom
    //CANVAS_PX_BOTTOM;
  }

  void execute(const CameraInfo::CameraID id);

protected:
private:

  // id of the camera the module is curently running on
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(RobotDetector,Image);
  DOUBLE_CAM_REQUIRE(RobotDetector,CameraMatrix);
  DOUBLE_CAM_REQUIRE(RobotDetector,FieldPercept);
  //DOUBLE_CAM_REQUIRE(RobotDetector,ColoredGrid);
  DOUBLE_CAM_REQUIRE(RobotDetector,ColorClassificationModel);

  DOUBLE_CAM_PROVIDE(RobotDetector,PlayersPercept);

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("RobotDetectorParameters")
    {
      //Constraints definition section:
      //Please, don't change this parameters,
      //if you don't know what they mean
      PARAMETER_REGISTER(maxScanPointsToSkip) = 4;
      PARAMETER_REGISTER(maxMarkerNumber) = 20;
      PARAMETER_REGISTER(markerMinSize) = 30;
      PARAMETER_REGISTER(blobMinMoment) = 3;
      PARAMETER_REGISTER(aboveWhiteRatio) = 0.5;
      PARAMETER_REGISTER(belowWhiteRatio) = 0.5;
      PARAMETER_REGISTER(greenGroundRatio) = 0.5;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    int maxScanPointsToSkip;
    int maxMarkerNumber;
    int markerMinSize;
    int blobMinMoment;
    double aboveWhiteRatio;
    double belowWhiteRatio;
    double greenGroundRatio;
  };

  Parameters params;
  //variables
  bool redColors[ColorClasses::numOfColors];
  bool blueColors[ColorClasses::numOfColors];
  BlobList redBlobs;
  BlobList blueBlobs;
  BlobList blobs;
  BlobFinder theBlobFinder;
  BlobFinder theBlobFinderTop;
  //ColoredGrid coloredGrid;
  WholeArea searchArea;
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
    double area;
    double eccentricity;
    double angle;
    Vector2<double> majorAxis;
    Vector2<double> minorAxis;
    Vector2<double> cog;
    Moments2<2> moments;
    GameData::TeamColor color;
  }; // end class Marker

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
  inline void detectRobots(const std::vector<Marker>& markers);

  //help functions
  inline void estimateArea(Marker& marker);
  inline void estimateMoments(Marker& marker);
  inline double findGreenRatio(int yCoord, int xStart, int xEnd, int stepSize);

  //scan functions
  inline void scanLine(Vector2<int> start, Vector2<int>& direction, int maxColorPointsToSkip, 
                        ColorClasses::Color searchColor, Vector2<int>& point, bool draw, Marker& marker);
  inline bool isSearchColor(ColorClasses::Color color, ColorClasses::Color searchColor);
  inline bool pixelInSearchArea(const Vector2<int>& pixel) const;


  const ColorClassificationModel& getColorTable64() const
  {
    return getColorClassificationModel();
  }


};

#endif // RobotDetector_h_