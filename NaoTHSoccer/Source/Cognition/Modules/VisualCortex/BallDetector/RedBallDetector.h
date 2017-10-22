/**
* @file RedBallDetector.h
*
* Definition of class RedBallDetector
*/

#ifndef _RedBallDetector_H_
#define _RedBallDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/PointList.h>
#include <Tools/DataStructures/OccupancyGrid.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>

#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/MultiBallPercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"


BEGIN_DECLARE_MODULE(RedBallDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(GoalPostHistograms) // is this still needed?

  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)

  PROVIDE(MultiBallPercept)
END_DECLARE_MODULE(RedBallDetector)


class RedBallDetector: private RedBallDetectorBase
{
public:
  RedBallDetector();
  ~RedBallDetector();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    getMultiBallPercept().reset();
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
 
private:
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("RedBallDetector")
    {
      PARAMETER_REGISTER(stepSize) = 2;    
      PARAMETER_REGISTER(maxBorderBrightness) = 70;
      PARAMETER_REGISTER(minOffsetToFieldY) = 100;
      //PARAMETER_REGISTER(minOffsetToGoalV) = 10;
      PARAMETER_REGISTER(minOffsetToFieldV) = 10;
      PARAMETER_REGISTER(mitUVDifference) = 50;
      PARAMETER_REGISTER(thresholdGradientUV) = 6;
      PARAMETER_REGISTER(thresholdSanityCheck) = 0.5;
      
      syncWithConfig();
    }

    ~Parameters()
    {
    }

	  int stepSize;

    int maxBorderBrightness;
    int minOffsetToFieldY;
    int minOffsetToFieldV;
    //int minOffsetToGoalV;
    int mitUVDifference;
    
    int thresholdGradientUV;
    double thresholdSanityCheck;

  } params;


private:
  inline bool isOrange(const Pixel& pixel) const {
    return getFieldColorPercept().isRedColor(pixel);
  }

  bool findMaximumRedPoint(std::vector<Vector2i>& points) const;
  bool scanForEdges(const Vector2i& start, const Vector2d& direction, std::vector<Vector2i>& endpoint) const;
  bool spiderScan(const Vector2i& start, std::vector<Vector2i>& endPoints) const;
  double estimatedBallRadius(const Vector2i& point) const;
  
  void calculateBallPercept(const Vector2i& center, double radius);
  void estimateCircleSimple(const std::vector<Vector2i>& endPoints, Vector2d& center, double& radius) const;
  bool sanityCheck(const Vector2i& center, double radius);
  
private: //data members
  std::vector<Vector2i> listOfRedPoints;
  std::vector<Vector2i> ballEndPoints;

private:     
  
  DOUBLE_CAM_PROVIDE(RedBallDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(RedBallDetector, Image);
  DOUBLE_CAM_REQUIRE(RedBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(RedBallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(RedBallDetector, FieldPercept);
 
  DOUBLE_CAM_PROVIDE(RedBallDetector, BallPercept);
          
};//end class RedBallDetector

#endif // _RedBallDetector_H_
