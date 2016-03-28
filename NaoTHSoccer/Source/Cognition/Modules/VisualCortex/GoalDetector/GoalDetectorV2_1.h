/**
* @file GoalDetectorV2_1.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GoalDetectorV2_1
*/

#ifndef _GoalDetectorV2_1_H_
#define _GoalDetectorV2_1_H_

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FieldInfo.h>
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/GoalFeaturePercept.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"

// tools
#include "Tools/DoubleCamHelpers.h"

#include <vector>

#include "Tools/naoth_opencv.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(GoalDetectorV2_1)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(FieldInfo)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)

  REQUIRE(GoalFeaturePercept)
  REQUIRE(GoalFeaturePerceptTop)

  PROVIDE(GoalPercept)
  PROVIDE(GoalPerceptTop)
  PROVIDE(GoalPostHistograms)
END_DECLARE_MODULE(GoalDetectorV2_1)


class GoalDetectorV2_1: private GoalDetectorV2_1Base
{
public:

  GoalDetectorV2_1();
  virtual ~GoalDetectorV2_1();

  // override the Module execute method
  virtual void execute()
  {
    execute(CameraInfo::Top);
    // HACK: RC15 don't check for goals in the bottom camera
    //execute(CameraInfo::Bottom);

    debugStuff(CameraInfo::Top);
    // HACK: RC15 don't check for goals in the bottom camera
    //debugStuff(CameraInfo::Bottom);
  }

private:
  void execute(CameraInfo::CameraID id);
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("GoalDetectorV2_1Parameters")
    {
      // minimum brighntess (or UV value) at which jumps shall be detected
      // - every value below this threshold will be set to half of the threshold 
      //   before used in the jumps detecting filter
      PARAMETER_REGISTER(threshold) = 140;
      // minimum strength of a jump to be taken as possible edge point
      PARAMETER_REGISTER(thresholdGradient) = 7;
      // minimum amount of matching features needed to build a post canditate
      PARAMETER_REGISTER(minGoodPoints) = 3;
      // switches between detecting colored or white goals
      PARAMETER_REGISTER(detectWhiteGoals) = true;

      // only for colored goal detection: factor for the deviation of the collected color histogram for the post color
      PARAMETER_REGISTER(colorRegionDeviation) = 2;
      // similarity of the normals between features
      PARAMETER_REGISTER(thresholdFeatureSimilarity) = 0.8;
      // maximium ration between detected and backprojected width of the goals bars
      PARAMETER_REGISTER(maxBarWidthRatio) = 1.5;
      // a post must have at least this ratio between detected and backprojected goal post height
      // (detected height can be bigger but not lower)
      PARAMETER_REGISTER(minGoalHeightRatio) = 0.8;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    int threshold;
    int thresholdGradient;
    int minGoodPoints;

    bool detectWhiteGoals;

    double thresholdFeatureSimilarity;
    double colorRegionDeviation;
    double maxBarWidthRatio;
    double minGoalHeightRatio;
  };

  Parameters params;

  // NOTE: experimental
  class Cluster 
  {
  private:
    std::vector<cv::Point2f> points;
    std::vector<GoalBarFeature > features;
    double summedWidths;

  public:
    Cluster() : summedWidths(0.0)
    {}

    Math::Line getLine() const
    {
      ASSERT(points.size() >= 2);

      // estimate the line
      cv::Vec4f line;
      cv::fitLine(cv::Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);

      Vector2d x(line[2], line[3]);
      Vector2d v(line[0], line[1]);
  
      // always point down in the image
      if(v.y < 0) {
        v *= -1.0;
      }

      return Math::Line(x, v);
    }

    void add(const GoalBarFeature& postFeature)
    {
      features.push_back(postFeature);
      points.push_back(cv::Point2f((float)postFeature.point.x, (float)postFeature.point.y));
      summedWidths += postFeature.width;
    }

    const std::vector<GoalBarFeature >& getFeatures()
    {
      return features;
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
  void clusterEdgelFeatures();
  void calcuateGoalPosts();
  Vector2i scanForEndPoint(const Vector2i& start, const Vector2d& direction) const;

  void debugStuff(CameraInfo::CameraID camID);

  Vector2d getBackProjectedTopPoint(const GoalPercept::GoalPost& post);
  Vector2d getBackProjectedBasePoint(const GoalPercept::GoalPost& post);
  int getBackProjectedTopBarWidth(const GoalPercept::GoalPost& post);
  int getBackProjectedBaseBarWidth(const GoalPercept::GoalPost& post);
  int getBackProjectedPostHeight(const GoalPercept::GoalPost& post);


  DOUBLE_CAM_PROVIDE(GoalDetectorV2_1, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GoalDetectorV2_1, Image);
  DOUBLE_CAM_REQUIRE(GoalDetectorV2_1, CameraMatrix);
  DOUBLE_CAM_REQUIRE(GoalDetectorV2_1, FieldPercept);
  DOUBLE_CAM_REQUIRE(GoalDetectorV2_1, GoalFeaturePercept);

  DOUBLE_CAM_PROVIDE(GoalDetectorV2_1, GoalPercept);

};//end class GoalDetectorV2_1

#endif // _GoalDetectorV2_1_H_
