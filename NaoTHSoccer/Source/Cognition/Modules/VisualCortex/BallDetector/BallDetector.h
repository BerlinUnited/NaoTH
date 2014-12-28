/**
* @file BallDetector.h
*
* Definition of class BallDetector
*/

#ifndef _BallDetector_H_
#define _BallDetector_H_

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
//#include "Representations/Infrastructure/ColoredGrid.h"
//#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/FieldPercept.h"
//#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/Histograms.h"

// tools
#include "Tools/ImageProcessing/GradientSpiderScan.h"
#include "Tools/DoubleCamHelpers.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"


BEGIN_DECLARE_MODULE(BallDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  //REQUIRE(ArtificialHorizon)
  //REQUIRE(ArtificialHorizonTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  //REQUIRE(BodyContour)
  //REQUIRE(BodyContourTop)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  //REQUIRE(OverTimeHistogram)
  //REQUIRE(OverTimeHistogramTop)
  REQUIRE(GoalPostHistograms)

  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)
END_DECLARE_MODULE(BallDetector)


class BallDetector: private BallDetectorBase
{
public:
  BallDetector();
  ~BallDetector(){}

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
 
private:
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("BallDetectorParameters")
    {
      PARAMETER_REGISTER(stepSize) = 2;    
      PARAMETER_REGISTER(minOffsetToFieldY) = 100;
      PARAMETER_REGISTER(minOffsetToGoalV) = 10;
      PARAMETER_REGISTER(minOffsetToFieldV) = 10;
      PARAMETER_REGISTER(mitUVDifference) = 50;
      PARAMETER_REGISTER(thresholdGradientUV) = 6;
      
      syncWithConfig();
    }

    ~Parameters()
    {
    }

	  int stepSize;

    int minOffsetToFieldY;
    int minOffsetToFieldV;
    int minOffsetToGoalV;
    int mitUVDifference;
    
    int thresholdGradientUV;

  } params;


private:
  inline bool isOrange(const Pixel& pixel) const {
    return
      pixel.y + params.minOffsetToFieldY > getFieldColorPercept().histogramField.y && // brighter than darkest acceptable green
      pixel.v > pixel.u + params.mitUVDifference && // y-u has to be high (this filter out the jerseys)
      pixel.v > getFieldColorPercept().range.getMax().v + params.minOffsetToFieldV &&
      pixel.v > getGoalPostHistograms().histogramV.mean + params.minOffsetToGoalV; 
  }

  bool findMaximumRedPoint(std::vector<Vector2i>& points) const;
  bool scanForEdges(const Vector2i& start, const Vector2d& direction, std::vector<Vector2i>& endpoint) const;
  void spiderScan(const Vector2i& start, std::vector<Vector2i>& endPoints) const;
  double estimatedBallRadius(const Vector2i& point) const;
  
  void calculateBallPercept(const Vector2i& center, double radius);
  void estimateCircleSimple(const std::vector<Vector2i>& endPoints, Vector2d& center, double& radius) const;
  
private: //data members
  std::vector<Vector2i> listOfRedPoints;
  std::vector<Vector2i> ballEndPoints;

private:     
  
  DOUBLE_CAM_PROVIDE(BallDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BallDetector, Image);
  DOUBLE_CAM_REQUIRE(BallDetector, CameraMatrix);
  //DOUBLE_CAM_REQUIRE(BallDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(BallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(BallDetector, FieldPercept);
  //DOUBLE_CAM_REQUIRE(BallDetector, BodyContour);
  //DOUBLE_CAM_REQUIRE(BallDetector, OverTimeHistogram);
 
  DOUBLE_CAM_PROVIDE(BallDetector, BallPercept);
          
};//end class BallDetector

#endif // _BallDetector_H_
