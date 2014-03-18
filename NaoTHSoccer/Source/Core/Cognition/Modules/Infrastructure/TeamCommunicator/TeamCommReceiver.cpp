#include "TeamCommReceiver.h"
#include "TeamCommSender.h"

#include <Tools/Debug/DebugRequest.h>
#include <Messages/Representations.pb.h>
#include <Representations/Modeling/SPLStandardMessage.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

TeamCommReceiver::TeamCommReceiver()
{
  DEBUG_REQUEST_REGISTER("TeamCommReceiver:artificial_delay",
                         "Add an artificial delay to all team comm messages", false );
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
     && delayBuffer.size() >= delayBuffer.getMaxEntries()/2)
  {
    // only handle a quarter of the messages
    for(int i=0; i < delayBuffer.getMaxEntries()/4; i++)
    {
      handleMessage(delayBuffer.first());
      delayBuffer.removeFirst();
    }
  }

  // add our own status as artifical message
  // (so we are not dependant on a lousy network)

  TeamMessage::Data ownTeamData;
  TeamCommSender::fillMessage(getPlayerInfo(), getRobotInfo(), getFrameInfo(),
                              getBallModel(), getRobotPose(), getBodyState(),
                              getSoccerStrategy(), getPlayersModel(), ownTeamData);
  // we don't have the right player number in the beginning, wait to send
  // one to ourself until we have a valid one
  if(ownTeamData.playerNum > 0)
  {
    SPLStandardMessage ownSPLMsg;
    TeamCommSender::convertToSPLMessage(ownTeamData, ownSPLMsg);

    std::string ownMsgData;
    ownMsgData.assign((char*) &ownSPLMsg, sizeof(SPLStandardMessage));
    handleMessage(ownMsgData, true);
  }
}

void TeamCommReceiver::handleMessage(const std::string& data, bool allowOwn)
{
  // TODO: implement handleMessage
  SPLStandardMessage spl;
  if(data.size() != sizeof(SPLStandardMessage))
  {
    //std::cerr << "wrong package size for teamcomm (allow own: " << allowOwn << ")"  << std::endl;
    // invalid message size
    return;
  }
  memcpy(&spl, data.c_str(), sizeof(SPLStandardMessage));
  // furter sanity check for header and version
  if(spl.header[0] != 'S' ||
     spl.header[1] != 'P' ||
     spl.header[2] != 'L' ||
     spl.header[3] != ' ')
  {
    //std::cerr << "wrong header '" << spl.header  << "' for teamcomm (allow own: " << allowOwn << ")"  << std::endl;
    return;
  }
  if(spl.version != SPL_STANDARD_MESSAGE_STRUCT_VERSION)
  {
    //std::cerr << "wrong version for teamcomm (allow own: " << allowOwn << ")"  << std::endl;
    return;
  }

  unsigned int num = spl.playerNum;
  GameData::TeamColor teamColor = (GameData::TeamColor) spl.team;

  if ( teamColor == getPlayerInfo().gameData.teamColor
       // ignore our own messages, we are adding it artficially later
       && (allowOwn || num != getPlayerInfo().gameData.playerNumber)
     )
  {
    TeamMessage::Data& data = getTeamMessage().data[num];
    data.frameInfo = getFrameInfo();

    data.playerNum = spl.playerNum;
    if(spl.team < GameData::numOfTeamColor)
    {
      data.teamColor = (GameData::TeamColor) spl.team;
    }

    data.pose.translation.x = spl.pose[0];
    data.pose.translation.y = spl.pose[1];
    data.pose.rotation = spl.pose[2];

    data.ballAge = spl.ballAge;

    data.ballPosition.x = spl.ball[0];
    data.ballPosition.y = spl.ball[1];

    data.ballVelocity.x = spl.ballVel[0];
    data.ballVelocity.y = spl.ballVel[1];

    data.fallen = spl.fallen;

    // TODO: use walkingTo and shootTo

    // check if we can deserialize the user defined data
    if(spl.numOfDataBytes > 0 && spl.numOfDataBytes < SPL_STANDARD_MESSAGE_DATA_SIZE)
    {
      naothmessages::BUUserTeamMessage userData;
      try
      {
        userData.ParseFromArray(spl.data, spl.numOfDataBytes);

        data.bodyID = userData.bodyid();
        data.timeToBall = userData.timetoball();
        data.wasStriker = userData.wasstriker();
        data.isPenalized = userData.ispenalized();
        data.opponents = std::vector<TeamMessage::Opponent>(userData.opponents_size());
        for(unsigned int i=0; i < data.opponents.size(); i++)
        {
          const naothmessages::Opponent& oppMsg = userData.opponents(i);
          TeamMessage::Opponent& opp = data.opponents[i];
          opp.playerNum = oppMsg.playernum();
          DataConversion::fromMessage(oppMsg.poseonfield(), opp.poseOnField);
        }
      }
      catch(...)
      {
        // well, this is not one of our messages, ignore

        // TODO: we might want to maintain a list of robots which send
        // non-compliant messages in order to avoid overhead when trying to parse it
      }
    }
  }
}
