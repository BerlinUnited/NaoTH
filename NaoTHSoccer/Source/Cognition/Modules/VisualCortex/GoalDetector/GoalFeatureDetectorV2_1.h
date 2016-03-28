/**
* @file GoalFeatureDetectorV2_1.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GoalFeatureDetectorV2_1
*/

#ifndef _GoalFeatureDetectorV2_1_H_
#define _GoalFeatureDetectorV2_1_H_

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

BEGIN_DECLARE_MODULE(GoalFeatureDetectorV2_1)
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
END_DECLARE_MODULE(GoalFeatureDetectorV2_1)


class GoalFeatureDetectorV2_1: private GoalFeatureDetectorV2_1Base
{
public:

  GoalFeatureDetectorV2_1();
  virtual ~GoalFeatureDetectorV2_1();

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
    Parameters() : ParameterList("GoalFeatureV2_1Parameters")
    {
      //amount of scanlines above plus amount of scanlines below horizon
      PARAMETER_REGISTER(numberOfScanlines) = 9;
      //distance between scanlines in pixels
      PARAMETER_REGISTER(scanlinesDistance) = 8;

      // minimum brighntess (or UV value) at which jumps shall be detected
      // - every value below this threshold will be set to half of the threshold 
      //   before used in the jumps detecting filter
      PARAMETER_REGISTER(threshold) = 140;
      // minimum strength of a jump to be taken as possible edge point
      PARAMETER_REGISTER(thresholdGradient) = 24;
      // if the switch useWeakJumpsToo is true than this is ne minimum strenght of a weak jump
      // - should be near to half of the strength of regular jumps
      PARAMETER_REGISTER(thresholdWeakGradient) = 10;
      // minimum similarity of the angles from the edges at begin and end of a feature candidate
      // - (range is [0,1]: 0 - not simmilar, 1 - very simmilar)
      PARAMETER_REGISTER(thresholdFeatureGradient) = 0.5;
      // maximum length a feature candidate can have
      PARAMETER_REGISTER(maxFeatureWidth) = 213;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    int numberOfScanlines;
    int scanlinesDistance;

    int threshold;
    int thresholdGradient;
    int thresholdWeakGradient;
    int maxFeatureWidth;

    double thresholdFeatureGradient;
  };

  Parameters parameters;

  class Switches: public ParameterList
  {
  public:
    Switches() : ParameterList("GoalFeatureV2_1Switches")
    {
      // switch to enable use of artificial jumps
      // - artificial jumps are created at the left or right border of the image,
      //   if the pixel vlue is above the threshold
      PARAMETER_REGISTER(useArtificialPoints) = true;
      // switch to enable the use of weak jumps
      PARAMETER_REGISTER(useWeakJumpsToo) = true;
      // switches between detecting colored or white goals
      PARAMETER_REGISTER(detectWhiteGoals) = true;

      syncWithConfig();
    }

    virtual ~Switches() {
    }

    bool useArtificialPoints;
    bool useWeakJumpsToo;
    bool detectWhiteGoals;
  };

  Switches switchParams;

  void findEdgelFeatures(const Vector2d& scanDir, const Vector2i& p1);
  Vector2d calculateGradientUV(const Vector2i& point) const;
  Vector2d calculateGradientY(const Vector2i& point) const;

  DOUBLE_CAM_PROVIDE(GoalFeatureDetectorV2_1, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GoalFeatureDetectorV2_1, Image);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetectorV2_1, ArtificialHorizon);

  DOUBLE_CAM_PROVIDE(GoalFeatureDetectorV2_1, GoalFeaturePercept);

};//end class GoalFeatureDetectorV2_1

#endif // _GoalFeatureDetectorV2_1_H_
