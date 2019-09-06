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
      PARAMETER_REGISTER(forwardKickThreshold.x)                = 50; // mm
      PARAMETER_REGISTER(forwardKickThreshold.y)                = 25; // mm
      PARAMETER_REGISTER(forwardKickAdaptive)                   = false;
      //PARAMETER_REGISTER(nearApproachForwardKickBallPosOffsetX) = 110;
      PARAMETER_REGISTER(nearApproachSideKickBallPosOffsetX)    = 100;
      PARAMETER_REGISTER(nearApproach_step_character)           = 0.3;
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
    //double readyForForwardKickThresholdX;
    //double readyForForwardKickThresholdY;
    Vector2d forwardKickThreshold;
    bool forwardKickAdaptive;

    //double nearApproachForwardKickBallPosOffsetX;
    double nearApproachSideKickBallPosOffsetX;
    double farToNearApproachThreshold;
    double rotationLength;
    double sidekickOffsetY;
    int forwardKickTime;
    int sideKickTime;
    double moveAroundBallCharacter;
    double moveAroundBallCharacterStable;
    double nearApproach_step_character;
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
  bool nearApproach_forwardKick(const double offsetX, const double offsetY);
  bool nearApproach_sideKick(const Foot& foot, const double offsetX, const double offsetY);
  bool sidesteps(const Foot& foot, const double direction);

  void forwardKick();
  void sideKick(const Foot& foot);

// generate a setter method
#define SET(Type, SetName, Name) \
  public: StepBufferElement& set##SetName(const Type& v) { Name = v; return *this; } \
  public: Type Name

  struct StepBufferElement
  {
    StepBufferElement() {}
    StepBufferElement(const std::string& name) : debug_name(name) {}

    SET(Pose2D,Pose, pose);
    SET(double,SpeedDirection, speedDirection);
    SET(StepType,StepType,type);
    SET(int,Time,time);
    SET(double,Character,character);
    SET(double,Scale,scale);
    SET(Foot,Foot,foot);
    SET(WalkRequest::Coordinate,Coordinate,coordinate);
    SET(RestrictionMode,Restriction,restriction);
    SET(bool,Protected,isProtected);

  public:
    std::string debug_name;
  };

  std::vector<StepBufferElement> stepBuffer;

  // Used to alternate between left and right foot
  // or to specify which foot to use
  Foot footToUse;

  // Used to synchronize stepIDs of WalkEngine to take control
  unsigned int lastStepRequestID;

  void addStep(const StepBufferElement& new_step) {
    stepBuffer.push_back(new_step);
    //std::cout << new_step.debug_name << std::endl;
  }

  /*
  NOTE: this feature is for future generations of advanced programmers who like to program like javascript peasants
  const StepBufferElement& addStep(const std::string& name) {
    stepBuffer.emplace_back(name);
    return stepBuffer.back();
  }
  */

  void updateSpecificStep(const unsigned int index, StepBufferElement& step);
  void manageStepBuffer();
  void executeStepBuffer();


private:
  bool kickPlanned;

};

#endif // _PathPlanner2018_H_
