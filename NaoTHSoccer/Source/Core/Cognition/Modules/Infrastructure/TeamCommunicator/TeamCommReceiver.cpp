

#include "TeamCommReceiver.h"

void TeamCommReceiver::execute()
{
  const naoth::TeamMessageDataIn& teamMessageData = getTeamMessageDataIn();

  for(vector<string>::const_iterator iter = teamMessageData.data.begin();
      iter != teamMessageData.data.end(); ++iter)
  {
    handleMessage(*iter);
  }
}

void TeamCommReceiver::handleMessage(const string& data)
{
  naothmessages::TeamCommMessage msg;
  msg.ParseFromString(data);

  unsigned int num = msg.playernumber();
  unsigned int teamnum = msg.teamnumber();

  if ( teamnum == getPlayerInfo().gameData.teamNumber )
  {
    TeamMessage::Data& content = getTeamMessage().data[num];
    content.frameInfo.setTime( getFrameInfo().getTime() );
    content.message = msg;

    if ( msg.has_opponent() && msg.opponent().number() )
    {
      // mark this opponent is updated
      getTeamMessage().lastFrameNumberHearOpp[msg.opponent().number()] = getFrameInfo().getFrameNumber();
    }
  }
}
