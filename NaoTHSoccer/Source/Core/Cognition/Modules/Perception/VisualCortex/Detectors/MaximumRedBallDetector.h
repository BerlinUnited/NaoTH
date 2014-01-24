/**
* @file MaximumRedBallDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* @author <a href="mailto:woltersd@informatik.hu-berlin.de">Peter Woltersdorf</a>
* Definition of class MaximumRedBallDetector
*/

#ifndef _MaximumRedBallDetector_H_
#define _MaximumRedBallDetector_H_

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

#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/ColoredGrid.h"
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

BEGIN_DECLARE_MODULE(MaximumRedBallDetector)
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

  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)
END_DECLARE_MODULE(MaximumRedBallDetector)


class MaximumRedBallDetector: private MaximumRedBallDetectorBase
{
public:
  MaximumRedBallDetector();
  ~MaximumRedBallDetector(){};

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
  bool findMaximumRedPoint(Vector2i& peakPos);

  Vector2i getCenterOfMass (BallPointList& goodPoints);
  Vector2d estimatePositionBySize();
  bool calculateCircle( const BallPointList& ballPoints, Vector2d& center, double& radius );
  bool findBall();
  bool getBestModel(const BallPointList& pointList, const Vector2i& start);
  bool checkIfPixelIsOrange (const Pixel& pixel);
  void clearDublicatePoints (BallPointList& ballPoints);
  bool getBestBallBruteForce(const BallPointList& pointList, const Vector2i& start, Vector2d& centerBest, double& radiusBest);
  bool getBestBallRansac(const BallPointList& pointList, const Vector2i& start, Vector2d& centerBest, double& radiusBest);
  void drawUsedPoints(const BallPointList& pointList);
  
  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("MaximumRedBallDetectorParameters")
    {
      PARAMETER_REGISTER(gradientThreshold) = 20;
      PARAMETER_REGISTER(meanThreshold) = 60;
      PARAMETER_REGISTER(stepSize) = 4;
      PARAMETER_REGISTER(percentOfRadius) = 0.8;
      PARAMETER_REGISTER(ransacPercentValid) = 0.05;	  
	    PARAMETER_REGISTER(maxBlueValue) = 60;
      PARAMETER_REGISTER(maxRedValue) = 100;
      PARAMETER_REGISTER(minSizeInImage) = 3;
      PARAMETER_REGISTER(maxSizeInImage) = 130;
      PARAMETER_REGISTER(maxRansacTries) = 40;
      PARAMETER_REGISTER(maxScanlineSteps) = 50;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    int meanThreshold;
    int gradientThreshold;
    int stepSize;
    double percentOfRadius;
    double ransacPercentValid;
	  int maxBlueValue;
    int maxRedValue;
    int minSizeInImage;
    int maxSizeInImage;
    int maxRansacTries;
    int maxScanlineSteps;    
  };

  Parameters params;

  // double cam stuff
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, Image);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, OverTimeHistogram);

  DOUBLE_CAM_PROVIDE(MaximumRedBallDetector, BallPercept);
          
};//end class MaximumRedBallDetector

#endif // _MaximumRedBallDetector_H_
