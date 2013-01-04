/**
* @file GPS_SelfLocator.h
*
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* Implementation of class GPS_SelfLocator
*/

#include "GPS_SelfLocator.h"

// debug
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"

GPS_SelfLocator::GPS_SelfLocator()
{
  DEBUG_REQUEST_REGISTER("GPS_SelfLocator:draw_GPSData", "draw the GPS data (only avaiables in webots)", false); 
  DEBUG_REQUEST_REGISTER("GPS_SelfLocator:use_GPSData", "use the GPS data (only avaiables in webots)", false);
}


void GPS_SelfLocator::execute()
{
  Pose2D tmpPose = calculateFromGPS(getGPSData());
  Pose2D gpsOdometryDelta = tmpPose - gpsRobotPose;
  gpsRobotPose = tmpPose;

  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  lastRobotOdometry = getOdometryData();

  DEBUG_REQUEST("GPS_SelfLocator:use_GPSData",
    getRobotPose() = gpsRobotPose;
  );

  DEBUG_REQUEST("GPS_SelfLocator:draw_GPSData",
    drawGPSData();
  );

  double x_factor = 1.0;
  MODIFY("GPS_SelfLocator:odometry_x_factor", x_factor);
  odometryDelta.translation.x *= x_factor;
  double difference = (gpsOdometryDelta.translation.x - odometryDelta.translation.x);
  PLOT("GPS_SelfLocator:odometry_error",difference);
  
}//end execute


void GPS_SelfLocator::drawGPSData()
{
  FIELD_DRAWING_CONTEXT;
  PEN("FFFFFF", 20);

  Pose2D pose = calculateFromGPS(getGPSData());
  ROBOT(pose.translation.x, pose.translation.y, pose.rotation);

  // assume getRobotPose() is already provided by another selflocator
  double difference = (pose.translation - getRobotPose().translation).abs();
  PLOT("GPS_SelfLocator:localization_error",difference);
}//end drawGPSData


Pose2D GPS_SelfLocator::calculateFromGPS(const GPSData& gps) const
{
  Pose2D pose;
  pose.translation.x = gps.data.translation.x;
  pose.translation.y = gps.data.translation.y;
  pose.rotation = gps.data.rotation.getZAngle();

  if ( getPlayerInfo().gameData.teamColor == GameData::blue )
  {
    //HACK (for webots): flip the position
    //pose.translation *= -1;
    pose.rotation = Math::normalizeAngle(pose.rotation+Math::pi);
  }

  return pose;
}//end calculateFromGPS


