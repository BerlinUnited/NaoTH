#include "TeamCommReceiver.h"
#include "TeamCommSender.h"

#include <Tools/Debug/DebugRequest.h>
#include <Messages/Representations.pb.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

TeamCommReceiver::TeamCommReceiver() : droppedMessages(0)
{
  DEBUG_REQUEST_REGISTER("TeamCommReceiver:artificial_delay",
                         "Add an artificial delay to all team comm messages", false );


  getDebugParameterList().add(&parameters);
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
    if(usingDelayBuffer) {
      delayBuffer.add(*iter);
    } else {
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
                              getSoccerStrategy(), getPlayersModel(),
                              getBatteryData(),
                              ownTeamData);
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

  PLOT("TeamCommReceiver:droppedMessages", droppedMessages);
}

TeamCommReceiver::~TeamCommReceiver()
{
  getDebugParameterList().remove(&parameters);
}

bool TeamCommReceiver::parseSPLStandardMessage(const std::string& data, SPLStandardMessage& msg) const
{
  if(data.size() > sizeof(SPLStandardMessage)) {
    //std::cerr << "wrong package size for teamcomm (allow own: " << allowOwn << ")"  << std::endl;
    // invalid message size
    return false;
  }

  memcpy(&msg, data.c_str(), sizeof(SPLStandardMessage));

  // furter sanity check for header and version
  if(msg.header[0] != 'S' ||
     msg.header[1] != 'P' ||
     msg.header[2] != 'L' ||
     msg.header[3] != ' ')
  {
    //std::cerr << "wrong header '" << spl.header  << "' for teamcomm (allow own: " << allowOwn << ")"  << std::endl;
    return false;
  }

  if(msg.version != SPL_STANDARD_MESSAGE_STRUCT_VERSION) {
    //std::cerr << "wrong version for teamcomm (allow own: " << allowOwn << ")"  << std::endl;
    return false;
  }

  return true;
}

bool TeamCommReceiver::parseTeamMessage(const SPLStandardMessage& spl, TeamMessage::Data& msg) const
{
  msg.frameInfo = getFrameInfo();

  msg.playerNum = spl.playerNum;

  if(spl.teamColor < GameData::numOfTeamColor) {
    msg.teamColor = (GameData::TeamColor) spl.teamColor;
  }

  msg.pose.translation.x = spl.pose[0];
  msg.pose.translation.y = spl.pose[1];
  msg.pose.rotation = spl.pose[2];

  msg.ballAge = spl.ballAge;

  msg.ballPosition.x = spl.ball[0];
  msg.ballPosition.y = spl.ball[1];

  msg.ballVelocity.x = spl.ballVel[0];
  msg.ballVelocity.y = spl.ballVel[1];

  msg.fallen = (spl.fallen == 1);

  // check if we can deserialize the user defined data
  if(spl.numOfDataBytes > 0 && spl.numOfDataBytes <= SPL_STANDARD_MESSAGE_DATA_SIZE)
  {
    naothmessages::BUUserTeamMessage userData;
    try
    {
      if(userData.ParseFromArray(spl.data, spl.numOfDataBytes))
      {
        msg.timestamp = userData.timestamp();
        msg.bodyID = userData.bodyid();
        msg.timeToBall = userData.timetoball();
        msg.wasStriker = userData.wasstriker();
        msg.isPenalized = userData.ispenalized();
        msg.batteryCharge = userData.batterycharge();
        msg.temperature = userData.temperature();
        msg.teamNumber = userData.teamnumber();
        msg.opponents = std::vector<TeamMessage::Opponent>(userData.opponents_size());
        for(size_t i=0; i < msg.opponents.size(); i++)
        {
          const naothmessages::Opponent& oppMsg = userData.opponents((int) i);
          TeamMessage::Opponent& opp = msg.opponents[i];
          opp.playerNum = oppMsg.playernum();
          DataConversion::fromMessage(oppMsg.poseonfield(), opp.poseOnField);
        }
      }
    }
    catch(...)
    {
      // well, this is not one of our messages, ignore

      // TODO: we might want to maintain a list of robots which send
      // non-compliant messages in order to avoid overhead when trying to parse it
      return false;
    }
  }

  return true;
}

void TeamCommReceiver::handleMessage(const std::string& data, bool allowOwn)
{
  SPLStandardMessage spl;
  if(!parseSPLStandardMessage(data, spl)) {
    return;
  }


  GameData::TeamColor teamColor = (GameData::TeamColor) spl.teamColor;

  if ( teamColor == getPlayerInfo().gameData.teamColor
       // ignore our own messages, we are adding it artficially later
       && (allowOwn || spl.playerNum != getPlayerInfo().gameData.playerNumber)
     )
  {
    TeamMessage::Data msg;
    if(parseTeamMessage(spl, msg)) {
      // copy new data to the blackboard if it's a message from our team
      if(!parameters.monotonicTimestampCheck || monotonicTimeStamp(msg)) {
        getTeamMessage().data[msg.playerNum] = msg;
      } else {
        droppedMessages++;
      }
    }
  }
}
