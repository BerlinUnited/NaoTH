#include "FieldColorParameters.h"

FieldColorParameters::FieldColorParameters()
:
  ParameterList("FieldColorParameters")
{
  PARAMETER_REGISTER(fieldcolorDistMin.y) = MIN_FIELD_COLOR_Y_CHANNEL_DIST;
  PARAMETER_REGISTER(fieldcolorDistMin.u) = MIN_FIELD_COLOR_CB_CHANNEL_DIST;
  PARAMETER_REGISTER(fieldcolorDistMin.v) = MIN_FIELD_COLOR_CR_CHANNEL_DIST;

  PARAMETER_REGISTER(fieldcolorDistMax.y) = MAX_FIELD_COLOR_Y_CHANNEL_DIST;
  PARAMETER_REGISTER(fieldcolorDistMax.u) = MAX_FIELD_COLOR_CB_CHANNEL_DIST;
  PARAMETER_REGISTER(fieldcolorDistMax.v) = MAX_FIELD_COLOR_CR_CHANNEL_DIST;

  PARAMETER_REGISTER(fieldColorMin.y) = MIN_FIELD_COLOR_Y_LEVEL;
  PARAMETER_REGISTER(fieldColorMin.u) = MIN_FIELD_COLOR_CB_LEVEL;
  PARAMETER_REGISTER(fieldColorMin.v) = MIN_FIELD_COLOR_CR_LEVEL;

  PARAMETER_REGISTER(fieldColorMax.y) = MAX_FIELD_COLOR_Y_LEVEL;
  PARAMETER_REGISTER(fieldColorMax.u) = MAX_FIELD_COLOR_CB_LEVEL;
  PARAMETER_REGISTER(fieldColorMax.v) = MAX_FIELD_COLOR_CR_LEVEL;

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

FieldColorParameters::~FieldColorParameters()
{
  DebugParameterList::getInstance().remove(this);
}
