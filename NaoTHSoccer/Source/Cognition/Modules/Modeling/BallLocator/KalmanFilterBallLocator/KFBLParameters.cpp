/**
* @file KFBLParameters.h copied by MCSLParameters
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
*
* Created on 16. Mai 2011, 21:53
*/

#include "KFBLParameters.h"
#include "Tools/Debug/DebugParameterList.h"

KFBLParameters::KFBLParameters()
  : ParameterList("KFBLParameters")
{
  PARAMETER_REGISTER(sigmaBallPosition) = 100;
  PARAMETER_REGISTER(sigmaBallPositionReactive) = 100;
  PARAMETER_REGISTER(sigmaBallSpeed) = 1000;
  PARAMETER_REGISTER(sigmaBallSpeedReactive) = 350;

  PARAMETER_REGISTER(processNoise) = 3;

  PARAMETER_REGISTER(ballMass) = 0.026;
  PARAMETER_REGISTER(frictionCoefficiant) = 0.3;

  PARAMETER_REGISTER(randomizeFrameSelection) = 0;

  // load from the file after registering all parameters
  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

KFBLParameters::~KFBLParameters()
{
  DebugParameterList::getInstance().remove(this);
}
