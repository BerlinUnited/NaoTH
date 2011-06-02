/**
* @file IKParameters.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implement of parameters for IK motion
*/

#include "IKParameters.h"


IKParameters::IKParameters()
{
  hipOffsetX = 0;
  footOffsetY = 0;
  
  stand.speed = 0.04;
  
  walk.comHeight = 260;
  walk.enableFSRProtection = true;
}