#include "TeamCommSender.h"
#include "PlatformInterface/Platform.h"
#include <Messages/Representations.pb.h>

using namespace std;

TeamCommSender::TeamCommSender()
  :lastSentTimestamp(0),
    send_interval(500)
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
                                   const MotionStatus& motionStatus,
                                   const SoccerStrategy& soccerStrategy,
                                   const PlayersModel& playersModel,
                                   const TeamMessage& teamMessage,
                                   TeamMessage::Data &out)
{
  out.playerNum = playerInfo.gameData.playerNumber;
  out.team = playerInfo.gameData.teamNumber;
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
  if(bodyState.fall_down_state == BodyState::upright)
  {
    out.fallen = -1;
  }
  else
  {
    out.fallen = frameInfo.getTimeSince(bodyState.fall_down_state_time);
  }

  out.bodyID = robotInfo.bodyID;
  out.timeToBall = (unsigned int) soccerStrategy.timeToBall;
  out.wasStriker = playerInfo.isPlayingStriker;
  out.isPenalized = playerInfo.gameData.gameState == GameData::penalized;

  out.opponents.clear();
  out.opponents.reserve(playersModel.opponents.size());
  for(unsigned int i=0; i < playersModel.opponents.size(); i++)
  {
    const PlayersModel::Player& p = playersModel.opponents[i];
    // only add the players that where seen in this frame
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
              getRobotPose(), getBodyState(), getMotionStatus(), getSoccerStrategy(),
              getPlayersModel(), getTeamMessage(), data);
  // convert to SPLStandardMessage
  convertToSPLMessage(data, msg);
}

void TeamCommSender::convertToSPLMessage(const TeamMessage::Data& teamData, SPLStandardMessage& splMsg)
{
  if(teamData.playerNum < std::numeric_limits<uint8_t>::max())
  {
    splMsg.playerNum = (uint8_t) teamData.playerNum;
  }
  if(teamData.team < std::numeric_limits<uint16_t>::max())
  {
    splMsg.team = (uint16_t) teamData.team;
  }
  splMsg.pose[0] = (float) teamData.pose.translation.x;
  splMsg.pose[1] = (float) teamData.pose.translation.y;
  splMsg.pose[2] = (float) teamData.pose.rotation;

  splMsg.ballAge = teamData.ballAge;

  splMsg.ball[0] = (float) teamData.ballPosition.x;
  splMsg.ball[1] = (float) teamData.ballPosition.y;

  splMsg.ballVel[0] = (float) teamData.ballVelocity.x;
  splMsg.ballVel[1] = (float) teamData.ballVelocity.y;

  splMsg.fallen = teamData.fallen;

  // user defined data
  naothmessages::BUUserTeamMessage userMsg;
  userMsg.set_bodyid(teamData.bodyID);
  userMsg.set_timetoball(teamData.timeToBall);
  userMsg.set_wasstriker(teamData.wasStriker);
  userMsg.set_ispenalized(teamData.isPenalized);
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
