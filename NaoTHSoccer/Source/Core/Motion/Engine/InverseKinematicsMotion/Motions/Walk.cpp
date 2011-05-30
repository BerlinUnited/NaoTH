/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "Walk.h"

Walk::Walk()
:IKMotion(motion::WALK),
theWalkParameters(theParameters.walk)
{
  
}
  
void Walk::execute(const MotionRequest& motionRequest, MotionStatus& motionStatus)
{
  if ( FSRProtection() ) return;
  
  
}

bool Walk::FSRProtection()
{
  // no foot on the ground, stop walking
  if ( theWalkParameters.enableFSRProtection &&
    theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE ) 
  {
    //TODO: clear walk?
    return true;
  }
  else
  {
    return false;
  }
}
