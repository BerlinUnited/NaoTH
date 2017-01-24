/**
 * @file PathSymbols.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#include "PathSymbols.h"


void PathSymbols::registerSymbols(xabsl::Engine& engine)
{
  // go to ball with right or left foot
  engine.registerBooleanOutputSymbol("path.goto_ball_right",
                                     &getPathModel().goto_ball_right);
  engine.registerBooleanOutputSymbol("path.goto_ball_left",
                                     &getPathModel().goto_ball_left);
  // distance and yOffset
  engine.registerDecimalOutputSymbol("path.goto_distance",
                                     &getPathModel().goto_distance);
  engine.registerDecimalOutputSymbol("path.goto_yOffset",
                                     &getPathModel().goto_yOffset);
}

void PathSymbols::execute()
{

}
