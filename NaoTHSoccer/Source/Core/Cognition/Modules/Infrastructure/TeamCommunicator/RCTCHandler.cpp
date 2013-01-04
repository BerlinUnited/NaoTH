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

  // read
  for(unsigned int i = 0; i < getRCTCTeamMessageDataIn().data.size(); i++)
  {
    const rctc::Message& msg = getRCTCTeamMessageDataIn().data[i];

    if (  msg.teamID == getPlayerInfo().gameData.teamNumber && 
      !(msg.playerID == getPlayerInfo().gameData.playerNumber))
    {
      TeamMessage::Data& content = getTeamMessage().data[msg.playerID];
      content.frameInfo.setTime( getFrameInfo().getTime() );
      
      //HACK: convert to protobuf message
      content.message.set_teamnumber(msg.teamID);
      content.message.set_playernumber(msg.playerID);

      content.message.set_wasstriker(msg.mabID > 0);

      if(msg.ballPosX > 0 && msg.ballPosY > 0)
      {
        content.message.mutable_ballposition()->set_x(msg.ballPosX);
        content.message.mutable_ballposition()->set_y(msg.ballPosY);
        content.message.set_timesinceballwasseen(0);
      }
      else
      {
        // very long
        content.message.set_timesinceballwasseen(10000000);
      }

    }//end if
  }//end for


  // only send data in intervals of 500ms
  if((unsigned int)getFrameInfo().getTimeSince(lastSentTimestamp) > send_interval)
  {
    // send data
    createMessage(getRCTCTeamMessageDataOut().data);
    getRCTCTeamMessageDataOut().valid = true;

    lastSentTimestamp = getFrameInfo().getTime();
  }
  else
  {
    getRCTCTeamMessageDataOut().valid = false;
  }
}//end execute


void RCTCHandler::createMessage(rctc::Message& msg)
{
  msg.teamID = getPlayerInfo().gameData.teamNumber;
  msg.playerID = getPlayerInfo().gameData.playerNumber;
  msg.goalieID = 1;
  
  if(getPlayerInfo().isPlayingStriker)
  {
    msg.mabID = getPlayerInfo().gameData.playerNumber;
  }
  else
  {
    msg.mabID = 0;
  }

  if(getFrameInfo().getTimeSince(getBallModel().frameInfoWhenBallWasSeen.getTime()) < 3000)
  {
    msg.ballPosX = (int16_t)getBallModel().position.x;
    msg.ballPosY = (int16_t)getBallModel().position.y;
  }
  else
  {
    msg.ballPosX = 0;
    msg.ballPosY = 0;
  }
}//end createMessage
