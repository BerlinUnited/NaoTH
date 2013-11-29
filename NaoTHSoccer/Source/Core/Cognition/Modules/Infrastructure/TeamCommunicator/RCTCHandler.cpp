#include "RCTCHandler.h"
#include "PlatformInterface/Platform.h"

RCTCHandler::RCTCHandler()
  : lastSentTimestamp(0),
    send_interval(500)
{
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  if ( config.hasKey("teamcomm", "send_interval") )
  {
    send_interval = config.getInt("teamcomm", "send_interval");
  }
}

void RCTCHandler::execute()
{

  // TODO
}//end execute


void RCTCHandler::createMessage(rctc::Message& msg)
{
  msg.teamID = static_cast<uint8_t>(getPlayerInfo().gameData.teamNumber);
  msg.playerID = static_cast<uint8_t>(getPlayerInfo().gameData.playerNumber);
  msg.goalieID = 1;
  
  if(getPlayerInfo().isPlayingStriker) {
    msg.mabID = static_cast<uint8_t>(getPlayerInfo().gameData.playerNumber);
  } else {
    msg.mabID = 0;
  }

  if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) < 3000)
  {
    msg.ballPosX = static_cast<uint16_t>(getBallModel().position.x);
    msg.ballPosY = static_cast<uint16_t>(getBallModel().position.y);
  }
  else
  {
    msg.ballPosX = 0;
    msg.ballPosY = 0;
  }
}//end createMessage
