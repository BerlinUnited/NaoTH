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

  // simulate own message reception
  auto& myself = getTeamState().getPlayer(getPlayerInfo().playerNumber);
  myself.messageFrameInfo = getFrameInfo();
  myself.messageParsed    = naoth::NaoTime::getSystemTimeInMilliSeconds();

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
  */
}

void TeamCommEventReceiver::handleMessage(const std::string& data)
{
  naothmessages::TeamState::Player message;
  if (message.ParseFromString(data))
  {
    // skip my "own" or the message of another player with the same number
    if (message.number() == getPlayerInfo().playerNumber) {
        return;
    }

    auto& state = getTeamState().getPlayer(message.number());
    state.messageFrameInfo = getFrameInfo();
    state.messageParsed    = naoth::NaoTime::getSystemTimeInMilliSeconds();

    if (message.ntprequest().size() > 0) {
        auto ntpRequests = std::vector<TeamMessageNTP::Request>(message.ntprequest_size());
        for(int i=0; i < message.ntprequest_size(); i++)
        {
            auto& request = message.ntprequest((int)i);
            auto& syncingPlayer = ntpRequests[i];
            syncingPlayer.playerNumber = request.playernum();
            syncingPlayer.sent = request.sent();
            syncingPlayer.received = request.received();
        }
        state.ntpRequests = ntpRequests;
    }

    if (message.has_robotstate())
    {
        state.state = (PlayerInfo::RobotState) message.robotstate();
    }

    if (message.has_fallen())
    {
        state.fallen = message.fallen();
    }

    if (message.has_readytowalk())
    {
        state.readyToWalk = message.readytowalk();
    }

    if (message.has_pose())
    {
        state.pose = {
          message.pose().rotation(),
          message.pose().translation().x(),
          message.pose().translation().y()
        };
    }

    if (message.has_ballage())
    {
        state.ballAge = message.ballage();
    }

    if (message.has_ballposition())
    {
        state.ballPosition = {
          message.ballposition().x(),
          message.ballposition().y()
        };
    }

    if (message.has_timetoball())
    {
        state.timeToBall = message.timetoball();
    }

    if (message.has_wantstobestriker())
    {
        state.wantsToBeStriker = message.wantstobestriker();
    }

    if (message.has_wasstriker())
    {
      state.wasStriker = message.wasstriker();
    }

    if (message.has_robotrole())
    { 
      state.robotRole = {
        (Roles::Static)message.robotrole().role_static(),
        (Roles::Dynamic)message.robotrole().role_dynamic()
      };
    }
  }
}
