/**
 * @file PathSymbols.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#include "PathSymbols.h"


void PathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // PathRoutine
  engine.registerEnumElement("path.routine", "path.routine.none",                static_cast<int>(PathModel::PathRoutine::NONE));
  engine.registerEnumElement("path.routine", "path.routine.go_to_ball_fast",     static_cast<int>(PathModel::PathRoutine::GO_TO_BALL_FAST));
  engine.registerEnumElement("path.routine", "path.routine.go_to_ball_slow",     static_cast<int>(PathModel::PathRoutine::GO_TO_BALL_SLOW));
  engine.registerEnumElement("path.routine", "path.routine.approach_ball_right", static_cast<int>(PathModel::PathRoutine::APPROACH_BALL_RIGHT));
  engine.registerEnumElement("path.routine", "path.routine.approach_ball_left",  static_cast<int>(PathModel::PathRoutine::APPROACH_BALL_LEFT));
  engine.registerEnumElement("path.routine", "path.routine.move_around_ball",    static_cast<int>(PathModel::PathRoutine::MOVE_AROUND_BALL));
  engine.registerEnumElement("path.routine", "path.routine.short_kick_right",    static_cast<int>(PathModel::PathRoutine::SHORT_KICK_RIGHT));
  engine.registerEnumElement("path.routine", "path.routine.short_kick_left",     static_cast<int>(PathModel::PathRoutine::SHORT_KICK_LEFT));
  engine.registerEnumElement("path.routine", "path.routine.long_kick_right",     static_cast<int>(PathModel::PathRoutine::LONG_KICK_RIGHT));
  engine.registerEnumElement("path.routine", "path.routine.long_kick_left",      static_cast<int>(PathModel::PathRoutine::LONG_KICK_LEFT));
  engine.registerEnumElement("path.routine", "path.routine.sidekick_right",      static_cast<int>(PathModel::PathRoutine::SIDEKICK_RIGHT));
  engine.registerEnumElement("path.routine", "path.routine.sidekick_left",       static_cast<int>(PathModel::PathRoutine::SIDEKICK_LEFT));

  // PathPlanner2018Routine
  engine.registerEnumElement("path2018.routine", "path2018.routine.none",              static_cast<int>(PathModel::PathPlanner2018Routine::NONE));
  engine.registerEnumElement("path2018.routine", "path2018.routine.move_around_ball",  static_cast<int>(PathModel::PathPlanner2018Routine::MOVE_AROUND_BALL));
  engine.registerEnumElement("path2018.routine", "path2018.routine.forwardkick_left",  static_cast<int>(PathModel::PathPlanner2018Routine::FORWARDKICK_LEFT));
  engine.registerEnumElement("path2018.routine", "path2018.routine.forwardkick_right", static_cast<int>(PathModel::PathPlanner2018Routine::FORWARDKICK_RIGHT));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidekick_left",     static_cast<int>(PathModel::PathPlanner2018Routine::SIDEKICK_LEFT));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidekick_right",    static_cast<int>(PathModel::PathPlanner2018Routine::SIDEKICK_RIGHT));

  engine.registerEnumeratedOutputSymbol("path.routine", "path.routine", &setPathRoutine, &getPathRoutine);
  engine.registerEnumeratedOutputSymbol("path2018.routine", "path2018.routine", &setPathRoutine2018, &getPathRoutine2018);

  // go to ball: distance and yOffset
  engine.registerDecimalOutputSymbol("path.distance", &getPathModel().distance);
  engine.registerDecimalOutputSymbol("path.yOffset", &getPathModel().yOffset);

  // move around ball: direction and radius
  engine.registerDecimalOutputSymbol("path.direction", &getPathModel().direction); // TODO this is in degrees should be converted here somehow
  engine.registerDecimalOutputSymbol("path.radius", &getPathModel().radius);

  engine.registerBooleanInputSymbol("path.kick_executed", &getPathModel().kick_executed);
}

PathSymbols* PathSymbols::theInstance = NULL;


void PathSymbols::execute()
{
}

void PathSymbols::setPathRoutine(int id) {
  theInstance->getPathModel().path_routine = static_cast<PathModel::PathRoutine>(id);
}

int PathSymbols::getPathRoutine() {
  return static_cast<int>(theInstance->getPathModel().path_routine);
}

void PathSymbols::setPathRoutine2018(int id) {
  theInstance->getPathModel().path2018_routine = static_cast<PathModel::PathPlanner2018Routine>(id);
}

int PathSymbols::getPathRoutine2018() {
  return static_cast<int>(theInstance->getPathModel().path2018_routine);
}
