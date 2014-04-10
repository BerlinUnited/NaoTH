/**
* @file SuperBallDetector.h
*
* Definition of class SuperBallDetector
*/

#ifndef _SuperBallDetector_H_
#define _SuperBallDetector_H_

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

BEGIN_DECLARE_MODULE(SuperBallDetector)
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
END_DECLARE_MODULE(SuperBallDetector)


class SuperBallDetector: private SuperBallDetectorBase
{
public:
  SuperBallDetector();
  ~SuperBallDetector(){}

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

    Parameters() : ParameterList("SuperBallDetectorParameters")
    {
      PARAMETER_REGISTER(stepSize) = 2;    
      PARAMETER_REGISTER(minOffsetToFieldY) = 100;
      PARAMETER_REGISTER(minOffsetToGoalV) = 10;
      PARAMETER_REGISTER(minOffsetV) = 100;
      PARAMETER_REGISTER(mitUVDifference) = 50;

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

  } params;

  template<typename T, typename V>
class MaximumScan
{
private:
  V threshhold;
  V value_max;

  // results of the scan
  T& peakMax;
  bool maximum;

public:

  MaximumScan(T& peakMax, V threshhold)
    : threshhold(threshhold),
      value_max(threshhold),
      peakMax(peakMax),
      maximum(false)
  {
  }

  inline bool addValue(const T& point, V value)
  {
    if(maximum) {
      value_max = threshhold;
      maximum = false;
    }

    if(value > value_max)
    {
      value_max = value;
      peakMax = point;
    } else if(value_max > threshhold && value < threshhold) {
      maximum = true;
    }

    return maximum;
  }

  inline bool isMax() { return maximum; }
  inline V maxValue() { return value_max; }
};

private:
  inline bool isOrange(const Pixel& pixel) const {
    return
      pixel.y + params.minOffsetToFieldY > getFieldColorPercept().histogramField.y && // brighter than darkest acceptable green
      params.minOffsetV < pixel.v &&
      pixel.v > pixel.u + params.mitUVDifference; // y-u hat to be high (this filter out the jerseys)
  }

  bool findMaximumRedPoint(Vector2i& peakPos) const;
  void scanForEdges(const Vector2i& start, const Vector2i& direction) const;
  void calculateBallPercept(const Vector2i& center);

private:
  // double cam stuff
  DOUBLE_CAM_REQUIRE(SuperBallDetector, Image);
  DOUBLE_CAM_REQUIRE(SuperBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(SuperBallDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(SuperBallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(SuperBallDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(SuperBallDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(SuperBallDetector, OverTimeHistogram);
 
  DOUBLE_CAM_PROVIDE(SuperBallDetector, BallPercept);
          
};//end class SuperBallDetector

#endif // _SuperBallDetector_H_
