/* 
 * File:   ALMotionParameters.cpp
 * Author: Heinrich Mellmann
 *
 * Created on 25. März 2009, 15:42
 */

#include "PFBLParameters.h"
#include "PlatformInterface/Platform.h"

PFBLParameters::PFBLParameters()
  : ParameterList("PFBLParameters")
{

  PARAMETER_REGISTER(frictionCoefficiant) = 0.1;
  PARAMETER_REGISTER(odometryNoiseFactor) = 0.1;
  PARAMETER_REGISTER(velocityNoiseFactor) = 0.1;

  PARAMETER_REGISTER(sigmaAngle) = 0.1;
  PARAMETER_REGISTER(sigmaDistance) = 0.1;

  // load from the file after registering all parameters
  syncWithConfig();
}

PFBLParameters::~PFBLParameters()
{
  
}
