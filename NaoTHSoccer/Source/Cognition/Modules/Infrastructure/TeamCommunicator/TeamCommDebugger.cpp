#include "TeamCommDebugger.h"
#include <Tools/DataConversion.h>

void TeamCommDebugger::execute()
{
  getTeamMessageDebug().host = parameters.host;
  getTeamMessageDebug().port = parameters.port;

  // only send data in the given interval
  if(getWifiMode().wifiEnabled && (unsigned int)getFrameInfo().getTimeSince(lastSentTimestamp) > parameters.send_interval)
  {
      const auto& state = getTeamState().getPlayer(getPlayerInfo().playerNumber);

      naothmessages::TeamMessageDebug debugMessage;
      debugMessage.set_bodyid(getRobotInfo().bodyId);
      debugMessage.set_teamnumber(getPlayerInfo().teamNumber);
      debugMessage.set_playernumber(getPlayerInfo().playerNumber);
      debugMessage.set_timestamp(state.messageTimestamp);
      debugMessage.mutable_frameinfo()->set_framenumber(getFrameInfo().getFrameNumber());
      debugMessage.mutable_frameinfo()->set_time(getFrameInfo().getTime());
      debugMessage.set_robotstate((naothmessages::RobotState)state.state());
      debugMessage.mutable_robotrole()->set_role_static((naothmessages::RobotRoleStatic)state.robotRole().role);
      debugMessage.mutable_robotrole()->set_role_dynamic((naothmessages::RobotRoleDynamic)state.robotRole().dynamic);
      DataConversion::toMessage(state.pose(), *(debugMessage.mutable_pose()));
      debugMessage.set_fallen(state.fallen());
      debugMessage.set_readytowalk(state.readyToWalk());
      debugMessage.set_batterycharge((float)getBatteryData().charge);
      debugMessage.set_temperature((float)std::max(getBodyState().temperatureLeftLeg, getBodyState().temperatureRightLeg));
      debugMessage.set_cputemperature((float)getCpuData().temperature);
      debugMessage.set_whistledetected(getWhistlePercept().whistleDetected);
      debugMessage.set_whistlecount(getWhistlePercept().recognizedWhistles.size());
      debugMessage.set_ballage(state.ballAge());
      DataConversion::toMessage(state.ballPosition(), *(debugMessage.mutable_ballposition()));
      DataConversion::toMessage(getBallModel().knows ? getBallModel().speed : Vector2d{0, 0}, *(debugMessage.mutable_ballvelocity()));
      //debugMessage.set_timetoball();
      debugMessage.set_wasstriker(getRoleDecisionModel().isStriker(getPlayerInfo().playerNumber));
      debugMessage.set_wantstobestriker(getRoleDecisionModel().wantsToBeStriker);

      Vector2d teamBall;
      if (getTeamBallModel().valid) {
          teamBall = getTeamBallModel().positionOnField;
      } else {
          // set teamball in teamcomm to an invalid value
          teamBall.x = std::numeric_limits<double>::infinity();
          teamBall.y = std::numeric_limits<double>::infinity();
      }
      DataConversion::toMessage(teamBall, *(debugMessage.mutable_teamball()));

      // add prefix for easier identification
      getTeamMessageDebug().data = "DBG " + debugMessage.SerializeAsString();

      getTeamMessageDebug().lastSend = getFrameInfo();
      getTeamMessageDebug().interval = getFrameInfo().getTime() - lastSentTimestamp;
      // remember the last sending time
      lastSentTimestamp = getFrameInfo().getTime();
  } else {
      getTeamMessageDebug().data.clear();
  }
}
 