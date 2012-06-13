/**
* @file AStartSearchParameters.cpp
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Kirill Yasinovskiy</a>
* Declaration of class AStartSearchParameters
*/

#include "AStarSearchParameters.h"
#include "Tools/Debug/DebugParameterList.h"

AStarSearchParameters::AStarSearchParameters()
  : ParameterList("AStarSearchParameters")
{
  PARAMETER_REGISTER(minBranchingFactor) = 5.0;
  PARAMETER_REGISTER(maxBranchingFactor) = 6.0;
  PARAMETER_REGISTER(minExpansionRadius) = 200.0;
  PARAMETER_REGISTER(maxExpansionRadius) = 400.0;
  PARAMETER_REGISTER(distanceToTarget) = 150.0;
  PARAMETER_REGISTER(distanceToOwnPenaltyArea) = 300.0;
  PARAMETER_REGISTER(obstacleRadius) = 150.0;
  PARAMETER_REGISTER(robotRadius) = 150;
  PARAMETER_REGISTER(endOfNear) = 300.0;
  PARAMETER_REGISTER(endOfFar) = 700.0;
  PARAMETER_REGISTER(standardGradientLength) = 0.0;
  PARAMETER_REGISTER(numberOfCalls) = 10.0;
  PARAMETER_REGISTER(maxNumberOfNodes) = 128.0;

  // load from the file after registering all parameters
  syncWithConfig();
  DebugParameterList::getInstance().add(this);
}

AStarSearchParameters::~AStarSearchParameters()
{
   DebugParameterList::getInstance().remove(this);
}
