#include "TeamCommDebugger.h"
#include <Tools/DataConversion.h>

void TeamCommDebugger::execute()
{
  // only send data in the given interval
  if(getWifiMode().wifiEnabled && (unsigned int)getFrameInfo().getTimeSince(lastSentTimestamp) > parameters.send_interval)
  {
      const auto& state = getTeamState().getPlayer(getPlayerInfo().playerNumber);

      // create the old SPL message
      SPLStandardMessage spl;

      spl.playerNum = (uint8_t) getPlayerInfo().playerNumber;
      spl.teamNum = (uint8_t) getPlayerInfo().teamNumber;

      spl.pose[0] = (float) state.pose().translation.x;
      spl.pose[1] = (float) state.pose().translation.y;
      spl.pose[2] = (float) state.pose().rotation;

      // in seconds (only if positive)!
      spl.ballAge = (float) state.ballAge();
      spl.ball[0] = (float) state.ballPosition().x;
      spl.ball[1] = (float) state.ballPosition().y;

      spl.fallen = (uint8_t) state.fallen();

      // user defined data
      naothmessages::BUUserTeamMessage userMsg;
      userMsg.set_bodyid(getRobotInfo().bodyID);
      userMsg.set_wasstriker(state.wasStriker());
      userMsg.set_wantstobestriker(state.wantsToBeStriker());
      userMsg.set_timestamp(state.messageTimestamp);
      userMsg.set_timetoball(state.timeToBall());
      userMsg.set_batterycharge((float)getBatteryData().charge);
      userMsg.set_temperature((float)std::max(getBodyState().temperatureLeftLeg, getBodyState().temperatureRightLeg));
      userMsg.set_cputemperature((float)getCpuData().temperature);
      userMsg.set_whistledetected(getWhistlePercept().whistleDetected);
      userMsg.set_whistlecount(getWhistlePercept().recognizedWhistles.size());

      Vector2d teamBall;
      if (getTeamBallModel().valid) {
          teamBall = getTeamBallModel().positionOnField;
      } else {
          // set teamball in teamcomm to an invalid value
          teamBall.x = std::numeric_limits<double>::infinity();
          teamBall.y = std::numeric_limits<double>::infinity();
      }
      DataConversion::toMessage(teamBall, *(userMsg.mutable_teamball()));

      Vector2d ballVelocity;
      if (getBallModel().knows) {
          ballVelocity = getBallModel().speed;
      } else {
          ballVelocity = {0, 0};
      }

      DataConversion::toMessage(ballVelocity, *(userMsg.mutable_ballvelocity()));
      userMsg.set_robotstate((naothmessages::RobotState)state.state());
      userMsg.mutable_robotrole()->set_role_static((naothmessages::RobotRoleStatic)state.robotRole().role);
      userMsg.mutable_robotrole()->set_role_dynamic((naothmessages::RobotRoleDynamic)state.robotRole().dynamic);
      userMsg.set_readytowalk(state.readyToWalk());
    
      size_t userSize = userMsg.ByteSize();
      spl.numOfDataBytes = static_cast<uint16_t>(userSize);
      userMsg.SerializeToArray(spl.data, static_cast<int>(userSize));
      
      // copy only the part of the message which is actually used
      getTeamMessageDebug().data.assign((char*)&spl, sizeof(SPLStandardMessage) - SPL_STANDARD_MESSAGE_DATA_SIZE + spl.numOfDataBytes);

      // remember the last sending time
      lastSentTimestamp = getFrameInfo().getTime();
  } else {
      getTeamMessageDebug().data.clear();
  }
}
 