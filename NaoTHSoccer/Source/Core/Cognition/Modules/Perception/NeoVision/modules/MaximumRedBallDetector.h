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

#include "Representations/Perception/BodyContour.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/FieldPercept.h"
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/FieldColorPercept.h"

// tools
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/GradientSpiderScan.h"
#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(MaximumRedBallDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(KinematicChain)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(BaseColorRegionPercept)
  REQUIRE(BaseColorRegionPerceptTop)
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
  typedef PointList<20> BallPointList;
  BallPointList goodPoints;
  BallPointList badPoints;

  BallPointList bestPoints;
  /************************************/
  BallPointList possibleModells[570];
  /***********************************/
  void findMaximumRedPoint(Vector2<int>& peakPos);
  Vector2<int> getCenterOfMass (BallPointList& goodPoints);
  Vector2d estimatePositionBySize();
  bool calculateCircle( const BallPointList& ballPoints, Vector2<double>& center, double& radius );
  bool findBall ();
  bool getBestModel(BallPointList& pointList);
  bool checkIfPixelIsOrange (Vector2d coord);
  void clearDublicatePoints ( BallPointList& ballPoints);
  bool getBestBallBruteForce(BallPointList& pointList, Vector2<double>& centerBest, double& radiusBest);
  bool getBestBallRansac(BallPointList& pointList, Vector2<double>& centerBest, double& radiusBest);
  
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
  };

  Parameters params;


  // double cam stuff
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, Image);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(MaximumRedBallDetector, BaseColorRegionPercept);

  DOUBLE_CAM_PROVIDE(MaximumRedBallDetector, BallPercept);
          
};//end class MaximumRedBallDetector

#endif // _MaximumRedBallDetector_H_
