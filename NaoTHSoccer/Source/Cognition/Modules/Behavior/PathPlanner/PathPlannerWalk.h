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
  std::size_t last_stepID; // aka stepID_count
  struct Step {
    std::size_t ID;
    float x;
    float y;
    float rotation;
  };
  std::vector<Step> step_list;

  enum Foot
  {
    Right,
    Left
  };

  unsigned int last_stepcontrol_stepID;
  Foot foot_to_be_used;

  void stand();
  void add(PathPlannerWalk::Step step);
  void pop_step();
  PathPlannerWalk::Step new_step(float x, float y, float rotation);
  PathPlannerWalk::Step new_step(std::size_t ID, float x, float y, float rotation);
  float towards_ball(const char x_or_y);
};

#endif // _PathPlannerWalk_H_
