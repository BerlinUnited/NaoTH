/**
* @file DataConversion.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* convert data to string or string to data
* or data to protobuf message
*/


#include "DataConversion.h"

namespace naoth
{

namespace DataConversion
{

void toMessage(const Vector2<double>& data, naothmessages::DoubleVector2& msg)
{
  msg.set_x(data.x);
  msg.set_y(data.y);
}

void fromMessage(const naothmessages::DoubleVector2& msg, Vector2<double>& data)
{
  data.x = msg.x();
  data.y = msg.y();
}

void toMessage(const Vector3<double>& data, naothmessages::DoubleVector3& msg)
{
  msg.set_x(data.x);
  msg.set_y(data.y);
  msg.set_z(data.z);
}

void fromMessage(const naothmessages::DoubleVector3& msg, Vector3<double>& data)
{
  data.x = msg.x();
  data.y = msg.y();
  data.z = msg.z();
}

void toMessage(const Pose2D& data, naothmessages::Pose2D& msg)
{
  toMessage(data.translation, *msg.mutable_translation() );
  msg.set_rotation(data.rotation);
}

void fromMessage(const naothmessages::Pose2D& msg, Pose2D& data)
{
  fromMessage(msg.translation(), data.translation);
  data.rotation = msg.rotation();
}

void toMessage(const Pose3D& data, naothmessages::Pose3D& msg)
{
  toMessage(data.translation, *msg.mutable_translation());
  for(int i=0; i<3; i++){
    toMessage(data.rotation[i], *msg.add_rotation());
  }
}

void fromMessage(const naothmessages::Pose3D& msg, Pose3D& data)
{
  fromMessage(msg.translation(), data.translation);
  for(int i=0; i<3; i++) {
    fromMessage(msg.rotation(i), data.rotation[i]);
  }
}

} // namespace DataConversion
} // namespace naoth
