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
#include "Tools/Debug/DebugParameterList.h"

// representations
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/PathModel.h"
#include "Representations/Debug/Stopwatch.h"


BEGIN_DECLARE_MODULE(PathPlanner)
PROVIDE(DebugRequest)
PROVIDE(DebugModify)
PROVIDE(DebugParameterList)

REQUIRE(FieldInfo)
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
  ~PathPlanner();

  virtual void execute();

private:
  class Parameters : public ParameterList
  {
  public:
    Parameters() : ParameterList("PathPlanner")
    {
      PARAMETER_REGISTER(sidekick_scale) = 1.0;
			PARAMETER_REGISTER(short_kick_time) = 300;
			PARAMETER_REGISTER(long_kick_time) = 300;
			PARAMETER_REGISTER(sidekick_time) = 300;

      PARAMETER_REGISTER(approach_ball_adapt_control) = true;
      PARAMETER_REGISTER(approach_ball_adapt_threshold) = 10;
      PARAMETER_REGISTER(approach_ball_slow_distance) = 50;

      syncWithConfig();
    }
    virtual ~Parameters(){}

    double sidekick_scale;
		int short_kick_time;
		int long_kick_time;
		int sidekick_time;
    bool approach_ball_adapt_control;
    int approach_ball_adapt_threshold;
    int approach_ball_slow_distance;
  } params;

  // NONE means hip
  enum Foot
  {
    RIGHT,
    LEFT,
    NONE
  };
  typedef WalkRequest::StepControlRequest::StepType StepType;

  // XABSL go_to_ball_with_USOA
  void walk_to_ball(const Foot foot, const bool go_fast = false);
  // XABSL move_around_ball
  void move_around_ball(const double direction, const double radius);
  // XABSL go_to_ball_with_foot_dynamic
  void approach_ball(const Foot foot);
  // XABSL fast_forward_kick
  void short_kick(const Foot foot);
  // XABSL kick_with_foot
  void long_kick(const Foot foot);
  // XABSL sidekick (with foot == Foot::LEFT kicks to the left, and foot == FOOT::RIGHT to the right)
  void sidekick(const Foot foot);

  void control_ball(const Foot foot);

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
    WalkRequest::StepControlRequest::RestrictionMode restriction;
    bool isProtected;
  };
  std::vector<Step_Buffer_Element> step_buffer;

  // Used to alternate between left and right foot
  // or to specify which foot to use
  Foot foot_to_use;

  // Used to synchronize stepIDs of WalkEngine to take control
  unsigned int last_stepRequestID;

  void add_step(Pose2D &pose,
                const StepType &type,
                const WalkRequest::Coordinate &coordinate,
                const double character,
                const Foot foot,
                const double scale,
                const double speedDirection,
                const WalkRequest::StepControlRequest::RestrictionMode restriction,
                const bool isProtected,
                int step_time);
  void update_step(Pose2D &pose);
  void manage_step_buffer();
  void execute_step_buffer();


private:
  bool kick_planned;
};

#endif // _PathPlanner_H_
