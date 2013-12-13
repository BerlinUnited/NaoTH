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
#include <ModuleFramework/Representation.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/PointList.h>
#include <Tools/DataStructures/OccupancyGrid.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>

#include <Representations/Infrastructure/Image.h>

#include "Representations/Perception/BodyContour.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/FieldPercept.h"
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/FieldColorPercept.h"

// tools
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/GradientSpiderScan.h"
#include "Tools/DoubleCamHelpers.h"

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
  CameraInfo::CameraID cameraID;

  RingBuffer<Vector2<double>, 5> pointBuffer;
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
      PARAMETER_REGISTER(responseHoldFactor) = 0.8;
      PARAMETER_REGISTER(minGoodPoints) = 3;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    int gradientThreshold;
    int minY;
    int dist;
    double responseHoldFactor;
    int minGoodPoints;
    int minScanPointsAfterGoodPoints;
  };

  Parameters params;

  class Feature
  {
  public:
    Vector2<int> begin;
    Vector2<int> center;
    Vector2<int> end;

    Vector2<double> responseAtBegin;
    Vector2<double> responseAtEnd;

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

  vector<Feature> features[5];
  int lastTestFeatureIdx[5];
  vector<GoalPercept::GoalPost> goalPosts;

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, Image);
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(GradientGoalDetector, FieldColorPercept);

  DOUBLE_CAM_PROVIDE(GradientGoalDetector, GoalPercept);
          
};//end class GradientGoalDetector

#endif // _GradientGoalDetector_H_
