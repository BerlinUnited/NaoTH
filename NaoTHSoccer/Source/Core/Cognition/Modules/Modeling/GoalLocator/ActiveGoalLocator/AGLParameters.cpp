/**
* @file AGLParameters.cpp
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
*
*/

#include "AGLParameters.h"
#include "Tools/Debug/DebugParameterList.h"

AGLParameters::AGLParameters()
  : ParameterList("AGLParameters")
{
  //Deviation of the second post (seenpost.position + goalwidth), if just one post is seen
  PARAMETER_REGISTER(standardDeviationDist) = 500;
  //in RAD
  PARAMETER_REGISTER(standardDeviationAngle) = 0.15;

  PARAMETER_REGISTER(timeFilterRange) = 0.1;

  PARAMETER_REGISTER(sigmaWeightingThreshhold) = 0.4;

  PARAMETER_REGISTER(thresholdCanopy) = 900;

  PARAMETER_REGISTER(processNoiseDistance) = 5;

  PARAMETER_REGISTER(resamplingThreshhold) = 0.99;

  // load from the file after registering all parameters
  loadFromConfig();

  DebugParameterList::getInstance().add(this);
}

AGLParameters::~AGLParameters()
{
  DebugParameterList::getInstance().remove(this);
}
