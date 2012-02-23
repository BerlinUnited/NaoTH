#include "BaseColorRegionParameters.h"

BaseColorRegionParameters::BaseColorRegionParameters()
:
  ParameterList("BaseColorRegionParameters")
{
  PARAMETER_REGISTER(fieldIndex.y) = FIELD_IDX_Y;
  PARAMETER_REGISTER(fieldIndex.u) = FIELD_IDX_CB;
  PARAMETER_REGISTER(fieldIndex.v) = FIELD_IDX_CR;

  PARAMETER_REGISTER(fieldDist.y) = FIELD_DIST_Y;
  PARAMETER_REGISTER(fieldDist.u) = FIELD_DIST_CB;
  PARAMETER_REGISTER(fieldDist.v) = FIELD_DIST_CR;

  PARAMETER_REGISTER(goalIndex.y) = GOAL_IDX_Y;
  PARAMETER_REGISTER(goalIndex.u) = GOAL_IDX_CB;
  PARAMETER_REGISTER(goalIndex.v) = GOAL_IDX_CR;

  PARAMETER_REGISTER(goalDist.y) = GOAL_DIST_Y;
  PARAMETER_REGISTER(goalDist.u) = GOAL_DIST_CB;
  PARAMETER_REGISTER(goalDist.v) = GOAL_DIST_CR;

  PARAMETER_REGISTER(ballIndex.y) = BALL_IDX_Y;
  PARAMETER_REGISTER(ballIndex.u) = BALL_IDX_CB;
  PARAMETER_REGISTER(ballIndex.v) = BALL_IDX_CR;

  PARAMETER_REGISTER(ballDist.y) = BALL_DIST_Y;
  PARAMETER_REGISTER(ballDist.u) = BALL_DIST_CB;
  PARAMETER_REGISTER(ballDist.v) = BALL_DIST_CR;

  PARAMETER_REGISTER(lineIndex.y) = LINE_IDX_Y;
  PARAMETER_REGISTER(lineIndex.u) = LINE_IDX_CB;
  PARAMETER_REGISTER(lineIndex.v) = LINE_IDX_CR;

  PARAMETER_REGISTER(lineDist.y) = LINE_DIST_Y;
  PARAMETER_REGISTER(lineDist.u) = LINE_DIST_CB;
  PARAMETER_REGISTER(lineDist.v) = LINE_DIST_CR;

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

BaseColorRegionParameters::~BaseColorRegionParameters()
{
  DebugParameterList::getInstance().remove(this);
}

