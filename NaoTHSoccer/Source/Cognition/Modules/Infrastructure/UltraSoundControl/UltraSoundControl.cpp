/**
 * @file UltraSoundControl.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#include "UltraSoundControl.h"

UltraSoundControl::UltraSoundControl()
{
  DEBUG_REQUEST_REGISTER("UltraSoundControl:mode:CAPTURE_LEFT", "left Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundControl:mode:CAPTURE_RIGHT", "right Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundControl:mode:CAPTURE_BOTH", "left and right capture", false);
  DEBUG_REQUEST_REGISTER("UltraSoundControl:mode:TRANSMIT_LEFT", "left Transmitter", false);
  DEBUG_REQUEST_REGISTER("UltraSoundControl:mode:TRANSMIT_RIGHT", "right Transmitter", false);
  DEBUG_REQUEST_REGISTER("UltraSoundControl:mode:TRANSMIT_BOTH", "left and right transmitter", false);
  DEBUG_REQUEST_REGISTER("UltraSoundControl:mode:PERIODIC", "send US periodically", false);
}

void UltraSoundControl::execute()
{
  // default state
  int transmitter = UltraSoundSendData::TRANSMIT_LEFT;
  int receiver = UltraSoundSendData::CAPTURE_BOTH;

  DEBUG_REQUEST("UltraSoundControl:mode:CAPTURE_LEFT",
    receiver = UltraSoundSendData::CAPTURE_LEFT;
  );
  DEBUG_REQUEST("UltraSoundControl:mode:CAPTURE_RIGHT",
    receiver = UltraSoundSendData::CAPTURE_RIGHT;
  );
  DEBUG_REQUEST("UltraSoundControl:mode:CAPTURE_BOTH",
    receiver = UltraSoundSendData::CAPTURE_BOTH;
  );
  DEBUG_REQUEST("UltraSoundControl:mode:TRANSMIT_LEFT",
    transmitter = UltraSoundSendData::TRANSMIT_LEFT;
  );
  DEBUG_REQUEST("UltraSoundControl:mode:TRANSMIT_RIGHT",
    transmitter = UltraSoundSendData::TRANSMIT_RIGHT;
  );
  DEBUG_REQUEST("UltraSoundControl:mode:TRANSMIT_BOTH",
    transmitter = UltraSoundSendData::TRANSMIT_BOTH;
  );

  int mode = transmitter + receiver;

  DEBUG_REQUEST("UltraSoundControl:mode:PERIODIC",
    mode += UltraSoundSendData::PERIODIC;
  );

  // send new receive command every 100 ms
  if(getFrameInfo().getTimeSince(lastSend.getTime()) > 100) {
    getUltraSoundSendData().setMode(mode);
    lastSend = getFrameInfo();
  } else {
    getUltraSoundSendData().setMode(-1);
  }
}//end execute