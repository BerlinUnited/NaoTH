/**
 * @file PathPlanner.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class PathPlanner
 */

#ifndef _PathPlanner_H_
#define _PathPlanner_H_

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


BEGIN_DECLARE_MODULE(PathPlanner)
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
END_DECLARE_MODULE(PathPlanner)

class PathPlanner: public PathPlannerBase
{
public:
  PathPlanner();
  ~PathPlanner(){};

  virtual void execute();

private:
  // NONE means hip
  enum Foot
  {
    RIGHT,
    LEFT,
    NONE
  };
  typedef WalkRequest::StepControlRequest::StepType StepType;

  // Primitive Routines
  // Walks to the ball from far away
  void walk_to_ball(const Foot foot, const bool go_fast = false);
  // Moves around the ball
  void move_around_ball(const double direction, const double radius);
  // Approaches the ball from near - if neccessary 
  // keep the rotation of the robot, walk back and then approach it
  void approach_ball(const Foot foot);
  // Short kick forward
  void short_kick(const Foot foot);
  // Long kick forward
  void long_kick(const Foot foot);
  // Sidekick (with left foot == Foot::LEFT kicks to the left, and foot == FOOT::RIGHT to the right)
  void sidekick(const Foot foot);

  // Stepcontrol
  struct Step_Buffer_Element {
    Pose2D pose;
    double speedDirection;
    StepType type;
    int time;
    double character;
    double scale;
    Foot foot;
    WalkRequest::Coordinate coordinate;
  };
  std::vector<Step_Buffer_Element> step_buffer;

  // Used to alternate between left and right foot
  // or to specify which foot to use
  Foot foot_to_use;

  // Used to synchronize stepIDs of WalkEngine to take control
  unsigned int last_stepcontrol_stepID;

  void add_step(Pose2D &pose, const StepType &type, const WalkRequest::Coordinate &coordinate, const double character = 0.5, const Foot foot = Foot::NONE, const double scale = 1.0, const double speedDirection = 0.0);
  bool add_single_step(Pose2D &pose, const StepType &type, const WalkRequest::Coordinate &coordinate);void update_step(Pose2D &pose);
  void manage_step_buffer();
  void execute_step_buffer();
};

#endif // _PathPlanner_H_
