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
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/FieldColorPercept.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/GoalFeaturePercept.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/FieldPercept.h"

// tools
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Matrix_mxn.h"

#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/RingBuffer.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

#include <vector>

#include "Tools/naoth_opencv.h"

BEGIN_DECLARE_MODULE(NewGoalDetector)
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
  REQUIRE(FrameInfo)
  
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
  virtual bool execute(CameraInfo::CameraID id, bool horizon = true);

  void execute()
  {
    bool topScanned = execute(CameraInfo::Top);

    if(!topScanned) {
      execute(CameraInfo::Top, false);
    }
    debugStuff(CameraInfo::Top);
    
    if(topScanned && getGoalPercept().getNumberOfSeenPosts() == 0) {
      if( !execute(CameraInfo::Bottom)) {
        execute(CameraInfo::Bottom, false);
      }
      debugStuff(CameraInfo::Bottom);
    }
  }

private:
  static const int imageBorderOffset = 5;
  CameraInfo::CameraID cameraID;

  RingBuffer<Vector2i, 5> pointBuffer;
  RingBufferWithSum<double, 5> valueBuffer;
  RingBufferWithSum<double, 5> valueBufferY;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("NewGoalDetectorParameters")
    {
      //PARAMETER_REGISTER(numberOfScanlines) = 5;
      //PARAMETER_REGISTER(scanlinesDistance) = 6;
      PARAMETER_REGISTER(thresholdUV) = 60;
      PARAMETER_REGISTER(thresholdY) = 140;

      PARAMETER_REGISTER(maxFeatureDeviation) = 5;
      PARAMETER_REGISTER(maxFootScanSquareError) = 4.0;
      PARAMETER_REGISTER(minGoodPoints) = 3;
      PARAMETER_REGISTER(footGreenScanSize) = 10;
      PARAMETER_REGISTER(maxFeatureWidthError) = 0.2;
      PARAMETER_REGISTER(enableFeatureWidthCheck) = false;
      PARAMETER_REGISTER(enableGreenCheck) = false;

      PARAMETER_REGISTER(colorRegionDeviation) = 2;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    virtual ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    //int numberOfScanlines;
    //int scanlinesDistance;
    int thresholdUV;
    int thresholdY;

    int maxFeatureDeviation;
    double maxFootScanSquareError;
    int minGoodPoints;

    bool enableGreenCheck;
    int footGreenScanSize; // number of pixels to scan for green below the footpoint
    
    double maxFeatureWidthError;
    bool enableFeatureWidthCheck;

    double colorRegionDeviation;
  };

  Parameters params;

  class ExtFeature
  {
  public:
    Vector2i begin;
    Vector2i center;
    Vector2i end;

    Vector2d responseAtBegin;
    Vector2d responseAtEnd;

    double width;

    bool possibleObstacle;
    bool used;

    ExtFeature(const GoalFeaturePercept::Feature& feature)
    :
      begin(feature.begin),
      center(feature.center),
      end(feature.end),
      responseAtBegin(feature.responseAtBegin),
      responseAtEnd(feature.responseAtEnd),
      width(0.0),
      possibleObstacle(feature.possibleObstacle),
      used(false)
    {

    }
  };

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

  std::vector<std::vector<ExtFeature> > features;
  std::vector<ExtFeature> goodFeatures;

  // NOTE: needed by checkForGoodFeatures (has to have the same size as features)
  std::vector<int> lastTestFeatureIdx;

  // NOTE: experimental
  void clusterEdgelFeatures();

  void checkForGoodFeatures(const Vector2d& scanDir, ExtFeature& candidate, int scanLineId, double threshold, double thresholdY);
  void scanForFootPoints(const Vector2d& scanDir, Vector2i pos, double threshold, double thresholdY);
  void scanForTopPoints(GoalPercept::GoalPost& post, Vector2i pos, double threshold, double thresholdY);
  void scanForStatisticsToFootPoint(Vector2i footPoint, Vector2i pos);
  void debugStuff(CameraInfo::CameraID camID);

  Math::Line fitLine(const std::vector<ExtFeature>& features) const;

  // double cam stuff
  DOUBLE_CAM_REQUIRE(NewGoalDetector, Image);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(NewGoalDetector, GoalFeaturePercept);

  DOUBLE_CAM_PROVIDE(NewGoalDetector, GoalPercept);

};//end class NewGoalDetector

#endif // _NewGoalDetector_H_
