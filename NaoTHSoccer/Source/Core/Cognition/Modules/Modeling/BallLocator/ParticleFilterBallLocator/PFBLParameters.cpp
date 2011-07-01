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
  PARAMETER_REGISTER(sigmaDistanceOwnPose) = 900;
  PARAMETER_REGISTER(sigmaAngleOwnPose) = 0.6;

  PARAMETER_REGISTER(sigmaDistanceGoalModel) = 1000;
  PARAMETER_REGISTER(sigmaAngleGoalModel) = 0.3;

  PARAMETER_REGISTER(sigmaDistanceGoalPost) = 1000;
  PARAMETER_REGISTER(sigmaAngleGoalPost) = 0.6;

  PARAMETER_REGISTER(sigmaDistanceCenterCircle) = 1000;
  PARAMETER_REGISTER(sigmaAngleCenterCircle) = 0.3;

  PARAMETER_REGISTER(sigmaDistanceCorner) = 1000;
  PARAMETER_REGISTER(sigmaAngleCorner) = 0.3;

  PARAMETER_REGISTER(sigmaDistanceLine) = 1000;
  PARAMETER_REGISTER(sigmaAngleLine) = 0.6;

  PARAMETER_REGISTER(thresholdCanopy) = 900;

  PARAMETER_REGISTER(resamplingThreshhold) = 0.01;

  PARAMETER_REGISTER(processNoiseDistance) = 120;
  PARAMETER_REGISTER(processNoiseAngle) = 0.1;

  PARAMETER_REGISTER(motionNoiseDistance) = 10;
  PARAMETER_REGISTER(motionNoiseAngle) = 0.01;


  PARAMETER_REGISTER(updateByGoals) = 1;
  PARAMETER_REGISTER(updateByOldPose) = 0;
  PARAMETER_REGISTER(updateByLinesTable) = 1;
  PARAMETER_REGISTER(updateByCornerTable) = 1;
  PARAMETER_REGISTER(updateByCenterCircle) = 1;
  PARAMETER_REGISTER(updateByFlags) = 0;

  // load from the file after registering all parameters
  syncWithConfig();
}

PFBLParameters::~PFBLParameters()
{
  
}
