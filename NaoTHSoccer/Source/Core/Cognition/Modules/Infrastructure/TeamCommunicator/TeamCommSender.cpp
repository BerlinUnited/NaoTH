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
  Vector2d ballOnField = getRobotPose() * getBallModel().position;
  DataConversion::toMessage(ballOnField, *msg.mutable_ballposition());

  // robot pose
  DataConversion::toMessage(getRobotPose(), *(msg.mutable_positiononfield()));

  // TODO: set falldown state in teamcomm message
  /*
  msg.set_isfallendown(theBodyState.fall_down_state != BodyState::upright
    || theMotionStatus.currentMotion == MotionRequestID::stand_up_from_back
    || theMotionStatus.currentMotion == MotionRequestID::stand_up_from_front);
  */

  // TODO: set ball and pose info in teamcomm message
  /*
  msg.set_timetoball(theSoccerStrategy.timeToBall);
*/
}

