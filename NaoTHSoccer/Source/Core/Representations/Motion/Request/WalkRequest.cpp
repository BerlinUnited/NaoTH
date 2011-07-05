/**
* @file WalkRequest.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "WalkRequest.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<WalkRequest>::serialize(const WalkRequest& representation, std::ostream& stream)
{
  naothmessages::WalkRequest message;
  serialize(representation, &message);
  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<WalkRequest>::serialize(const WalkRequest& representation, naothmessages::WalkRequest* msg)
{
  msg->set_character(representation.character);
  msg->set_coordinate(representation.coordinate);
  DataConversion::toMessage(representation.target, *(msg->mutable_target()));

  // step control
  if ( representation.stepControl.stepID >= 0 )
  {
    naothmessages::StepControlRequest* stepControl = msg->mutable_stepcontrol();
    stepControl->set_stepid(representation.stepControl.stepID);
    stepControl->set_moveleftfoot(representation.stepControl.moveLeftFoot);
    stepControl->set_time(representation.stepControl.time);
    DataConversion::toMessage(representation.stepControl.target, *(stepControl->mutable_target()));
    stepControl->set_speeddirection(representation.stepControl.speedDirection);
  }
}

void Serializer<WalkRequest>::deserialize(std::istream& stream, WalkRequest& representation)
{
  naothmessages::WalkRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  deserialize(&message, representation);
}

void Serializer<WalkRequest>::deserialize(const naothmessages::WalkRequest* msg, WalkRequest& representation)
{
  representation.character = msg->character();
  representation.coordinate = static_cast<WalkRequest::Coordinate>(msg->coordinate());
  DataConversion::fromMessage(msg->target(), representation.target);

  // step control
  if ( msg->has_stepcontrol() )
  {
    const naothmessages::StepControlRequest& stepControl = msg->stepcontrol();
    representation.stepControl.stepID = stepControl.stepid();
    representation.stepControl.moveLeftFoot = stepControl.moveleftfoot();
    representation.stepControl.time = stepControl.time();
    DataConversion::fromMessage(stepControl.target(), representation.stepControl.target);
    representation.stepControl.speedDirection = stepControl.speeddirection();
  }
  else
  {
    representation.stepControl.stepID = -1;
  }
}
