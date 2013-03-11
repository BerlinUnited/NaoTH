#include "TeamCommSender.h"
#include "PlatformInterface/Platform.h"

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
    naothmessages::TeamCommMessage msg;
    createMessage(msg);

    getTeamMessageDataOut().data = msg.SerializeAsString();
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
                                   naothmessages::TeamCommMessage &msg)
{
  msg.set_playernumber(playerInfo.gameData.playerNumber);
  msg.set_teamnumber(playerInfo.gameData.teamNumber);
  msg.set_ispenalized(playerInfo.gameData.gameState == GameData::penalized);
  msg.set_wasstriker(playerInfo.isPlayingStriker);
  msg.set_bodyid(robotInfo.bodyID);

  msg.set_timesinceballwasseen(
    frameInfo.getTimeSince(ballModel.frameInfoWhenBallWasSeen.getTime()));
  DataConversion::toMessage(ballModel.position, *msg.mutable_ballposition());

  // robot pose
  DataConversion::toMessage(robotPose, *(msg.mutable_positiononfield()));

  msg.set_isfallendown(bodyState.fall_down_state != BodyState::upright
    || motionStatus.currentMotion == motion::stand_up_from_back
    || motionStatus.currentMotion == motion::stand_up_from_front);

  msg.set_timetoball(soccerStrategy.timeToBall);

  unsigned int oppNum = selectSendOpp(playersModel, frameInfo,
                                      teamMessage);
  if (oppNum != 0)
  {
    addSendOppModel(oppNum, playersModel, msg);
  }
}

void TeamCommSender::createMessage(naothmessages::TeamCommMessage &msg)
{
  fillMessage(getPlayerInfo(), getRobotInfo(), getFrameInfo(), getBallModel(),
              getRobotPose(), getBodyState(), getMotionStatus(), getSoccerStrategy(),
              getPlayersModel(), getTeamMessage(), msg);
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
                                     naothmessages::TeamCommMessage& msg)
{
  for (vector<PlayersModel::Player>::const_iterator iter = playersModel.opponents.begin();
    iter != playersModel.opponents.end(); ++iter)
  {
    if ( iter->number == oppNum )
    {
      msg.mutable_opponent()->set_number(oppNum);
      DataConversion::toMessage(iter->globalPose, *(msg.mutable_opponent()->mutable_poseonfield()));
      break;
    }
  }
}
