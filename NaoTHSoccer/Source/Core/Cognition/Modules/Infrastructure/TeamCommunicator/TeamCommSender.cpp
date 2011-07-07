#include "TeamCommSender.h"
#include "PlatformInterface/Platform.h"
#include "Tools/DataConversion.h"

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

void TeamCommSender::createMessage(naothmessages::TeamCommMessage &msg)
{
  msg.set_playernumber(getPlayerInfo().gameData.playerNumber);
  msg.set_teamnumber(getPlayerInfo().gameData.teamNumber);
  msg.set_ispenalized(getPlayerInfo().gameData.gameState == GameData::penalized);
  msg.set_wasstriker(getPlayerInfo().isPlayingStriker);
  msg.set_bodyid(getRobotInfo().bodyID);

  msg.set_timesinceballwasseen(
    getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()));
  DataConversion::toMessage(getBallModel().position, *msg.mutable_ballposition());

  // robot pose
  DataConversion::toMessage(getRobotPose(), *(msg.mutable_positiononfield()));

  msg.set_isfallendown(getBodyState().fall_down_state != BodyState::upright
    || getMotionStatus().currentMotion == motion::stand_up_from_back
    || getMotionStatus().currentMotion == motion::stand_up_from_front);

  msg.set_timetoball(getSoccerStrategy().timeToBall);

  unsigned int oppNum = selectSendOpp();
  if (oppNum != 0)
  {
    addSendOppModel(oppNum, msg);
  }
}

// select one opponent:
// * I see him
// * the message about him is the oldest one
unsigned int TeamCommSender::selectSendOpp() const
{
  set<unsigned int> seenOppNum;
  for(vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().opponents.begin();
    iter != getPlayersModel().opponents.end(); ++iter)
  {
    if ( getFrameInfo().getFrameNumber() == iter->frameInfoWhenWasSeen.getFrameNumber() )
    {
      seenOppNum.insert(iter->number);
    }
  }

  unsigned int selectedNum = 0;
  unsigned int earliest = getFrameInfo().getFrameNumber();
  for(set<unsigned int>::const_iterator iter = seenOppNum.begin();
    iter != seenOppNum.end(); ++iter)
  {
    map<unsigned int, unsigned int>::const_iterator opp = getTeamMessage().lastFrameNumberHearOpp.find(*iter);
    if ( opp == getTeamMessage().lastFrameNumberHearOpp.end() )
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

void TeamCommSender::addSendOppModel(unsigned int oppNum, naothmessages::TeamCommMessage& msg) const
{
  for (vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().opponents.begin();
    iter != getPlayersModel().opponents.end(); ++iter)
  {
    if ( iter->number == oppNum )
    {
      msg.mutable_opponent()->set_number(oppNum);
      DataConversion::toMessage(iter->globalPose, *(msg.mutable_opponent()->mutable_poseonfield()));
      break;
    }
  }
}
