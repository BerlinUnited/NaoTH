/**
* @file MotionRequest.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "MotionRequest.h"

using namespace naoth;

/*
void MotionRequest::toDataStream(ostream& stream) const
{
  naothmessages::MotionRequest message;
  message.set_id(id);
  message.set_forced(forced);
  message.set_time(time);
  switch (id)
  {
  case MotionRequestID::walk:
    walkRequest.toMessage(*message.mutable_walkrequest());
    break;
  default:
    //TODO
    break;
  }
  google::protobuf::io::OstreamOutputStreamLite buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}//end toDataStream

void MotionRequest::fromDataStream(istream& stream)
{
  naothmessages::MotionRequest message;
  google::protobuf::io::IstreamInputStreamLite buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  id = static_cast<MotionRequestID::MotionID>(message.id());
  forced = message.forced();
  time = message.time();
  if ( message.has_walkrequest() )
  {
    walkRequest.fromMessage(*message.mutable_walkrequest());
  }
}//end fromDataStream
*/