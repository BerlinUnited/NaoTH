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
  // get all incoming messages
  const naoth::TeamMessageDataIn& teamMessageData = getTeamMessageDataIn();

  bool usingDelayBuffer = false;
  DEBUG_REQUEST("TeamCommReceiver:artificial_delay",
    usingDelayBuffer = true;
  );

  for(auto const &it: teamMessageData.data) {
    if(usingDelayBuffer) {
      delayBuffer.add(it);
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
  if(getPlayerInfo().playerNumber > 0) {
      // copying our own (old) message to the inbox
      TeamMessageData msg(getTeamMessageData());
      getTeamMessage().data[msg.playerNumber] = msg;
  }

  // marking the begin of the outgoing message
  getTeamMessageData().frameInfo = getFrameInfo();
  // TODO: should we clear the old state?!? (see TeamMessageData::clear())

  PLOT("TeamCommReceiver:droppedMessages", droppedMessages);
}

void TeamCommReceiver::handleMessage(const std::string& data, bool allowOwn)
{
    // init message data container
    TeamMessageData msg;
    // parse only own team messages
    int parseTeam = getPlayerInfo().teamNumber;
    int parsePlayer = (allowOwn ? 1 : -1) * getPlayerInfo().playerNumber;
    // parsing ...

    if(msg.parseFromSplMessageString(data, parseTeam, parsePlayer)) {
        // copy new data to the blackboard if it's a message from our team
        if(!parameters.monotonicTimestampCheck || monotonicTimeStamp(msg)) {
            getTeamMessage().data[msg.playerNumber] = msg;
        } else {
            droppedMessages++;
        }
    }
}
