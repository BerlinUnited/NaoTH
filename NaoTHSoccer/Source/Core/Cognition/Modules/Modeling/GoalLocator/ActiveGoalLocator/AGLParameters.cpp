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
  //PARAMETER_REGISTER(standardDeviationDist) = 500;
  //in RAD
  PARAMETER_REGISTER(particleFilter.standardDeviationAngle) = 0.1;
  PARAMETER_REGISTER(particleFilter.standardDeviationDistance) = 0.1;
  PARAMETER_REGISTER(particleFilter.motionNoiseDistance) = 100;
  PARAMETER_REGISTER(particleFilter.motionNoiseRotation) = 0.03;

  PARAMETER_REGISTER(particleFilter.processNoiseDistance) = 0.01;
  PARAMETER_REGISTER(particleFilter.processNoiseRotation) = 0.01;

  PARAMETER_REGISTER(particleFilter.resamplingThreshhold) = 0;
  PARAMETER_REGISTER(particleFilter.particlesToReset) = 0;
  PARAMETER_REGISTER(particleFilter.numberOfParticles) = 10;
  


  PARAMETER_REGISTER(weightingTreshholdForUpdateWithAngle) = 0.5;
  PARAMETER_REGISTER(timeFilterRange) = 0.90;

  //PARAMETER_REGISTER(sigmaWeightingThreshhold) = 0.4;

  PARAMETER_REGISTER(thresholdCanopy) = 900;

  PARAMETER_REGISTER(possibleGoalWidhtError) = 200;

  PARAMETER_REGISTER(deletePFbyTotalWeightingThreshold) = 0.57;

  // load from the file after registering all parameters
  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

AGLParameters::~AGLParameters()
{
  DebugParameterList::getInstance().remove(this);
}
