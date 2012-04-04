#include "FieldColorParameters.h"

FieldColorParameters::FieldColorParameters()
:
  ParameterList("FieldColorParameters")
{
  PARAMETER_REGISTER(CromaRedChannelDistance) = 10;
  PARAMETER_REGISTER(MaxCromaBlueChannelValue) = 80;

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

FieldColorParameters::~FieldColorParameters()
{
  DebugParameterList::getInstance().remove(this);
}
