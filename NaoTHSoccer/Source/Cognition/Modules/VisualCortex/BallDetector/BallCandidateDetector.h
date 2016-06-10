/**
* @file BallCandidateDetector.h
*
* Definition of class BallCandidateDetector
*/

#ifndef _BallCandidateDetector_H_
#define _BallCandidateDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>

#include <Representations/Infrastructure/Image.h>
#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BodyContour.h"

#include "Representations/Perception/GameColorIntegralImage.h"
#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Perception/BallCandidates.h"
#include "Representations/Debug/Stopwatch.h"
#include "Representations/Modeling/RobotPose.h"

// tools
#include "Tools/DoubleCamHelpers.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"


#include "Tools/naoth_opencv.h"


BEGIN_DECLARE_MODULE(BallCandidateDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugPlot)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugModify)
  PROVIDE(StopwatchManager)

  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  PROVIDE(GameColorIntegralImage)
  PROVIDE(GameColorIntegralImageTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)

  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

  //hack
  REQUIRE(RobotPose)
  
  PROVIDE(MultiBallPercept)
  PROVIDE(BallCandidates)
  PROVIDE(BallCandidatesTop)
END_DECLARE_MODULE(BallCandidateDetector)


class BallCandidateDetector: private BallCandidateDetectorBase
{
public:
  BallCandidateDetector();
  ~BallCandidateDetector();

  bool execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    getMultiBallPercept().reset();
    globalNumberOfKeysClassified = 0;

    // only execute search on top camera if bottom camera did not find anything
    if(!execute(CameraInfo::Bottom))
    {
      execute(CameraInfo::Top);
    }
    else
    {
      DEBUG_REQUEST("Vision:BallCandidateDetector:forceBothCameras",
        execute(CameraInfo::Top);
      );
    }
  }
 
private:
  CameraInfo::CameraID cameraID;

  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("BallCandidateDetector")
    {
      
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorClose) = 0.5;
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorFar) = 0.8;

      PARAMETER_REGISTER(classifier.basic_svm) = false;
      PARAMETER_REGISTER(classifier.cv_svm_histogram) = true;
      PARAMETER_REGISTER(classifier.heuristic) = false;
      PARAMETER_REGISTER(classifier.maxNumberOfKeys) = 4;
      PARAMETER_REGISTER(classifier.maxMomentAxesRatio) = 2.0;
      
      PARAMETER_REGISTER(heuristic.maxGreenInsideRatio) = 0.3;
      PARAMETER_REGISTER(heuristic.minGreenBelowRatio) = 0.5;
      PARAMETER_REGISTER(heuristic.blackDotsWhiteOffset) = 110;
      PARAMETER_REGISTER(heuristic.blackDotsMinCount) = 8;
      PARAMETER_REGISTER(heuristic.onlyOnField) = false;
      PARAMETER_REGISTER(heuristic.maxMomentAxesRatio) = 2.0;
      PARAMETER_REGISTER(heuristic.blackDotsMinRatio) = 0.1;
      PARAMETER_REGISTER(heuristic.minBlackDetectionSize) = 20;
      PARAMETER_REGISTER(heuristic.minBallSizeForSVM) = 20;

      PARAMETER_REGISTER(thresholdGradientUV) = 40;
      PARAMETER_REGISTER(minNumberOfJumps) = 4;
      syncWithConfig();
    }

	  struct Classifier {
      bool basic_svm;
      bool cv_svm_histogram;
      bool heuristic;
      int maxNumberOfKeys;
      double maxMomentAxesRatio;
    } classifier;

    struct KeyDetector {
      double borderRadiusFactorClose;
      double borderRadiusFactorFar;
    } keyDetector;

    struct Heuristics {
      double maxGreenInsideRatio;
      double minGreenBelowRatio;
      int blackDotsWhiteOffset;
      int blackDotsMinCount;
      double blackDotsMinRatio;
      bool onlyOnField;
      double maxMomentAxesRatio;

      int minBlackDetectionSize;
      int minBallSizeForSVM;
    } heuristic;

    int thresholdGradientUV;
    int minNumberOfJumps;
    
  } params;


private:
  int globalNumberOfKeysClassified;

  class Best 
  {
  public:

    class BallCandidate 
    {
    public:
      BallCandidate() : radius(-1), value(-1) {}
      BallCandidate(const Vector2i& center, double radius, double value) 
        : center(center), radius(radius), value(value)
      {}
      Vector2i center;
      double radius;
      double value;
    };

    std::list<BallCandidate> candidates;

    template <typename Iter>
    Iter nextIter(Iter iter) {
        return ++iter;
    }

    void add(const Vector2i& center, double radius, double value)
    {
      if(candidates.empty()) {
        candidates.push_back(BallCandidate(center, radius, value));
        return;
      }

      bool stop = false;
      for (std::list<BallCandidate>::iterator i = candidates.begin(); i != candidates.end(); /*nothing*/)
      {
        if (std::max(std::abs((*i).center.x - center.x), std::abs((*i).center.y - center.y)) < ((*i).radius + radius)) {
          if(value > (*i).value) {
            i = candidates.erase(i);
          } else {
            stop = true;
            ++i;
          }
        } else {
          ++i;
        }
      }
      if(stop) {
        return;
      }

      for(std::list<BallCandidate>::iterator i = candidates.begin(); i != candidates.end(); ++i)
      {
        // insert
        if(value < (*i).value) {
          i = candidates.insert(i,BallCandidate(center, radius, value));
          break;
        } else if(nextIter(i) == candidates.end()) {
          candidates.push_back(BallCandidate(center, radius, value));
          break;
        }
      }

      /*
      if(candidates.size() > 30) {
        candidates.pop_front();
      }
      */
    }

    void clear() {
      candidates.clear();
    }

  } best;

private:
  int calculateKeyPointsBlack(int minX, int minY, int maxX, int maxY) const;
  void calculateKeyPoints(Best& best) const;
  void extractPatches();

  void subsampling(std::vector<unsigned char>& data, int x0, int y0, int x1, int y1) const;
  void subsampling(std::vector<BallCandidates::ClassifiedPixel>& data, Moments2<2>& moments, int x0, int y0, int x1, int y1) const;

  double estimatedBallRadius(int x, int y) const;
  void addBallPercept(const Vector2i& center, double radius);

  double greenPoints(int minX, int minY, int maxX, int maxY) const;
  void executeHeuristic();

private:
  double blackPointsCount(BallCandidates::PatchYUVClassified& p, double blackWhiteOffset) const;
  Vector2d spiderScan(const Vector2i& start, std::vector<Vector2i>& endPoints, int max_length) const;
  Vector2d scanForEdges(const Vector2i& start, const Vector2d& direction, std::vector<Vector2i>& points, int max_length) const;

private:
  
  DOUBLE_CAM_PROVIDE(BallCandidateDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, Image);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, FieldPercept);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, GameColorIntegralImage);

  DOUBLE_CAM_PROVIDE(BallCandidateDetector, BallCandidates);

  cv::Ptr<cv::ml::SVM> histModelBottom;
  cv::Ptr<cv::ml::SVM> histModelTop;
};//end class BallCandidateDetector

#endif // _BallCandidateDetector_H_
