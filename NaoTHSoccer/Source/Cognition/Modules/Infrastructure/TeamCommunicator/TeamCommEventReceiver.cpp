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

    for (auto messagePart : message.details())
    {
      if (messagePart.Is<naothmessages::Ntp>())
      {
        naothmessages::Ntp ntp;
        messagePart.UnpackTo(&ntp);

        std::cout << message.playernum() << " == " << ntp.playernum() << " - " << ntp.sent() << " - " << ntp.received() << std::endl;
      }
    }
  }
}
