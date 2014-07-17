/**
* @file StableBallDetector.h
*
* Definition of class StableBallDetector
*/

#ifndef _StableBallDetector_H_
#define _StableBallDetector_H_

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

BEGIN_DECLARE_MODULE(StableBallDetector)
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
END_DECLARE_MODULE(StableBallDetector)


class StableBallDetector: private StableBallDetectorBase
{
public:
  StableBallDetector();
  ~StableBallDetector(){}

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

    Parameters() : ParameterList("StableBallDetectorParameters")
    {
      PARAMETER_REGISTER(stepSize) = 2;    
      PARAMETER_REGISTER(minOffsetToFieldY) = 100;
      PARAMETER_REGISTER(minOffsetToGoalV) = 10;
      PARAMETER_REGISTER(minOffsetV) = 100;
      PARAMETER_REGISTER(mitUVDifference) = 50;
      PARAMETER_REGISTER(thresholdGradientUV) = 6;
      

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

	  int stepSize;
    int minOffsetToFieldY;
    int minOffsetToGoalV;
    int minOffsetV;
    int mitUVDifference;
    int thresholdGradientUV;

  } params;


private:
  inline bool isOrange(const Pixel& pixel) const {
    return
      pixel.y + params.minOffsetToFieldY > getFieldColorPercept().histogramField.y && // brighter than darkest acceptable green
      pixel.v > pixel.u + params.mitUVDifference && // y-u hat to be high (this filter out the jerseys)
      pixel.v > params.minOffsetV &&
      pixel.v > getGoalPostHistograms().histogramV.mean + params.minOffsetToGoalV; 
  }

  bool findMaximumRedPoint(std::vector<Vector2i>& points) const;
  bool scanForEdges(const Vector2i& start, const Vector2d& direction, std::vector<Vector2i>& endpoint) const;
  bool spiderScan(const Vector2i& start, Vector2d& center, double& radius);
  void calculateBallPercept(const Vector2i& center, double radius);
  
private: //data members
  std::vector<Vector2i> listOfRedPoints;

private:
  // double cam stuff
  DOUBLE_CAM_REQUIRE(StableBallDetector, Image);
  DOUBLE_CAM_REQUIRE(StableBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(StableBallDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(StableBallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(StableBallDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(StableBallDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(StableBallDetector, OverTimeHistogram);
 
  DOUBLE_CAM_PROVIDE(StableBallDetector, BallPercept);
          
};//end class StableBallDetector

#endif // _StableBallDetector_H_
