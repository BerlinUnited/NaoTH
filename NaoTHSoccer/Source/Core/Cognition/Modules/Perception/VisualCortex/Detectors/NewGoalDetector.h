/**
* @file NewGoalDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class NewGoalDetector
*/

#ifndef _NewGoalDetector_H_
#define _NewGoalDetector_H_

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/GoalFeaturePercept.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"

// tools
#include "Tools/DoubleCamHelpers.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

#include <vector>

#include "Tools/naoth_opencv.h"

BEGIN_DECLARE_MODULE(NewGoalDetector)
  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  //REQUIRE(ArtificialHorizon)
  //REQUIRE(ArtificialHorizonTop)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)

  //REQUIRE(FieldColorPercept)
  //REQUIRE(FieldColorPerceptTop)
  
  REQUIRE(GoalFeaturePercept)
  REQUIRE(GoalFeaturePerceptTop)

  PROVIDE(GoalPercept)
  PROVIDE(GoalPerceptTop)
  PROVIDE(GoalPostHistograms)
END_DECLARE_MODULE(NewGoalDetector)


class NewGoalDetector: private NewGoalDetectorBase
{
public:

  NewGoalDetector();
  virtual ~NewGoalDetector(){}

  // override the Module execute method
  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);

    debugStuff(CameraInfo::Top);
    debugStuff(CameraInfo::Bottom);
  }

private:
  void execute(CameraInfo::CameraID id);
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("NewGoalDetectorParameters")
    {
      PARAMETER_REGISTER(thresholdUV) = 60;
      PARAMETER_REGISTER(minGoodPoints) = 3;
      PARAMETER_REGISTER(colorRegionDeviation) = 2;
      PARAMETER_REGISTER(thresholdFeatureSimilarity) = 0.8;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    virtual ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int thresholdUV;
    int minGoodPoints;
    double thresholdFeatureSimilarity;
    double colorRegionDeviation;
  };

  Parameters params;

  // NOTE: experimental
  class Cluster 
  {
  private:
    std::vector<cv::Point2f> points;
    std::vector<EdgelT<double> > edgels;

  public:
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

    void add(const EdgelT<double>& edgel) {
      edgels.push_back(edgel);
      points.push_back(cv::Point2f((float)edgel.point.x, (float)edgel.point.y));
    }

    int size() const {
      return (int)points.size();
    }

    double sim(const EdgelT<double>& edgel) const {
      double max_sim = 0;
      for(size_t i = 0; i < edgels.size(); i++) {
        double s = edgel.sim(edgels[i]);
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

  // double cam stuff
  DOUBLE_CAM_REQUIRE(NewGoalDetector, Image);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, CameraMatrix);
  //DOUBLE_CAM_REQUIRE(NewGoalDetector, ArtificialHorizon);
  //DOUBLE_CAM_REQUIRE(NewGoalDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, GoalFeaturePercept);

  DOUBLE_CAM_PROVIDE(NewGoalDetector, GoalPercept);

};//end class NewGoalDetector

#endif // _NewGoalDetector_H_
