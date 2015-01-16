/**
* @file MaxRedHogBallDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* @author <a href="mailto:woltersd@informatik.hu-berlin.de">Peter Woltersdorf</a>
* Definition of class MaxRedHogBallDetector
*/

#ifndef _MaxRedHogBallDetector_H_
#define _MaxRedHogBallDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/PointList.h>
#include <Tools/DataStructures/OccupancyGrid.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>
#include "Tools/ImageProcessing/SimpleHogFeature.h"
#include "Tools/ImageProcessing/SimpleHogFeatureDetector.h"

#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/Histograms.h"

// tools
#include "Tools/ImageProcessing/GradientSpiderScan.h"
#include "Tools/DoubleCamHelpers.h"

//debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"


#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#endif


BEGIN_DECLARE_MODULE(MaxRedHogBallDetector)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(OverTimeHistogram)
  REQUIRE(OverTimeHistogramTop)
  REQUIRE(GoalPostHistograms)

  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)
END_DECLARE_MODULE(MaxRedHogBallDetector)


class MaxRedHogBallDetector: private MaxRedHogBallDetectorBase
{
public:
  MaxRedHogBallDetector();
  ~MaxRedHogBallDetector();

  // override the Module execute method
  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Top);
     execute(CameraInfo::Bottom);
  };
 
private:
  CameraInfo::CameraID cameraID;
  double dynamicThresholdY;

  typedef PointList<20> BallPointList;
  BallPointList goodPoints;
  BallPointList badPoints;
  BallPointList bestPoints;

  BallPointList bestMatchedBallpoints;

  /************************************/
  std::vector<BallPointList> possibleModells;
  /***********************************/

  CvDTree dtree;
  cv::Mat trainInput;
  cv::Mat trainOutput;
  bool dtreeTrained;
  std::string modelFileName;


  bool findMaximumRedPoint(Vector2i& peakPos);

  Vector2i getCenterOfMass (BallPointList& goodPoints);
  Vector2d estimatePositionBySize();
  bool calculateCircle( const BallPointList& ballPoints, Vector2d& center, double& radius );
  bool findBall();
  bool getBestModel(const BallPointList& pointList, const Vector2i& start);
  void clearDublicatePoints (BallPointList& ballPoints);
  bool getBestBallBruteForce(const BallPointList& pointList, const Vector2i& start, Vector2d& centerBest, double& radiusBest);
  bool getBestBallRansac(const BallPointList& pointList, const Vector2i& start, Vector2d& centerBest, double& radiusBest);
  void drawUsedPoints(const BallPointList& pointList);
  bool checkBallForGreen (Vector2d center, double radius);
  void setToLastPointInImage(Vector2i& point);
  
  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("MaxRedHogBallDetectorParameters")
    {
      PARAMETER_REGISTER(checkBallForGreen) = 0.9;      
      PARAMETER_REGISTER(gradientThreshold) = 20;
      PARAMETER_REGISTER(minPercentOfPointsUsed) = 0.6;
      PARAMETER_REGISTER(stepSize) = 4;
      PARAMETER_REGISTER(ransacPercentValid) = 0.05;    
      PARAMETER_REGISTER(minRadiusInImage) = 3;
      PARAMETER_REGISTER(maxRadiusInImage) = 60;
      PARAMETER_REGISTER(maxRansacTries) = 40;
      PARAMETER_REGISTER(maxScanlineSteps) = 50;
      PARAMETER_REGISTER(maxU) = 128;

      syncWithConfig();

    }

    ~Parameters() {}

    double checkBallForGreen;
    double minPercentOfPointsUsed;
    int gradientThreshold;
    int stepSize;
  //  double percentOfRadius;
    double ransacPercentValid;
    int minRadiusInImage;
    int maxRadiusInImage;
    int maxRansacTries;
    int maxScanlineSteps;
    int maxU;
 //   int minY;
  };

  Parameters params;

  // double cam stuff
  DOUBLE_CAM_REQUIRE(MaxRedHogBallDetector, Image);
  DOUBLE_CAM_REQUIRE(MaxRedHogBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(MaxRedHogBallDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(MaxRedHogBallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(MaxRedHogBallDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(MaxRedHogBallDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(MaxRedHogBallDetector, OverTimeHistogram);

  DOUBLE_CAM_PROVIDE(MaxRedHogBallDetector, DebugImageDrawings);
 
  DOUBLE_CAM_PROVIDE(MaxRedHogBallDetector, BallPercept);
          
};//end class MaxRedHogBallDetector

#endif // _MaxRedHogBallDetector_H_
