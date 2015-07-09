#include "TeamCommSender.h"
#include "PlatformInterface/Platform.h"
#include <Messages/Representations.pb.h>

#include <Tools/NaoTime.h>

using namespace std;

TeamCommSender::TeamCommSender()
  :lastSentTimestamp(0),
    send_interval(400)
{
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  if ( config.hasKey("teamcomm", "send_interval") )
  {
    send_interval = config.getInt("teamcomm", "send_interval");
  }
}

void TeamCommSender::execute()
{
  // only send data in intervals of 500ms
  if((unsigned int)getFrameInfo().getTimeSince(lastSentTimestamp) > send_interval)
  {
    // send data
    SPLStandardMessage msg;
    createMessage(msg);

    getTeamMessageDataOut().data.assign((char*) &msg, sizeof(SPLStandardMessage));

    lastSentTimestamp = getFrameInfo().getTime();
  }
  else
  {
    getTeamMessageDataOut().data.clear();
  }
}

void TeamCommSender::fillMessage(const PlayerInfo& playerInfo,
                                   const RobotInfo& robotInfo,
                                   const FrameInfo& frameInfo,
                                   const BallModel& ballModel,
                                   const RobotPose& robotPose,
                                   const BodyState& bodyState,
                                   const RoleDecisionModel& roleDecisionModel,
                                   const SoccerStrategy& soccerStrategy,
                                   const PlayersModel& playersModel,
                                   const BatteryData &batteryData,
                                   TeamMessage::Data &out)
{
  out.playerNum = playerInfo.gameData.playerNumber;
  out.teamNumber = playerInfo.gameData.teamNumber;
  out.pose = robotPose;

  if(ballModel.valid)
  {
    out.ballAge = frameInfo.getTimeSince(ballModel.frameInfoWhenBallWasSeen.getTime());
    out.ballPosition = ballModel.position;
    out.ballVelocity = ballModel.speed;
  }
  else
  {
    out.ballAge = -1;
    out.ballPosition.x = std::numeric_limits<double>::max();
    out.ballPosition.y = std::numeric_limits<double>::max();
    out.ballVelocity.x = 0;
    out.ballVelocity.y = 0;
  }
  
  out.fallen = bodyState.fall_down_state != BodyState::upright;
  out.bodyID = robotInfo.bodyID;
  out.timestamp = naoth::NaoTime::getSystemTimeInMilliSeconds(); //frameInfo.getTime();
  out.timeToBall = (unsigned int) soccerStrategy.timeToBall;
  out.wasStriker = playerInfo.isPlayingStriker;
  out.wantsToBeStriker = roleDecisionModel.wantsToBeStriker;
  out.isPenalized = playerInfo.gameData.gameState == GameData::penalized;
  out.batteryCharge = (float) batteryData.charge;
  out.temperature =
      (float) std::max(bodyState.temperatureLeftLeg, bodyState.temperatureRightLeg);

  out.opponents.clear();
  out.opponents.reserve(playersModel.opponents.size());
  for(size_t i=0; i < playersModel.opponents.size(); i++)
  {
    const PlayersModel::Player& p = playersModel.opponents[i];
    // only add the players that were seen in this frame
    if(p.frameInfoWhenWasSeen.getFrameNumber() == frameInfo.getFrameNumber())
    {
      TeamMessage::Opponent opp;
      opp.playerNum = p.number;
      opp.poseOnField = p.globalPose;
      out.opponents.push_back(opp);
    }
  }

}

void TeamCommSender::createMessage(SPLStandardMessage &msg)
{
  TeamMessage::Data data;
  fillMessage(getPlayerInfo(), getRobotInfo(), getFrameInfo(), getBallModel(),
              getRobotPose(), getBodyState(), getRoleDecisionModel(), getSoccerStrategy(),
              getPlayersModel(), getBatteryData(), data);
  // convert to SPLStandardMessage
  convertToSPLMessage(data, msg);
}

void TeamCommSender::convertToSPLMessage(const TeamMessage::Data& teamData, SPLStandardMessage& splMsg)
{
  if(teamData.playerNum < std::numeric_limits<uint8_t>::max())
  {
    splMsg.playerNum = (uint8_t) teamData.playerNum;
  }
  if(teamData.teamNumber < std::numeric_limits<uint8_t>::max())
  {
    splMsg.teamNum = (uint8_t) teamData.teamNumber;
  }

  splMsg.pose[0] = (float) teamData.pose.translation.x;
  splMsg.pose[1] = (float) teamData.pose.translation.y;
  splMsg.pose[2] = (float) teamData.pose.rotation;

  // convert milliseconds to seconds
  splMsg.ballAge = (float) ((double) teamData.ballAge / 1000.0);

  splMsg.ball[0] = (float) teamData.ballPosition.x;
  splMsg.ball[1] = (float) teamData.ballPosition.y;

  splMsg.ballVel[0] = (float) teamData.ballVelocity.x;
  splMsg.ballVel[1] = (float) teamData.ballVelocity.y;

  splMsg.fallen = (uint8_t) teamData.fallen;

  // TODO: actually set walkingTo when we are walking to some point
  splMsg.walkingTo[0] = splMsg.pose[0];
  splMsg.walkingTo[1] = splMsg.pose[1];
  // TODO: actually set shootingTo when we are shooting to some point
  splMsg.shootingTo[0] = splMsg.pose[0];
  splMsg.shootingTo[1] = splMsg.pose[1];

  // TODO: suggest something
  for(int i = 0; i < SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS; ++i)
  {
    splMsg.suggestion[i] = 0;
  }

  // TODO: map more behavior states (attacker etc.) to our intention
  splMsg.intention = 0;
  if(teamData.wasStriker)
  {
    splMsg.intention = 3;
  }
  else if(teamData.playerNum == 1)
  {
    // play keeper
    splMsg.intention = 1;
  }

  // user defined data
  naothmessages::BUUserTeamMessage userMsg;
  userMsg.set_bodyid(teamData.bodyID);
  userMsg.set_wasstriker(teamData.wasStriker);
  userMsg.set_wantstobestriker(teamData.wantsToBeStriker);
  userMsg.set_timestamp(teamData.timestamp);
  userMsg.set_timetoball(teamData.timeToBall);
  userMsg.set_ispenalized(teamData.isPenalized);
  userMsg.set_batterycharge(teamData.batteryCharge);
  userMsg.set_temperature(teamData.temperature);
  for(unsigned int i=0; i < teamData.opponents.size(); i++)
  {
    naothmessages::Opponent* opp = userMsg.add_opponents();
    opp->set_playernum(teamData.opponents[i].playerNum);
    DataConversion::toMessage(teamData.opponents[i].poseOnField, *(opp->mutable_poseonfield()));
  }

  int userSize = userMsg.ByteSize();
  if(splMsg.numOfDataBytes < SPL_STANDARD_MESSAGE_DATA_SIZE)
  {
    splMsg.numOfDataBytes = (uint16_t) userMsg.ByteSize();
    userMsg.SerializeToArray(splMsg.data, userSize);
  }
  else
  {
    splMsg.numOfDataBytes = 0;
  }


}

void TeamCommSender::addSendOppModel(unsigned int oppNum,
                                     const PlayersModel& playersModel,
                                     TeamMessage::Opponent& out)
{
  for (vector<PlayersModel::Player>::const_iterator iter = playersModel.opponents.begin();
    iter != playersModel.opponents.end(); ++iter)
  {
    if ( iter->number == oppNum )
    {
      out.playerNum = oppNum;
      out.poseOnField = iter->globalPose;
      break;
    }
  }
}
