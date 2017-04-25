/**
 * @file PathSymbols.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#include "PathSymbols.h"


void PathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // PathRoutine
  engine.registerEnumElement("path.routine", "path.routine.none",                PathModel::PathRoutine::NONE);
  engine.registerEnumElement("path.routine", "path.routine.go_to_ball",          PathModel::PathRoutine::GO_TO_BALL);
  engine.registerEnumElement("path.routine", "path.routine.go_to_ball_right",    PathModel::PathRoutine::GO_TO_BALL_RIGHT);
  engine.registerEnumElement("path.routine", "path.routine.go_to_ball_left",     PathModel::PathRoutine::GO_TO_BALL_LEFT);
  engine.registerEnumElement("path.routine", "path.routine.approach_ball_right", PathModel::PathRoutine::APPROACH_BALL_RIGHT);
  engine.registerEnumElement("path.routine", "path.routine.approach_ball_left",  PathModel::PathRoutine::APPROACH_BALL_LEFT);
  engine.registerEnumElement("path.routine", "path.routine.move_around_ball",    PathModel::PathRoutine::MOVE_AROUND_BALL);
  engine.registerEnumElement("path.routine", "path.routine.short_kick_right",    PathModel::PathRoutine::SHORT_KICK_RIGHT);
  engine.registerEnumElement("path.routine", "path.routine.short_kick_left",     PathModel::PathRoutine::SHORT_KICK_LEFT);
  engine.registerEnumElement("path.routine", "path.routine.long_kick_right",     PathModel::PathRoutine::LONG_KICK_RIGHT);
  engine.registerEnumElement("path.routine", "path.routine.long_kick_left",      PathModel::PathRoutine::LONG_KICK_LEFT);
  engine.registerEnumElement("path.routine", "path.routine.sidekick_right",      PathModel::PathRoutine::SIDEKICK_RIGHT);
  engine.registerEnumElement("path.routine", "path.routine.sidekick_left",       PathModel::PathRoutine::SIDEKICK_LEFT);


  engine.registerEnumeratedOutputSymbol("path.routine", "path.routine", (int*)&getPathModel().path_routine);

  // go to ball: distance and yOffset
  engine.registerDecimalOutputSymbol("path.distance",
                                     &getPathModel().distance);
  engine.registerDecimalOutputSymbol("path.yOffset",
                                     &getPathModel().yOffset);

  // move around ball: direction and radius
  engine.registerDecimalOutputSymbol("path.direction",
                                     &getPathModel().direction);
  engine.registerDecimalOutputSymbol("path.radius",
                                     &getPathModel().radius);

  engine.registerBooleanInputSymbol("path.routine_executed", &getPathModel().routine_executed);
}

void PathSymbols::execute()
{
}
