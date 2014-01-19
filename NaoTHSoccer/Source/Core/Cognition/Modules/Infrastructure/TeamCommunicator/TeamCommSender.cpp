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
    TeamMessage::Data msg;
    createMessage(msg);

//TODO:    getTeamMessageDataOut().data = msg.SerializeAsString();
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

void TeamCommSender::createMessage(TeamMessage::Data &msg)
{
  fillMessage(getPlayerInfo(), getRobotInfo(), getFrameInfo(), getBallModel(),
              getRobotPose(), getBodyState(), getMotionStatus(), getSoccerStrategy(),
              getPlayersModel(), getTeamMessage(), true, msg);
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
