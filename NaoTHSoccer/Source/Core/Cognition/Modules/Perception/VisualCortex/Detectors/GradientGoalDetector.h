/**
* @file GradientGoalDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GradientGoalDetector
*/

#ifndef _GradientGoalDetector_H_
#define _GradientGoalDetector_H_

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/FieldColorPercept.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/GoalPercept.h"

// tools
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Matrix_mxn.h"

#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/RingBuffer.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

#include <opencv2/core/core.hpp>
#include <vector>

BEGIN_DECLARE_MODULE(GradientGoalDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  //REQUIRE(FieldPercept)
  //REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(FrameInfo)

  PROVIDE(GoalPercept)
  PROVIDE(GoalPerceptTop)
END_DECLARE_MODULE(GradientGoalDetector)


class GradientGoalDetector: private GradientGoalDetectorBase
{
public:

  GradientGoalDetector();
  ~GradientGoalDetector(){};

  // override the Module execute method
  virtual void execute(CameraInfo::CameraID id, bool horizon = true);

  void execute()
  {
    execute(CameraInfo::Top);
    if( getGoalPercept().getNumberOfSeenPosts() == 0) {
      execute(CameraInfo::Top, false);
    }
    execute(CameraInfo::Bottom);
  }
 
private:
  static const int imageBorderOffset = 25;
  CameraInfo::CameraID cameraID;
  
  RingBuffer<Vector2d, 5> pointBuffer;
  RingBufferWithSum<double, 5> valueBuffer;
  RingBufferWithSum<double, 5> valueBufferY;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("GradientGoalDetectorParameters")
    {
      PARAMETER_REGISTER(gradientThreshold) = 60;
      PARAMETER_REGISTER(minY) = 140;
      PARAMETER_REGISTER(dist) = 5;
      PARAMETER_REGISTER(maxSquareError) = 4.0;
      PARAMETER_REGISTER(responseHoldFactor) = 0.8;
      PARAMETER_REGISTER(minGoodPoints) = 3;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int gradientThreshold;
    int minY;
    int dist;
    double maxSquareError;
    double responseHoldFactor;
    int minGoodPoints;
    int minScanPointsAfterGoodPoints;
  };

  Parameters params;

  class Feature
  {
  public:
    Vector2i begin;
    Vector2i center;
    Vector2i end;

    Vector2d responseAtBegin;
    Vector2d responseAtEnd;

    bool possibleObstacle;

    Feature()
    :
      begin(-1,-1),
      end(-1, -1),
      responseAtBegin(0.0, 0.0),
      responseAtEnd(0.0, 0.0),
      possibleObstacle(false)
    {

    }

  };

  class TestFeature
  {
  public:
    Vector2d diff;
    Vector2i start;
    size_t idxS;
    size_t idxE;
  };

  static const int numberOfScanlines = 5;
  std::vector<std::vector<Feature> > features;
  std::vector<int> lastTestFeatureIdx;

  //std::vector<GoalPercept::GoalPost> goalPosts;

  //std::vector<TestFeature> testFeatures;

  void findFeatureCandidates(const Vector2d& scanDir, const Vector2d& p1, double threshold, double thresholdY);
  std::vector<Feature> checkForGoodFeatures(const Vector2d& scanDir, const Feature& candidate, double threshold, double thresholdY);
  Vector2d findBestDownScanDirection(const std::vector<Feature>& features);
  cv::Vec4f fitLine(const std::vector<Feature>& features);
  void scanForFootPoints(const Vector2d& scanDir, Vector2i pos, double threshold, double thresholdY, bool horizon);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, Image);
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, FieldColorPercept);

  DOUBLE_CAM_PROVIDE(GradientGoalDetector, GoalPercept);
          
};//end class GradientGoalDetector

#endif // _GradientGoalDetector_H_
