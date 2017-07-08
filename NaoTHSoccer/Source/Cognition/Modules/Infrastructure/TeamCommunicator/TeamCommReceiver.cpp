#include "TeamCommReceiver.h"
#include "TeamCommSender.h"

#include <Tools/Debug/DebugRequest.h>
#include <Messages/Representations.pb.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

TeamCommReceiver::TeamCommReceiver():
    dropNoSplMessage(0),
    dropNotOurTeam(0),
    dropNotParseable(0),
    dropKeyFail(0),
    dropMonotonic(0)
{
  DEBUG_REQUEST_REGISTER("TeamCommReceiver:artificial_delay",
                         "Add an artificial delay to all team comm messages", false );

  getDebugParameterList().add(&parameters);
}

TeamCommReceiver::~TeamCommReceiver()
{
  getDebugParameterList().remove(&parameters);
}

void TeamCommReceiver::execute()
{
  bool usingDelayBuffer = false;
  DEBUG_REQUEST("TeamCommReceiver:artificial_delay",
    usingDelayBuffer = true;
  );

  // process all incomming messages
  for (auto const &it : getTeamMessageDataIn().data) {
    if(usingDelayBuffer) {
      delayBuffer.add(it); // can be used for debugging
    } else {
      handleMessage(it);
    }
  }

  // handle messages if buffer half full (so we get really old messages)
  if(usingDelayBuffer && delayBuffer.size() >= delayBuffer.getMaxEntries()/2)
  {
    // only handle a quarter of the messages
    for(int i=0; i < delayBuffer.getMaxEntries()/4; i++)
    {
      handleMessage(delayBuffer.first());
      delayBuffer.removeFirst();
    }
  }

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

  PLOT("TeamCommReceiver:dropNoSplMessage", dropNoSplMessage);
  PLOT("TeamCommReceiver:dropNotOurTeam",   dropNotOurTeam);
  PLOT("TeamCommReceiver:dropNotParseable", dropNotParseable);
  PLOT("TeamCommReceiver:dropKeyFail",      dropKeyFail);
  PLOT("TeamCommReceiver:dropMonotonic",    dropMonotonic);
}

void TeamCommReceiver::handleMessage(const std::string& data)
{
  SPLStandardMessage spl;
  // only legal SPL messages
  if (!parseFromSplMessageString(data, spl)) {
    dropNoSplMessage++;
    return;
  }

  // only messages from own "team"
  if (spl.teamNum != (int)getPlayerInfo().teamNumber) {
    dropNotOurTeam++;
    return;
  }

  // ignore own messages
  if (spl.playerNum == (int)getPlayerInfo().playerNumber) {
    return;
  }

  // unpack the message and make sure the user part can be parsed
  TeamMessageData msg(getFrameInfo());
  if (msg.parseFromSplMessage(spl))
  {
    // make sure it's really our message
    if (msg.custom.key != NAOTH_TEAMCOMM_MESAGE_KEY) {
      dropKeyFail++;
      return;
    }

    // make sure the time step is monotonically rising
    if (parameters.monotonicTimestampCheck && !monotonicTimeStamp(msg)) {
      dropMonotonic++;
      return;
    }
  }
  else if (parameters.acceptMixedTeamMessages)
  {
    // TODO: accept mixed team communication
    msg.custom.wantsToBeStriker = (msg.intention == 3);
    msg.custom.wasStriker = (msg.intention == 3);
    
    // estimate time to ball for dortmund guys
    const double stepTime = 200; //ms
    const double speed = 50.0 / stepTime; // mm/ms
    const double turnSpeed = Math::fromDegrees(30) / stepTime;

    if (msg.ballAge < 1000) {
      msg.custom.timeToBall = static_cast<unsigned int>((msg.ballPosition.abs() / speed) + (fabs(msg.ballPosition.angle()) / turnSpeed));
    }
  }
  else
  {
    dropNotParseable++;
    return;
  }

  // copy the message to the blackboard
  getTeamMessage().data[msg.playerNumber] = msg;
}

bool TeamCommReceiver::parseFromSplMessageString(const std::string &data, SPLStandardMessage& spl)
{
  // invalid message size
  if (data.size() > sizeof(SPLStandardMessage)) {
    std::cout << "invalid size" << std::endl;
    return false;
  }

  // convert back to struct
  memcpy(&spl, data.c_str(), data.size());

  // furter sanity check for header and version
  if (spl.header[0] != 'S' ||
      spl.header[1] != 'P' ||
      spl.header[2] != 'L' ||
      spl.header[3] != ' ' ||
      spl.version != SPL_STANDARD_MESSAGE_STRUCT_VERSION)
  {
    std::cout << "invalid header/version" << std::endl;
    return false;
  }

  // check if the size of the user part is meaningfull 
  if (spl.numOfDataBytes > SPL_STANDARD_MESSAGE_DATA_SIZE) {
    return false;
  }

  return true;
}
