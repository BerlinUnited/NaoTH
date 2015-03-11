/**
* @file GoalCrossBarDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GoalCrossBarDetector
*/

#ifndef _GoalCrossBarDetector_H_
#define _GoalCrossBarDetector_H_

#include <ModuleFramework/Module.h>

//#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FieldInfo.h>
#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/GoalCrossBarPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Tools/ImageProcessing/GoalBarFeature.h"

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

BEGIN_DECLARE_MODULE(GoalCrossBarDetector)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)

  PROVIDE(GoalCrossBarPercept)
  PROVIDE(GoalCrossBarPerceptTop)
END_DECLARE_MODULE(GoalCrossBarDetector)


class GoalCrossBarDetector: private GoalCrossBarDetectorBase
{
public:

  GoalCrossBarDetector();
  virtual ~GoalCrossBarDetector();

  // override the Module execute method
  bool execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }

  Vector2d calculateGradientUV(const Vector2i& point) const;
  Vector2d calculateGradientY(const Vector2i& point) const;

private:
  static const int imageBorderOffset = 5;
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("GoalCrossBarParameters")
    {
      //PARAMETER_REGISTER(numberOfScanlines) = 9;
      PARAMETER_REGISTER(scanlinesDistance) = 20;

      PARAMETER_REGISTER(detectWhiteGoals) = true;
      PARAMETER_REGISTER(useColorFeatures) = true;
      PARAMETER_REGISTER(threshold) = 220;
      PARAMETER_REGISTER(thresholdGradient) = 40;
      PARAMETER_REGISTER(maxFeatureWidth) = 213;
      PARAMETER_REGISTER(thresholdFeatureGradient) = 0.5;
      PARAMETER_REGISTER(barWidthSimilarity) = 0.25;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    //int numberOfScanlines;
    int scanlinesDistance;

    bool detectWhiteGoals;
    //bool usePrewitt;
    bool useColorFeatures;
    int threshold;
    int thresholdGradient;
    int maxFeatureWidth;

    double thresholdFeatureGradient;
    double barWidthSimilarity;
  };

  Parameters parameters;

  std::vector<std::vector<GoalBarFeature> > features;
  std::vector<GoalBarFeature> positiveTestFeatures;
  size_t lastCrossBarScanLineId;

  void scanAlongCrossBar(const Vector2i& start, const Vector2i& end, const Vector2d& direction, double width);
  size_t scanDown(size_t id, const Vector2i& downStart, const Vector2i& downEnd);
  size_t scanDownColor(size_t id, const Vector2i& downStart, const Vector2i& downEnd);
  size_t scanDownDiff(size_t id, const Vector2i& downStart, const Vector2i& downEnd);
  bool estimateCrossBarDirection(const GoalPercept::GoalPost& post, Vector2i& start, Vector2d& direction);
  Vector2d getBackProjectedTopPoint(const GoalPercept::GoalPost& post);
  int getBackProjectedTopWidth(const GoalPercept::GoalPost& post);

  bool checkProjectedPostDistance(const GoalPercept::GoalPost& post0, const GoalPercept::GoalPost& post1);
  void checkSinglePost(const GoalPercept::GoalPost& post);
  void checkBothPosts(const GoalPercept::GoalPost& post0, const GoalPercept::GoalPost& post1);


  void testBackProjection();

  DOUBLE_CAM_PROVIDE(GoalCrossBarDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GoalCrossBarDetector, Image);
  DOUBLE_CAM_REQUIRE(GoalCrossBarDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(GoalCrossBarDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(GoalCrossBarDetector, GoalPercept);

  DOUBLE_CAM_PROVIDE(GoalCrossBarDetector, GoalCrossBarPercept);

};//end class GoalCrossBarDetector

#endif // _GoalCrossBarDetector_H_
