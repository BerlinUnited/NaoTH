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

// local tools
#include "Tools/BestPatchList.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"

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
      PARAMETER_REGISTER(maxNumberOfKeys) = 4;
      syncWithConfig();
    }

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

    int maxNumberOfKeys;
    
  } params;


private:
  int globalNumberOfKeysClassified;
  BestPatchList best;

private:
  int calculateKeyPointsBlack(int minX, int minY, int maxX, int maxY) const;
  void calculateKeyPoints(BestPatchList& best) const;
  void extractPatches();

  void subsampling(std::vector<unsigned char>& data, int x0, int y0, int x1, int y1) const;
  void subsampling(std::vector<BallCandidates::ClassifiedPixel>& data, Moments2<2>& moments, int x0, int y0, int x1, int y1) const;

  double estimatedBallRadius(int x, int y) const;
  void addBallPercept(const Vector2i& center, double radius);

  double greenPoints(int minX, int minY, int maxX, int maxY) const;
  void executeHeuristic();

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
};//end class BallCandidateDetector

#endif // _BallCandidateDetector_H_
