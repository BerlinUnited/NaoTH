#include "FieldColorParameters.h"

FieldColorParameters::FieldColorParameters()
:
  ParameterList("FieldColorParameters")
{
  PARAMETER_REGISTER(fieldcolorDistMin.y) = 20;
  PARAMETER_REGISTER(fieldcolorDistMin.u) = 5;
  PARAMETER_REGISTER(fieldcolorDistMin.v) = 5;

  PARAMETER_REGISTER(fieldcolorDistMax.y) = 72;
  PARAMETER_REGISTER(fieldcolorDistMax.u) = 32;
  PARAMETER_REGISTER(fieldcolorDistMax.v) = 24;

  PARAMETER_REGISTER(fieldColorMin.y) = 5;
  PARAMETER_REGISTER(fieldColorMin.u) = 2;
  PARAMETER_REGISTER(fieldColorMin.v) = 2;

  PARAMETER_REGISTER(fieldColorMax.y) = 240;
  PARAMETER_REGISTER(fieldColorMax.u) = 255;
  PARAMETER_REGISTER(fieldColorMax.v) = 130;

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

FieldColorParameters::~FieldColorParameters()
{
  DebugParameterList::getInstance().remove(this);
}
