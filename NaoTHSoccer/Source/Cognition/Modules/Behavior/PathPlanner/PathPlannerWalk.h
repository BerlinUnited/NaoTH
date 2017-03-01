/**
 * @file PathPlannerWalk.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class PathPlannerWalk
 */

#ifndef _PathPlannerWalk_H_
#define _PathPlannerWalk_H_

#include <ModuleFramework/Module.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Modeling/BallModel.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Modeling/PathModel.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

BEGIN_DECLARE_MODULE(PathPlannerWalk)
PROVIDE(DebugRequest)
PROVIDE(DebugModify)

REQUIRE(FrameInfo)
REQUIRE(FieldInfo)
REQUIRE(MotionStatus)
REQUIRE(BallPercept)
REQUIRE(BallPerceptTop)
REQUIRE(MultiBallPercept)
PROVIDE(BallModel)
REQUIRE(OdometryData)

PROVIDE(PathModel)

PROVIDE(HeadMotionRequest)
PROVIDE(MotionRequest)
END_DECLARE_MODULE(PathPlannerWalk)

class PathPlannerWalk: public PathPlannerWalkBase
{
public:
  PathPlannerWalk();
  ~PathPlannerWalk(){};

  virtual void execute();

  void execute_step_list();

private:
  struct Step {
    float x;
    float y;
    float rotation;
    float character;
  };
  std::vector<Step> step_list;

  enum Foot
  {
    Right,
    Left
  };
  // used to alternate between left and right foot when walking
  // inside execute_steplist()
  // everywhere else, indicates the last foot that has been moved
  Foot foot_to_be_used;

  unsigned int last_stepcontrol_stepID;

  void stand();
  void look_at_ball();
  
  float towards_ball(const char x_or_y);
  float towards_ball(const char x_or_y, const char for_left_or_right_foot);
  char approximate_foot_with(std::size_t approximate_steps_to_ball);

  void add(PathPlannerWalk::Step step);
  void pop_step();
  PathPlannerWalk::Step new_step(float x, float y, float rotation);
  PathPlannerWalk::Step new_step(float x, float y, float rotation, float character);
};

#endif // _PathPlannerWalk_H_
