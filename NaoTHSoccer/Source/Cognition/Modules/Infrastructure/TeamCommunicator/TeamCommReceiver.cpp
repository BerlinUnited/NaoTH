#include "TeamCommReceiver.h"
#include "TeamCommSender.h"

#include <Tools/Debug/DebugRequest.h>
#include <Messages/Representations.pb.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

TeamCommReceiver::TeamCommReceiver()
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

  if(getWifiMode().wifiEnabled) {
    // process all incomming messages
    for (auto const &it : getTeamMessageDataIn().data) {
      if(usingDelayBuffer) {
        delayBuffer.add(it); // can be used for debugging
      } else {
        handleMessage(it);
      }
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

  PLOT("TeamCommReceiver:dropNoSplMessage", getTeamMessage().dropNoSplMessage);
  PLOT("TeamCommReceiver:dropNotOurTeam",   getTeamMessage().dropNotOurTeam);
  PLOT("TeamCommReceiver:dropNotParseable", getTeamMessage().dropNotParseable);
  PLOT("TeamCommReceiver:dropKeyFail",      getTeamMessage().dropKeyFail);
  PLOT("TeamCommReceiver:dropMonotonic",    getTeamMessage().dropMonotonic);
}

void TeamCommReceiver::handleMessage(const std::string& data)
{
  SPLStandardMessage spl;
  // only legal SPL messages
  if (!parseFromSplMessageString(data, spl)) {
    getTeamMessage().dropNoSplMessage++;
    return;
  }

  // only messages from own "team"
  if (spl.teamNum != (int)getPlayerInfo().teamNumber) {
    getTeamMessage().dropNotOurTeam++;
    return;
  }

  // ignore own messages
  if (spl.playerNum == (int)getPlayerInfo().playerNumber) {
    return;
  }

  // unpack the message and make sure the user part can be parsed
  TeamMessageData msg(getFrameInfo());
  // current timestamp as parsing time
  msg.timestampParsed = naoth::NaoTime::getSystemTimeInMilliSeconds();

  //accept own message
  if (msg.parseFromSplMessage(spl))
  {
    // make sure it's really our message
    if (!msg.isBerlinUnitedMessage()) {
      getTeamMessage().dropKeyFail++;
      return;
    }

    // make sure the time step is monotonically rising
    if (parameters.monotonicTimestampCheck && !monotonicTimeStamp(msg)) {
      getTeamMessage().dropMonotonic++;
      return;
    }
  }
  else if (parameters.acceptMixedTeamMessages && msg.isDoBerManMessage())
  {
    if (msg.ballAge >= 0) {
      // estimate time to ball for dortmund guys
      const double stepTime = 200; //ms
      const double speed = 50.0 / stepTime; // mm/ms
      const double turnSpeed = Math::fromDegrees(30) / stepTime;
      // sets the field
      msg.custom.timeToBall = static_cast<unsigned int>((msg.ballPosition.abs() / speed) + (fabs(msg.ballPosition.angle()) / turnSpeed));
    }
    // artificial time the other robot needs to process the message (cognition cycle)
    int processingTime = 20;
    // create a "pseudo" ntp request
    NtpRequest request;
    request.playerNumber = getPlayerInfo().playerNumber;
    // use the average RTT/latency to generate the "pseudo" request
    request.sent = naoth::NaoTime::getSystemTimeInMilliSeconds() - getTeamMessageTimeStatistics().globalRTT.getAverage() - processingTime;
    request.received = request.sent + getTeamMessageTimeStatistics().globalLatency.getAverage();
    msg.custom.timestamp = request.received + processingTime;
    msg.custom.ntpRequests.push_back(request);
  }
  else
  {
    getTeamMessage().dropNotParseable++;
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
