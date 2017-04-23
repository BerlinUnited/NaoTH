/**
 * @file PathSymbols.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#include "PathSymbols.h"


void PathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // PathType
  engine.registerEnumElement("path.type", "path.type.none",                       PathModel::PathType::none);
  engine.registerEnumElement("path.type", "path.type.go_to_ball",                 PathModel::PathType::go_to_ball);
  engine.registerEnumElement("path.type", "path.type.go_to_ball_right",           PathModel::PathType::go_to_ball_right);
  engine.registerEnumElement("path.type", "path.type.go_to_ball_left",            PathModel::PathType::go_to_ball_left);
  engine.registerEnumElement("path.type", "path.type.go_to_ball_dynamic_right",   PathModel::PathType::go_to_ball_dynamic_right);
  engine.registerEnumElement("path.type", "path.type.go_to_ball_dynamic_left",    PathModel::PathType::go_to_ball_dynamic_left);
  engine.registerEnumElement("path.type", "path.type.move_around_ball",           PathModel::PathType::move_around_ball);
  engine.registerEnumElement("path.type", "path.type.prepare_walk_back_approach", PathModel::PathType::prepare_walk_back_approach);
  engine.registerEnumElement("path.type", "path.type.fast_forward_right",         PathModel::PathType::fast_forward_right);
  engine.registerEnumElement("path.type", "path.type.fast_forward_left",          PathModel::PathType::fast_forward_left);
  engine.registerEnumElement("path.type", "path.type.kick_with_foot_right",       PathModel::PathType::kick_with_foot_right);
  engine.registerEnumElement("path.type", "path.type.kick_with_foot_left",        PathModel::PathType::kick_with_foot_left);
  engine.registerEnumElement("path.type", "path.type.sidekick_left",              PathModel::PathType::sidekick_left);
  engine.registerEnumElement("path.type", "path.type.sidekick_right",             PathModel::PathType::sidekick_right);


  engine.registerEnumeratedOutputSymbol("path.type", "path.type", (int*)&getPathModel().pathType);

  // go to ball: distance and yOffset
  engine.registerDecimalOutputSymbol("path.goto_distance",
                                     &getPathModel().goto_distance);
  engine.registerDecimalOutputSymbol("path.goto_yOffset",
                                     &getPathModel().goto_yOffset);

  // move around ball: direction and radius
  engine.registerDecimalOutputSymbol("path.move_around_ball_direction",
                                     &getPathModel().move_around_ball_direction);
  engine.registerDecimalOutputSymbol("path.move_around_ball_radius",
                                     &getPathModel().move_around_ball_radius);
}

void PathSymbols::execute()
{
}
