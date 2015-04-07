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

#include "Tools/naoth_opencv.h"
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
      PARAMETER_REGISTER(minGoodPoints) = 3;

      PARAMETER_REGISTER(detectWhiteGoals) = true;
      PARAMETER_REGISTER(useColorFeatures) = false;
      PARAMETER_REGISTER(threshold) = 140;
      PARAMETER_REGISTER(thresholdGradient) = 40;
      PARAMETER_REGISTER(maxFeatureWidth) = 213;
      PARAMETER_REGISTER(thresholdFeatureGradient) = 0.5;
      PARAMETER_REGISTER(barWidthSimilarity) = 0.25;
      PARAMETER_REGISTER(thresholdFeatureSimilarity) = 0.8;
      PARAMETER_REGISTER(maxGoalWidthRatio) = 1.5;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    //int numberOfScanlines;
    int scanlinesDistance;
    int minGoodPoints;

    bool detectWhiteGoals;
    //bool usePrewitt;
    bool useColorFeatures;
    int threshold;
    int thresholdGradient;
    int maxFeatureWidth;

    double thresholdFeatureGradient;
    double barWidthSimilarity;
    double thresholdFeatureSimilarity;
    double maxGoalWidthRatio;
  };

  Parameters parameters;

  struct Goal
  {
    Goal() : post0(0), post1(0) {}

    int post0;
    int post1;
  };

  class Cluster 
  {
  private:
    std::vector<cv::Point2f> points;
    std::vector<GoalBarFeature> features;
    double summedWidths;

  public:
    Cluster() : summedWidths(0)
    {}

    Math::Line getLine() const
    {
      ASSERT(points.size() >= 2);

      // estimate the line
      cv::Vec4f line;
      cv::fitLine(cv::Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);

      Vector2d x(line[2], line[3]);
      Vector2d v(line[0], line[1]);
  
      return Math::Line(x, v);
    }

    void add(const GoalBarFeature& postFeature) {
      features.push_back(postFeature);
      points.push_back(cv::Point2f((float)postFeature.point.x, (float)postFeature.point.y));
      summedWidths += postFeature.width;
    }

    double getFeatureWidth()
    {
      return summedWidths / static_cast<double>(features.size());
    }

    int size() const {
      return (int)points.size();
    }

    double sim(const GoalBarFeature& postFeature) const {
      double max_sim = 0;
      for(size_t i = 0; i < features.size(); i++) {
        double s = postFeature.sim(features[i]);
        if(s > max_sim) {
          max_sim = s;
        }
      }
      return max_sim;
    }
  };

  std::vector<Cluster> clusters;

  class CrossBar
  {
  public:
    CrossBar(Vector2d& dir, double len)
    :
      length(len),
      direction(dir)
    {}

    double length;
    Vector2d direction;
  };

  class GoalCandidate
  {
  public:

    GoalCandidate(const GoalPercept::GoalPost& postLeft, const GoalPercept::GoalPost& postRight, CrossBar& bar)
    :
      postCount(2),
      goalPostLeft(postLeft),
      goalPostRight(postRight),
      crossBar(bar)
    {}

    GoalCandidate(const GoalPercept::GoalPost& post, CrossBar& bar)
    :
      postCount(1),
      goalPost(post),
      crossBar(bar)
    {}

    int postCount;
    GoalPercept::GoalPost goalPostLeft;
    GoalPercept::GoalPost goalPostRight;
    GoalPercept::GoalPost goalPost;
    CrossBar crossBar;
  };

  std::vector<GoalCandidate> goalCandidates;
  std::vector<std::vector<GoalBarFeature> > features;
  size_t lastCrossBarScanLineId;

  void scanAlongCrossBar(const Vector2i& start, const Vector2i& end, const Vector2d& direction, double barWidth);
  size_t scanDown(size_t id, const Vector2i& downStart, const Vector2i& downEnd, double barWidth);
  size_t scanDownColor(size_t id, const Vector2i& downStart, const Vector2i& downEnd, double barWidth);
  size_t scanDownDiff(size_t id, const Vector2i& downStart, const Vector2i& downEnd, double barWidth);
  bool estimateCrossBarDirection(const GoalPercept::GoalPost& post, Vector2i& start, Vector2d& direction);
  Vector2d getBackProjectedTopPoint(const GoalPercept::GoalPost& post);
  int getBackProjectedTopBarWidth(const GoalPercept::GoalPost& post);
  int getBackProjectedPostHeight(const GoalPercept::GoalPost& post);

  bool checkProjectedPostDistance(const GoalPercept::GoalPost& post0, const GoalPercept::GoalPost& post1);
  void checkSinglePost(const GoalPercept::GoalPost& post);
  void checkBothPosts(const GoalPercept::GoalPost& post0, const GoalPercept::GoalPost& post1);

  void clusterEdgelFeatures();
  void calcuateCrossBars();

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
