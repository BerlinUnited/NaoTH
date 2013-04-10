/**
* @file OdometrySelfLocator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class OdometrySelfLocator
*/

#include "OdometrySelfLocator.h"

OdometrySelfLocator::OdometrySelfLocator()
{
  DEBUG_REQUEST_REGISTER("OdometrySelfLocator:draw_position","draw robot's position (self locator)", false);
  DEBUG_REQUEST_REGISTER("OdometrySelfLocator:reset","reset the robot's position", false);
}


void OdometrySelfLocator::execute()
{
  getRobotPose() += getOdometryData() - lastRobotOdometry;
  lastRobotOdometry = getOdometryData();

  DEBUG_REQUEST("OdometrySelfLocator:reset",
    getRobotPose().translation.x = 0;
    getRobotPose().translation.y = 0;
    getRobotPose().rotation = 0;
  );
}//end execute

