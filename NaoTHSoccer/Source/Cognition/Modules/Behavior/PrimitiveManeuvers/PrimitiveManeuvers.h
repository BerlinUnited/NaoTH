/**
 * @file PrimitiveManeuvers.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class PrimitiveManeuvers
 */

#ifndef _PrimitiveManeuvers_H_
#define _PrimitiveManeuvers_H_

#include <ModuleFramework/Module.h>

#include "Tools/Math/Geometry.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

// representations
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/PathModel.h"
#include "Representations/Debug/Stopwatch.h"


BEGIN_DECLARE_MODULE(PrimitiveManeuvers)
PROVIDE(DebugRequest)
PROVIDE(DebugModify)

REQUIRE(FieldInfo)
REQUIRE(BallPercept)
REQUIRE(MultiBallPercept)
REQUIRE(MotionStatus)
REQUIRE(BallModel)

PROVIDE(PathModel)
PROVIDE(MotionRequest)
PROVIDE(HeadMotionRequest)
PROVIDE(StopwatchManager)
END_DECLARE_MODULE(PrimitiveManeuvers)

class PrimitiveManeuvers: public PrimitiveManeuversBase
{
public:
  PrimitiveManeuvers();
  ~PrimitiveManeuvers(){};

  virtual void execute();

  void execute_step_list();

private:
  // NONE means hip
  enum Foot
  {
    Right,
    Left,
    NONE
  };
  typedef WalkRequest::StepControlRequest::stepType stepType;

  // Primitive Maneuvers

  // Walks to the ball
  void MWalk_to_ball(Foot foot);
  // Moves around the ball
  void MMove_around_ball(double direction, double radius);
  // Walks to the ball "dynamically" - if neccessary 
  // keep the rotation of the robot walk back and approach it
  void MWalk_to_ball_dynamic(Foot foot);
  // Fast forward kick
  void MFast_forward_kick(Foot foot);
  // Normal kick
  void MKick_with_foot(Foot foot);
  // Sidekick (with left foot kicks to the left, and right to the right)
  void MSidekick(Foot foot);

  // Limit steps depending on rotation
  // returns Vector3d where z is the rotation
  Vector3d limit_step(Vector3d step);

  // Stepcontrol
  struct Step {
    double x;
    double y;
    double rotation;
    double character;
    stepType type;
    double speedDirection;
  };
  std::vector<Step> step_list;

  // used to alternate between left and right foot 
  // when walking inside execute_steplist()
  Foot foot_to_be_used;

  unsigned int last_stepcontrol_stepID;

  void prepare_walk();

  bool add(PrimitiveManeuvers::Step step);
  void pop_step();
  PrimitiveManeuvers::Step new_step(double x, double y, double rotation);
  PrimitiveManeuvers::Step new_step(double x, double y, double rotation, double character, stepType type, double speedDirection);
};

#endif // _PrimitiveManeuvers_H_
