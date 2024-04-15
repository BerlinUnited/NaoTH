/**
 * @file PathSymbols.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#include "PathSymbols.h"


void PathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // PathPlanner2018Routine
  engine.registerEnumElement("path2018.routine", "path2018.routine.none",              static_cast<int>(PathRequest::PathID::NONE));
  engine.registerEnumElement("path2018.routine", "path2018.routine.avoid",             static_cast<int>(PathRequest::PathID::AVOID));
  engine.registerEnumElement("path2018.routine", "path2018.routine.move_around_ball_old", static_cast<int>(PathRequest::PathID::MOVE_AROUND_BALL_OLD));
  engine.registerEnumElement("path2018.routine", "path2018.routine.move_around_ball2", static_cast<int>(PathRequest::PathID::MOVE_AROUND_BALL2)); 
  engine.registerEnumElement("path2018.routine", "path2018.routine.forwardkick",       static_cast<int>(PathRequest::PathID::FORWARDKICK));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidekick_left",     static_cast<int>(PathRequest::PathID::SIDEKICK_LEFT));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidekick_right",    static_cast<int>(PathRequest::PathID::SIDEKICK_RIGHT));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidestep",          static_cast<int>(PathRequest::PathID::SIDESTEP));

  engine.registerEnumeratedOutputSymbol("path2018.routine", "path2018.routine", &setPathRoutine2018, &getPathRoutine2018);

  // move around ball: direction and radius
  engine.registerDecimalOutputSymbol("path.direction", &setDirection, &getDirection);
  engine.registerDecimalOutputSymbol("path.radius", &getPathRequest().radius);
  engine.registerBooleanOutputSymbol("path.stable", &getPathRequest().stable);

  // input
  engine.registerBooleanInputSymbol("path.kick_executed", &getPathStatus().kick_executed);
}

PathSymbols* PathSymbols::theInstance = NULL;


void PathSymbols::execute()
{
}

void PathSymbols::setPathRoutine2018(int id) {
  theInstance->getPathRequest().path2018_routine = static_cast<PathRequest::PathID>(id);
}

int PathSymbols::getPathRoutine2018() {
  return static_cast<int>(theInstance->getPathRequest().path2018_routine);
}

void PathSymbols::setDirection(double rot) {
  theInstance->getPathRequest().direction = Math::fromDegrees(rot);
}

double PathSymbols::getDirection() {
  return Math::toDegrees(theInstance->getPathRequest().direction);
}
