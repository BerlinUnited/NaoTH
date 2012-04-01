#include "FieldColorParameters.h"

FieldColorParameters::FieldColorParameters()
:
  ParameterList("FieldColorParameters")
{
  PARAMETER_REGISTER(CromaRedChannelDistance) = 10;

  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

FieldColorParameters::~FieldColorParameters()
{
  DebugParameterList::getInstance().remove(this);
}
