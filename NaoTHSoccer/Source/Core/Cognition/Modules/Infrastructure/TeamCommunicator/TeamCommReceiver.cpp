#include "TeamCommReceiver.h"

#include <Tools/Debug/DebugRequest.h>

TeamCommReceiver::TeamCommReceiver()
{
  DEBUG_REQUEST_REGISTER("TeamCommReceiver:artificial_delay",
                         "Add an artificial delay to all team comm messages", false);
}

void TeamCommReceiver::execute()
{
  const naoth::TeamMessageDataIn& teamMessageData = getTeamMessageDataIn();

  bool usingDelayBuffer = false;
  DEBUG_REQUEST("TeamCommReceiver:artificial_delay",
    usingDelayBuffer = true;
  );

  for(vector<string>::const_iterator iter = teamMessageData.data.begin();
      iter != teamMessageData.data.end(); ++iter)
  {
    if(usingDelayBuffer)
    {
      delayBuffer.add(*iter);
    }
    else
    {
      handleMessage(*iter);
    }
  }

  // handle messages if buffer half full (so we get really old messages)
  if(usingDelayBuffer
     && delayBuffer.getNumberOfEntries() >= delayBuffer.getMaxEntries()/2)
  {
    // only handle a quarter of the messages
    for(int i=0; i < delayBuffer.getMaxEntries()/4; i++)
    {
      handleMessage(delayBuffer.first());
      delayBuffer.removeFirst();
    }
  }
}

void TeamCommReceiver::handleMessage(const string& data)
{
  naothmessages::TeamCommMessage msg;
  msg.ParseFromString(data);

  unsigned int num = msg.playernumber();
  unsigned int teamnum = msg.teamnumber();
  std::string bodyID = msg.bodyid();

  if ( teamnum == getPlayerInfo().gameData.teamNumber && !(num == getPlayerInfo().gameData.playerNumber && bodyID != getRobotInfo().bodyID))
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
