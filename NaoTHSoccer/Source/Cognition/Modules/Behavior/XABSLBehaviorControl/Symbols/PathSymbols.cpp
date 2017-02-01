/**
 * @file PathSymbols.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#include "PathSymbols.h"


void PathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // PathType
  engine.registerEnumElement("path.type", "path.type.go_to_ball_right", PathModel::go_to_ball_right);
  engine.registerEnumElement("path.type", "path.type.go_to_ball_left", PathModel::go_to_ball_left);
  engine.registerEnumElement("path.type", "path.type.move_around_ball", PathModel::move_around_ball);
  engine.registerEnumElement("path.type", "path.type.fast_forward_right", PathModel::fast_forward_right);
  engine.registerEnumElement("path.type", "path.type.fast_forward_left", PathModel::fast_forward_left);

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
