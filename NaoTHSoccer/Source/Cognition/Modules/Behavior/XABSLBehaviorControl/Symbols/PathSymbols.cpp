/**
 * @file PathSymbols.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#include "PathSymbols.h"


void PathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // PathPlanner2018Routine
  engine.registerEnumElement("path2018.routine", "path2018.routine.none",              static_cast<int>(PathModel::PathPlanner2018Routine::NONE));
  engine.registerEnumElement("path2018.routine", "path2018.routine.avoid",             static_cast<int>(PathModel::PathPlanner2018Routine::AVOID));
  engine.registerEnumElement("path2018.routine", "path2018.routine.move_around_ball2", static_cast<int>(PathModel::PathPlanner2018Routine::MOVE_AROUND_BALL2));  
  engine.registerEnumElement("path2018.routine", "path2018.routine.forwardkick",       static_cast<int>(PathModel::PathPlanner2018Routine::FORWARDKICK));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidekick_left",     static_cast<int>(PathModel::PathPlanner2018Routine::SIDEKICK_LEFT));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidekick_right",    static_cast<int>(PathModel::PathPlanner2018Routine::SIDEKICK_RIGHT));
  engine.registerEnumElement("path2018.routine", "path2018.routine.sidestep",          static_cast<int>(PathModel::PathPlanner2018Routine::SIDESTEP));

  engine.registerEnumeratedOutputSymbol("path2018.routine", "path2018.routine", &setPathRoutine2018, &getPathRoutine2018);

  // go to ball: distance and yOffset
  engine.registerDecimalOutputSymbol("path.distance", &getPathModel().distance);
  engine.registerDecimalOutputSymbol("path.yOffset", &getPathModel().yOffset);  //TODO delete me

  // move around ball: direction and radius
  //engine.registerDecimalOutputSymbol("path.direction", &getPathModel().direction); // TODO this is in degrees should be converted here somehow
  engine.registerDecimalOutputSymbol("path.direction", &setDirection, &getDirection);
  engine.registerDecimalOutputSymbol("path.radius", &getPathModel().radius);
  engine.registerBooleanOutputSymbol("path.stable", &getPathModel().stable);

  engine.registerBooleanInputSymbol("path.kick_executed", &getPathModel().kick_executed);
}

PathSymbols* PathSymbols::theInstance = NULL;


void PathSymbols::execute()
{
}

void PathSymbols::setPathRoutine2018(int id) {
  theInstance->getPathModel().path2018_routine = static_cast<PathModel::PathPlanner2018Routine>(id);
}

int PathSymbols::getPathRoutine2018() {
  return static_cast<int>(theInstance->getPathModel().path2018_routine);
}

void PathSymbols::setDirection(double rot) {
  theInstance->getPathModel().direction = Math::fromDegrees(rot);
}

double PathSymbols::getDirection() {
  return Math::toDegrees(theInstance->getPathModel().direction);
}
