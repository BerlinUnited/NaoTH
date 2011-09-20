/* 
 * File:   ALMotionParameters.cpp
 * Author: Heinrich Mellmann
 *
 * Created on 25. März 2009, 15:42
 */

#include "MCSLParameters.h"
#include "Tools/Debug/DebugParameterList.h"

MCSLParameters::MCSLParameters()
  : ParameterList("MCSLParameters")
{
  PARAMETER_REGISTER(sigmaDistanceOwnPose) = 900;
  PARAMETER_REGISTER(sigmaAngleOwnPose) = 0.6;

  PARAMETER_REGISTER(sigmaDistanceGoalModel) = 500;
  PARAMETER_REGISTER(sigmaAngleGoalModel) = 0.2;

  PARAMETER_REGISTER(sigmaDistanceGoalPost) = 0.1;
  PARAMETER_REGISTER(sigmaAngleGoalPost) = 0.2;

  PARAMETER_REGISTER(sigmaDistanceCenterCircle) = 1000;
  PARAMETER_REGISTER(sigmaAngleCenterCircle) = 0.2;

  PARAMETER_REGISTER(sigmaDistanceCorner) = 0.2;
  PARAMETER_REGISTER(sigmaAngleCorner) = 0.2;

  PARAMETER_REGISTER(sigmaDistanceLine) = 0.2;
  PARAMETER_REGISTER(sigmaAngleLine) = 0.2;

  PARAMETER_REGISTER(thresholdCanopy) = 900;

  PARAMETER_REGISTER(resamplingThreshhold) = 0.07;

  PARAMETER_REGISTER(processNoiseDistance) = 70;
  PARAMETER_REGISTER(processNoiseAngle) = 0.1;

  PARAMETER_REGISTER(motionNoiseDistance) = 10;
  PARAMETER_REGISTER(motionNoiseAngle) = 0.01;


  PARAMETER_REGISTER(updateByGoals) = 1;
  PARAMETER_REGISTER(updateByOldPose) = 0;
  PARAMETER_REGISTER(updateByLinesTable) = 1;
  PARAMETER_REGISTER(updateByCornerTable) = 1;
  PARAMETER_REGISTER(updateByCenterCircle) = 1;
  PARAMETER_REGISTER(updateByFlags) = 0;

  PARAMETER_REGISTER(treatFallDown) = 0;
  PARAMETER_REGISTER(treatLiftUp) = 0;

  // load from the file after registering all parameters
  syncWithConfig();

  DebugParameterList::getInstance().add(this);
}

MCSLParameters::~MCSLParameters()
{
  DebugParameterList::getInstance().remove(this);
}
