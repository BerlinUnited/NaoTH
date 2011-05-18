/*
 * File:   MotionBlackBoard.cpp
 * Author: Oliver Welter
 *
 * Created on 26. Januar 2009, 01:39
 */

#include "MotionBlackBoard.h"

MotionBlackBoard::MotionBlackBoard()
 : 
  lastTimeStamp(0),
  basicTimeStep(0),
  basicTimeStepInS(((double)basicTimeStep)/1000.0)
{
}

void MotionBlackBoard::init()
{
  theSensorJointData.init(Platform::getInstance().theConfigPathInfo.joint_info.c_str());
  theKinematicChain.init(theSensorJointData, Platform::getInstance().theMassConfig);
  theKinematicChainModel.init(theMotorJointData, Platform::getInstance().theMassConfig);
  theInertialSensorData.init();
}

MotionBlackBoard::~MotionBlackBoard()
{
}
