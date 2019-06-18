/**
* @file PathPlanner2018.h
*
* @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
* Definition of class PathPlanner2018
*/

#ifndef _PathPlanner2018_H_
#define _PathPlanner2018_H_

#include <ModuleFramework/Module.h>

#include "Tools/Math/Geometry.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"

// representations
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/PathModel.h"
#include "Representations/Debug/Stopwatch.h"


BEGIN_DECLARE_MODULE(PathPlanner2018)
PROVIDE(DebugRequest)
PROVIDE(DebugModify)
PROVIDE(DebugParameterList)

REQUIRE(FieldInfo)
REQUIRE(MultiBallPercept)
REQUIRE(MotionStatus)
REQUIRE(BallModel)

PROVIDE(PathModel)
PROVIDE(MotionRequest)
PROVIDE(HeadMotionRequest)
PROVIDE(StopwatchManager)
END_DECLARE_MODULE(PathPlanner2018)



class PathPlanner2018 : public PathPlanner2018Base
{
public:
  PathPlanner2018();
  ~PathPlanner2018();

  virtual void execute();

private:
  class Parameters : public ParameterList
  {
  public:
    Parameters() : ParameterList("PathPlanner2018")
    {
      PARAMETER_REGISTER(stepLength)                            = 80.0;
      PARAMETER_REGISTER(readyForSideKickThresholdX)            = 4.0;
      PARAMETER_REGISTER(readyForSideKickThresholdY)            = 0.3;
      PARAMETER_REGISTER(readyForForwardKickThresholdX)         = 4.0;
      PARAMETER_REGISTER(readyForForwardKickThresholdY)         = 0.3;
      PARAMETER_REGISTER(nearApproachForwardKickBallPosOffsetX) = 110;
      PARAMETER_REGISTER(nearApproachSideKickBallPosOffsetX)    = 100;
      PARAMETER_REGISTER(farToNearApproachThreshold)            = 10.0;
      PARAMETER_REGISTER(rotationLength)                        = 30.0;
      PARAMETER_REGISTER(sidekickOffsetY)                       = 40.0;
      PARAMETER_REGISTER(forwardKickTime)                       = 300;
      PARAMETER_REGISTER(sideKickTime)                          = 300;
      PARAMETER_REGISTER(moveAroundBallCharacter)               = 1.0;
      PARAMETER_REGISTER(moveAroundBallCharacterStable)         = 0.3;

      syncWithConfig();
    }
    virtual ~Parameters(){}

    double stepLength;
    double readyForSideKickThresholdX;
    double readyForSideKickThresholdY;
    double readyForForwardKickThresholdX;
    double readyForForwardKickThresholdY;
    double nearApproachForwardKickBallPosOffsetX;
    double nearApproachSideKickBallPosOffsetX;
    double farToNearApproachThreshold;
    double rotationLength;
    double sidekickOffsetY;
    int forwardKickTime;
    int sideKickTime;
    double moveAroundBallCharacter;
    double moveAroundBallCharacterStable;
  } params;

  // NONE means hip
  enum Foot
  {
    RIGHT,
    LEFT,
    NONE
  };

  typedef WalkRequest::StepControlRequest::StepType StepType;
  typedef WalkRequest::StepControlRequest::RestrictionMode RestrictionMode;
  typedef WalkRequest::Coordinate Coordinate;

  void moveAroundBall(const double direction, const double radius, const bool stable);

  // goToBall is split up between sideKick and forwardKick so that changing things in upcoming RoboCup 2018
  // won't be so complex as to introduce bugs easily
  bool farApproach();
  bool nearApproach_forwardKick(const Foot& foot, const double offsetX, const double offsetY);
  bool nearApproach_sideKick(const Foot& foot, const double offsetX, const double offsetY);
  bool sidesteps(const Foot& foot);

  void forwardKick(const Foot& foot);
  void sideKick(const Foot& foot);

  struct StepBufferElement 
  {
    void setPose(const Pose2D& p)
    {
      pose = p;
    }
    void setSpeedDirection(const double s)
    {
      speedDirection = s;
    }
    void setStepType(const StepType& t)
    {
      type = t;
    }
    void setTime(const int t)
    {
      time = t;
    }
    void setCharacter(const double c)
    {
      character = c;
    }
    void setScale(const double s)
    {
      scale = s;
    }
    void setFoot(const Foot& f)
    {
      foot = f;
    }
    void setCoordinate(const WalkRequest::Coordinate& c)
    {
      coordinate = c;
    }
    void setRestriction(const RestrictionMode& r)
    {
      restriction = r;
    }
    void setProtected(const bool p)
    {
      isProtected = p;
    }
    Pose2D pose;
    double speedDirection;
    StepType type;
    int time;
    double character;
    double scale;
    Foot foot;
    WalkRequest::Coordinate coordinate;
    RestrictionMode restriction;
    bool isProtected;
  };
  std::vector<StepBufferElement> stepBuffer;

  // Used to alternate between left and right foot
  // or to specify which foot to use
  Foot footToUse;

  // Used to synchronize stepIDs of WalkEngine to take control
  unsigned int lastStepRequestID;

  void addStep(const StepBufferElement& newStep);
  void updateSpecificStep(const unsigned int index, StepBufferElement& step);
  void manageStepBuffer();
  void executeStepBuffer();


private:
  bool kickPlanned;
  double numPossibleSteps;
  double numPossibleStepsX;
  double numPossibleStepsY;
  double numRotationStepsNecessary;
};

#endif // _PathPlanner2018_H_
