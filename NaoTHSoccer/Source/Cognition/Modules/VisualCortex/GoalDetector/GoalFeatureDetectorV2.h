/**
* @file GoalFeatureDetectorV2.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GoalFeatureDetectorV2
*/

#ifndef _GoalFeatureDetectorV2_H_
#define _GoalFeatureDetectorV2_H_

#include <ModuleFramework/Module.h>

//#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/GoalFeaturePercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/RingBuffer.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

#include "Tools/naoth_opencv.h"
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/ImageProcessing/Filter.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(GoalFeatureDetectorV2)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  PROVIDE(GoalFeaturePercept)
  PROVIDE(GoalFeaturePerceptTop)
END_DECLARE_MODULE(GoalFeatureDetectorV2)


class GoalFeatureDetectorV2: private GoalFeatureDetectorV2Base
{
public:

  GoalFeatureDetectorV2();
  virtual ~GoalFeatureDetectorV2();

  // override the Module execute method
  bool execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    execute(CameraInfo::Top);

    // HACK: RC15 don't check for goals in the bottom camera
    //execute(CameraInfo::Bottom);
  }

private:
  static const int imageBorderOffset = 5;
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("GoalFeatureV2Parameters")
    {
      PARAMETER_REGISTER(numberOfScanlines) = 9;
      PARAMETER_REGISTER(scanlinesDistance) = 8;

      PARAMETER_REGISTER(detectWhiteGoals) = true;
      PARAMETER_REGISTER(threshold) = 140;
      PARAMETER_REGISTER(thresholdGradient) = 7;
      PARAMETER_REGISTER(thresholdFeatureGradient) = 0.5;
      PARAMETER_REGISTER(maxFeatureWidth) = 213;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    int numberOfScanlines;
    int scanlinesDistance;

    bool detectWhiteGoals;
    bool usePrewitt;
    bool useColorFeatures;
    bool experimental;
    int threshold;
    int thresholdGradient;
    int maxFeatureWidth;

    double thresholdFeatureGradient;
  };

  Parameters parameters;

  void findEdgelFeatures(const Vector2d& scanDir, const Vector2i& p1);
  Vector2d calculateGradientUV(const Vector2i& point) const;
  Vector2d calculateGradientY(const Vector2i& point) const;

  DOUBLE_CAM_PROVIDE(GoalFeatureDetectorV2, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GoalFeatureDetectorV2, Image);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetectorV2, ArtificialHorizon);

  DOUBLE_CAM_PROVIDE(GoalFeatureDetectorV2, GoalFeaturePercept);

};//end class GoalFeatureDetectorV2

#endif // _GoalFeatureDetectorV2_H_
