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

  // wait until we got a valid playnumber
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

  PLOT("TeamCommReceiver:droppedMessages", droppedMessages);
}

void TeamCommReceiver::handleMessage(const std::string& data)
{
  SPLStandardMessage spl;
  if (parseFromSplMessageString(data, spl))
  {
    // copy new data to the blackboard if it's a message from our team
    if (  spl.teamNum == (int)getPlayerInfo().teamNumber && // only messages from own "team"
         (spl.playerNum != (int)getPlayerInfo().playerNumber)) // accept own messages
    {
      TeamMessageData msg;
      msg.parseFromSplMessage(spl);
      if (!parameters.monotonicTimestampCheck || monotonicTimeStamp(msg)) {
        getTeamMessage().data[msg.playerNumber] = msg;
      }
      else {
        droppedMessages++;
      }
    }
  }
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

  return true;
}
