#include "TeamCommEventSender.h"

TeamCommEventSender::TeamCommEventSender() : lastSentTimestamp(0)
{
  getDebugParameterList().add(&parameters);
}

TeamCommEventSender::~TeamCommEventSender()
{
  getDebugParameterList().remove(&parameters);
}

void TeamCommEventSender::execute()
{
    if(shouldSendMessage())
    {
        // create the message string of the known data
        getTeamMessageDataOut().data = createMessage();
        // remember the last sending time
        lastSentTimestamp = getFrameInfo().getTime();

        PLOT(std::string("TeamCommEventSender:message_size"), static_cast<double>(getTeamMessageDataOut().data.size()));
    } else {
        getTeamMessageDataOut().data.clear();
    }
}

inline bool TeamCommEventSender::shouldSendMessage() const
{
    return getWifiMode().wifiEnabled
      && (!parameters.strictMessageBudget || getGameData().ownTeam.messageBudget >= parameters.thresholdMessageBudget);
}

std::string TeamCommEventSender::createMessage() const
{
    naothmessages::TeamMessageEvent message;
    message.set_playernum(getPlayerInfo().playerNumber);

    // a dummy message for testing
    naothmessages::Ntp ntp;
    ntp.set_playernum(1);
    ntp.set_sent(getFrameInfo().getFrameNumber());
    ntp.set_received(getFrameInfo().getFrameNumber());
    message.add_details()->PackFrom(ntp);

    return message.SerializeAsString();
}

