/**
* @file BallCandidateDetector.h
*
* Definition of class BallCandidateDetector
*/

#ifndef _BallCandidateDetector_H_
#define _BallCandidateDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>

#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BodyContour.h"

#include "Representations/Perception/GameColorIntegralImage.h"
#include "Representations/Perception/BallCandidates.h"
#include "Representations/Perception/MultiBallPercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"

// local tools
#include "Tools/BestPatchList.h"
#include "Tools/BallKeyPointExtractor.h"
#include "Tools/CVHaarClassifier.h"

// debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"

BEGIN_DECLARE_MODULE(BallCandidateDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugModify)
  PROVIDE(StopwatchManager)

  REQUIRE(FrameInfo)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  PROVIDE(GameColorIntegralImage)
  PROVIDE(GameColorIntegralImageTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  //REQUIRE(BodyContour)
  //REQUIRE(BodyContourTop)

  PROVIDE(BallCandidates)
  PROVIDE(BallCandidatesTop)
  PROVIDE(MultiBallPercept)
END_DECLARE_MODULE(BallCandidateDetector)


class BallCandidateDetector: private BallCandidateDetectorBase, private ModuleManager
{
public:
  BallCandidateDetector();
  ~BallCandidateDetector();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    getMultiBallPercept().reset();
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }
 
private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("BallCandidateDetector")
    {
      
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorClose) = 0.5;
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorFar) = 0.8;

      PARAMETER_REGISTER(heuristic.maxGreenInsideRatio) = 0.3;
      PARAMETER_REGISTER(heuristic.minGreenBelowRatio) = 0.5;
      PARAMETER_REGISTER(heuristic.blackDotsMinCount) = 1;
      PARAMETER_REGISTER(heuristic.minBlackDetectionSize) = 20;

      PARAMETER_REGISTER(haarDetector.minNeighbors) = 0;

      PARAMETER_REGISTER(maxNumberOfKeys) = 4;
      syncWithConfig();
    }

    BallKeyPointExtractor::Parameter keyDetector;

    struct Heuristics {
      double maxGreenInsideRatio;
      double minGreenBelowRatio;
      double blackDotsMinRatio;

      int blackDotsMinCount;
      int minBlackDetectionSize;
    } heuristic;

    struct HaarDetector {
      int minNeighbors;
    } haarDetector;

    int maxNumberOfKeys;
    
  } params;


private:
  CVHaarClassifier cvClassifier;
  ModuleCreator<BallKeyPointExtractor>* theBallKeyPointExtractor;
  BestPatchList best;

private:
  void calculateCandidates();
  void addBallPercept(const Vector2i& center, double radius);

private:
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_PROVIDE(BallCandidateDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, Image);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, FieldPercept);
  //DOUBLE_CAM_REQUIRE(BallCandidateDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(BallCandidateDetector, GameColorIntegralImage);

  DOUBLE_CAM_PROVIDE(BallCandidateDetector, BallCandidates);
};//end class BallCandidateDetector

#endif // _BallCandidateDetector_H_
