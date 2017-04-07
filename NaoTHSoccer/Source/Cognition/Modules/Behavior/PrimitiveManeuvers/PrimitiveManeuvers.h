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
#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Modeling/BallModel.h"


BEGIN_DECLARE_MODULE(PrimitiveManeuvers)
PROVIDE(DebugRequest)
PROVIDE(DebugModify)

REQUIRE(FieldInfo)
REQUIRE(MultiBallPercept)
REQUIRE(MotionStatus)
REQUIRE(BallModel)

PROVIDE(MotionRequest)
PROVIDE(HeadMotionRequest)
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

  // Primitive Maneuvers

  // Walks to the ball
  // returns true if reached
  bool MWalk_to_ball(Foot foot);

  // Limit steps depending on rotation
  // returns Vector2d with x component stepX and y component stepRotation
  Vector2d limit_step(Vector2d step);

  // Stepcontrol
  struct Step {
    double x;
    double y;
    double rotation;
    double character;
  };
  std::vector<Step> step_list;

  // used to alternate between left and right foot when walking
  // inside execute_steplist()
  // everywhere else, indicates the last foot that has been moved
  Foot foot_to_be_used;

  unsigned int last_stepcontrol_stepID;

  void prepare_walk();

  bool add(PrimitiveManeuvers::Step step);
  void pop_step();
  PrimitiveManeuvers::Step new_step(double x, double y, double rotation);
  PrimitiveManeuvers::Step new_step(double x, double y, double rotation,
                                    double character);
};

#endif // _PrimitiveManeuvers_H_
