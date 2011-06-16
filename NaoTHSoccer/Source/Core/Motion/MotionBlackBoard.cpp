/*
 * File:   MotionBlackBoard.cpp
 * Author: Oliver Welter
 *
 * Created on 26. Januar 2009, 01:39
 */

#include "MotionBlackBoard.h"


MotionBlackBoard::MotionBlackBoard():
currentlyExecutedMotion(NULL)
{
}

void MotionBlackBoard::init()
{
  theSensorJointData.init();
  theKinematicChain.init(theSensorJointData);
  theKinematicChainModel.init(theMotorJointData);
}

MotionBlackBoard::~MotionBlackBoard()
{
}
