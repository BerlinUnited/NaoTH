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
                                   bool onlySendOneOpponent,
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
  if(onlySendOneOpponent)
  {
    unsigned int oppNum = selectSendOpp(playersModel, frameInfo,
                                        teamMessage);
    if (oppNum != 0)
    {
      out.opponents = std::vector<TeamMessage::Opponent>(1);
      addSendOppModel(oppNum, playersModel, out.opponents[0]);
    }
  }
  else
  {

    out.opponents = std::vector<TeamMessage::Opponent>(playersModel.opponents.size());
    for(unsigned int i=0; i < playersModel.opponents.size(); i++)
    {
      out.opponents[i].playerNum = playersModel.opponents[i].number;
      out.opponents[i].poseOnField = playersModel.opponents[i].globalPose;
    }
  }
}

void TeamCommSender::createMessage(SPLStandardMessage &msg)
{
  TeamMessage::Data data;
  fillMessage(getPlayerInfo(), getRobotInfo(), getFrameInfo(), getBallModel(),
              getRobotPose(), getBodyState(), getMotionStatus(), getSoccerStrategy(),
              getPlayersModel(), getTeamMessage(), true, data);
  // convert to SPLStandardMessage
  if(data.playerNum < std::numeric_limits<uint8_t>::max())
  {
    msg.playerNum = (uint8_t) data.playerNum;
  }
  if(data.team < std::numeric_limits<uint16_t>::max())
  {
    msg.team = (uint16_t) data.team;
  }
  msg.pose[0] = (float) data.pose.translation.x;
  msg.pose[1] = (float) data.pose.translation.y;
  msg.pose[2] = (float) data.pose.rotation;

  msg.ballAge = data.ballAge;

  msg.ball[0] = (float) data.ballPosition.x;
  msg.ball[1] = (float) data.ballPosition.y;

  msg.ballVel[0] = (float) data.ballVelocity.x;
  msg.ballVel[1] = (float) data.ballVelocity.y;

  msg.fallen = data.fallen;

  // user defined data
  naothmessages::BUUserTeamMessage userMsg;
  userMsg.set_bodyid(data.bodyID);
  userMsg.set_timetoball(data.timeToBall);
  userMsg.set_wasstriker(data.wasStriker);
  userMsg.set_ispenalized(data.isPenalized);
  for(unsigned int i=0; i < data.opponents.size(); i++)
  {
    naothmessages::Opponent* opp = userMsg.add_opponents();
    opp->set_playernum(data.opponents[i].playerNum);
    DataConversion::toMessage(data.opponents[i].poseOnField, *(opp->mutable_poseonfield()));
  }

  int userSize = userMsg.ByteSize();
  if(msg.numOfDataBytes < SPL_STANDARD_MESSAGE_DATA_SIZE)
  {
    msg.numOfDataBytes = (uint16_t) userMsg.ByteSize();
    userMsg.SerializeToArray(msg.data, userSize);
  }
  else
  {
    msg.numOfDataBytes = 0;
  }
}

// select one opponent:
// * I see him
// * the message about him is the oldest one
unsigned int TeamCommSender::selectSendOpp(const PlayersModel& playersModel,
                                           const FrameInfo& frameInfo,
                                           const TeamMessage& teamMessage)
{
  set<unsigned int> seenOppNum;
  for(vector<PlayersModel::Player>::const_iterator iter = playersModel.opponents.begin();
    iter != playersModel.opponents.end(); ++iter)
  {
    if ( frameInfo.getFrameNumber() == iter->frameInfoWhenWasSeen.getFrameNumber() )
    {
      seenOppNum.insert(iter->number);
    }
  }

  unsigned int selectedNum = 0;
  unsigned int earliest = frameInfo.getFrameNumber();
  for(set<unsigned int>::const_iterator iter = seenOppNum.begin();
    iter != seenOppNum.end(); ++iter)
  {
    map<unsigned int, unsigned int>::const_iterator opp = teamMessage.lastFrameNumberHearOpp.find(*iter);
    if ( opp == teamMessage.lastFrameNumberHearOpp.end() )
    {
      // no history of this opponent
      return *iter;
    }

    if ( opp->second < earliest )
    {
      selectedNum = *iter;
      earliest = opp->second;
    }
  }

  return selectedNum;
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
