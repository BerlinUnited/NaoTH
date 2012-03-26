#include "BaseColorRegionParameters.h"

BaseColorRegionParameters::BaseColorRegionParameters()
:
  ParameterList("BaseColorRegionParameters")
{
  PARAMETER_REGISTER(fieldIndex.y) = 125.0;
  PARAMETER_REGISTER(fieldIndex.u) = 127.0;
  PARAMETER_REGISTER(fieldIndex.v) = 63.0;

  PARAMETER_REGISTER(fieldDist.y) = 120.0;
  PARAMETER_REGISTER(fieldDist.u) = 128.0;
  PARAMETER_REGISTER(fieldDist.v) = 64.0;

  PARAMETER_REGISTER(goalIndex.y) = 180.0;
  PARAMETER_REGISTER(goalIndex.u) = 80.0;
  PARAMETER_REGISTER(goalIndex.v) = 135.0;

  PARAMETER_REGISTER(goalDist.y) = 120.0;
  PARAMETER_REGISTER(goalDist.u) = 24.0;
  PARAMETER_REGISTER(goalDist.v) = 24.0;

  PARAMETER_REGISTER(ballIndex.y) = 140.0;
  PARAMETER_REGISTER(ballIndex.u) = 85.0;
  PARAMETER_REGISTER(ballIndex.v) = 200.0;

  PARAMETER_REGISTER(ballDist.y) = 160.0;
  PARAMETER_REGISTER(ballDist.u) = 96.0;
  PARAMETER_REGISTER(ballDist.v) = 30.0;

  PARAMETER_REGISTER(lineIndex.y) = 254.0;
  PARAMETER_REGISTER(lineIndex.u) = 135.0;
  PARAMETER_REGISTER(lineIndex.v) = 125.0;

  PARAMETER_REGISTER(lineDist.y) = 96.0;
  PARAMETER_REGISTER(lineDist.u) = 8.0;
  PARAMETER_REGISTER(lineDist.v) = 8.0;

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

BaseColorRegionParameters::~BaseColorRegionParameters()
{
  DebugParameterList::getInstance().remove(this);
}

