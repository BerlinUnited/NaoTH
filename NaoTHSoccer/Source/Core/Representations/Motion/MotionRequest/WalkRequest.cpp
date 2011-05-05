/**
* @file WalkRequest.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "WalkRequest.h"

void WalkRequest::toMessage(naothmessages::WalkRequest& message) const
{
  message.set_coordinate(coordinate);
  message.set_stopwithstand(stopWithStand);
  message.set_stable(stable);
  message.mutable_pose()->mutable_translation()->set_x(translation.x);
  message.mutable_pose()->mutable_translation()->set_y(translation.y);
  message.mutable_pose()->set_rotation(rotation);
}

void WalkRequest::fromMessage(const naothmessages::WalkRequest& message)
{
  coordinate = static_cast<Coordinate>(message.coordinate());
  stopWithStand = message.stopwithstand();
  stable = message.stable();
  translation.x = message.pose().translation().x();
  translation.y = message.pose().translation().y();
  rotation = message.pose().rotation();
}
