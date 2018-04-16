
/**
* @file BDRBallDetector.cpp
*
* Implementation of class BDRBallDetector
*
*/

#include "BDRBallDetector.h"


BDRBallDetector::BDRBallDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:BDRBallDetector:draw_ball_candidates", "..", false);

  getDebugParameterList().add(&params);
}

BDRBallDetector::~BDRBallDetector()
{
  getDebugParameterList().remove(&params);
}

void BDRBallDetector::execute(CameraInfo::CameraID id)
{
  

}
