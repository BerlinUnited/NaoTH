/**
* @file GoalFeatureDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GoalFeatureDetector
*/

#ifndef _GoalFeatureDetector_H_
#define _GoalFeatureDetector_H_

#include <ModuleFramework/Module.h>

//#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/GoalFeaturePercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/RingBuffer.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

//#include "Tools/naoth_opencv.h"
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/ImageProcessing/Filter.h"
#include "Tools/ImageProcessing/MaximumScan.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(GoalFeatureDetector)
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
END_DECLARE_MODULE(GoalFeatureDetector)


class GoalFeatureDetector: private GoalFeatureDetectorBase
{
public:

  GoalFeatureDetector();
  virtual ~GoalFeatureDetector();

  // override the Module execute method
  bool execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }

private:
  static const int imageBorderOffset = 5;
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("GoalFeatureParameters")
    {
      PARAMETER_REGISTER(numberOfScanlines) = 9;
      PARAMETER_REGISTER(scanlinesDistance) = 8;

      PARAMETER_REGISTER(detectWhiteGoals) = true;
      PARAMETER_REGISTER(useColorFeatures) = true;
      PARAMETER_REGISTER(threshold) = 60;
      PARAMETER_REGISTER(thresholdGradient) = 40;
      PARAMETER_REGISTER(thresholdFeatureGradient) = 0.5;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    int numberOfScanlines;
    int scanlinesDistance;

    bool detectWhiteGoals;
    bool usePrewitt;
    bool useColorFeatures;
    int threshold;
    int thresholdGradient;

    double thresholdFeatureGradient;
  };


private:
  Parameters parameters;

  void findfeaturesColor(const Vector2d& scanDir, const Vector2i& p1);
  void findfeaturesDiff(const Vector2d& scanDir, const Vector2i& p1);
  Vector2d calculateGradientUV(const Vector2i& point) const;
  Vector2d calculateGradientY(const Vector2i& point) const;

private:
  DOUBLE_CAM_PROVIDE(GoalFeatureDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, Image);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, ArtificialHorizon);

  DOUBLE_CAM_PROVIDE(GoalFeatureDetector, GoalFeaturePercept);

};//end class GoalFeatureDetector

#endif // _GoalFeatureDetector_H_
