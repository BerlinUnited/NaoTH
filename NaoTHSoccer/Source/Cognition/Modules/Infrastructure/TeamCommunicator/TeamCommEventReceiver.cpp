#include "TeamCommEventReceiver.h"
#include "Tools/NaoTime.h"

TeamCommEventReceiver::TeamCommEventReceiver()
{
  getDebugParameterList().add(&parameters);
}

TeamCommEventReceiver::~TeamCommEventReceiver()
{
  getDebugParameterList().remove(&parameters);
}

void TeamCommEventReceiver::execute()
{
  if(getWifiMode().wifiEnabled) {
    // process all incomming messages
    for (auto const &it : getTeamMessageDataIn().data) {
        handleMessage(it);
    }
  }

  /*
  // wait until we got a valid player number
  if (getPlayerInfo().playerNumber > 0 && 
      getTeamMessageData().frameInfo.getFrameNumber() + 1 == getFrameInfo().getFrameNumber() &&
      getTeamMessageData().playerNumber == getPlayerInfo().playerNumber &&
      getTeamMessageData().teamNumber == getPlayerInfo().teamNumber) 
  {
    // copying our own (old) message to the inbox
    getTeamMessage().data[getPlayerInfo().playerNumber] = getTeamMessageData();
  }

  // marking the begin of the outgoing message
  getTeamMessageData().frameInfo = getFrameInfo();
  // TODO: should we clear the old state?!? (see TeamMessageData::clear())

  PLOT("TeamCommEventReceiver:dropNoSplMessage", getTeamMessage().dropNoSplMessage);
  PLOT("TeamCommEventReceiver:dropNotOurTeam",   getTeamMessage().dropNotOurTeam);
  PLOT("TeamCommEventReceiver:dropNotParseable", getTeamMessage().dropNotParseable);
  PLOT("TeamCommEventReceiver:dropKeyFail",      getTeamMessage().dropKeyFail);
  PLOT("TeamCommEventReceiver:dropMonotonic",    getTeamMessage().dropMonotonic);
  */
}

void TeamCommEventReceiver::handleMessage(const std::string& data)
{
  std::cout << "handleMessage()" << std::endl;

  naothmessages::TeamMessageEvent message;
  if (message.ParseFromString(data)) 
  {
    // ignore own messages
    // if (message.playernum() == (int)getPlayerInfo().playerNumber) {
    //   return;
    // }

    auto ntpRequests = std::vector<TeamMessageNTP::Request>();

    // iterate through received data
    for (auto messagePart : message.details())
    {
      if (messagePart.Is<naothmessages::Ntp>())
      {
        naothmessages::Ntp ntp;
        messagePart.UnpackTo(&ntp);

        ntpRequests.push_back({(PlayerNumber)ntp.playernum(), ntp.sent(), ntp.received()});
      }
    }

    if (!ntpRequests.empty())
    {
      getTeamState().getPlayer(message.playernum()).ntpRequests = ntpRequests;
    }
  }
}
