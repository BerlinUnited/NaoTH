/**
 * @file TeamCommEncoder.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * @brief encoder our team message (protobuf) to ANSCII, and decode from it
 * M essage may consist of 20 characters, which may be taken from the ASCII printing character
 * subset [0x20, 0x7E] except the white space character and the normal brackets ( and ).
 */

#include "TeamCommEncoder.h"
#include <Messages/Messages.pb.h>
#include "Tools/DataConversion.h"

using namespace std;
using namespace naoth;

TeamCommEncoder::TeamCommEncoder()
  :maxSize(22000, 15000) // TODO: this should be the same as field at least
{
  anglePiece = Math::fromDegrees(5.0);
}

// from protobuf to ASCII
string TeamCommEncoder::encode(const string& data)
{
  naothmessages::TeamCommMessage msg;
  msg.ParseFromString(data);

  string anscii;
  anscii += encoder.encode(static_cast<unsigned int>(msg.teamnumber()), 1);
  anscii += encoder.encode(static_cast<unsigned int>(msg.playernumber()), 1);
  anscii += encoder.encode(static_cast<unsigned int>(msg.wasstriker()), 1);

  // robot pose
  Pose2D pose;
  DataConversion::fromMessage(msg.positiononfield(), pose);
  pose.translation.x = Math::clamp(pose.translation.x, -maxSize.x, maxSize.x);
  pose.translation.y = Math::clamp(pose.translation.y, -maxSize.y, maxSize.y);
  anscii += encoder.encode(pose, maxSize, anglePiece, 4);

  // ball
  anscii += encoder.encode(static_cast<unsigned int> (msg.timesinceballwasseen())/60, 2); // 60 is vision frame rate

  Vector2d ballLocal(msg.ballposition().x(), msg.ballposition().y());
  Vector2d ballOnField = pose * ballLocal;
  ballOnField.x = Math::clamp(ballOnField.x, -maxSize.x, maxSize.x);
  ballOnField.y = Math::clamp(ballOnField.y, -maxSize.y, maxSize.y);
  anscii += encoder.encode(ballOnField, maxSize, 3);

  unsigned int isfalldown = msg.isfallendown()? 1:0;
  anscii += encoder.encode(isfalldown, 1);

  anscii += encoder.encode(static_cast<unsigned int>(msg.timetoball())/20, 2);

  // opponent
  if ( msg.has_opponent() )
  {
    unsigned int num = msg.opponent().number();
    Pose2D pose;
    DataConversion::fromMessage(msg.opponent().poseonfield(), pose);
    pose.translation.x = Math::clamp(pose.translation.x, -maxSize.x, maxSize.x);
    pose.translation.y = Math::clamp(pose.translation.y, -maxSize.y, maxSize.y);

    anscii += encoder.encode(num, 1);
    anscii += encoder.encode(pose, maxSize, anglePiece, 4);
  }

  ASSERT(anscii.size()==15 || anscii.size() == 20);

  return anscii;
}

/* The encode:
  0: team number
  1: player number
  2: was striker
  3-6: robot pose
  7-8: time ball was seen
  9-11: ball on field
  12: is fallen down
  13-14: time to ball
  15,16-19: opponent num, opponent pose
 */

// from ASCII to protobuf
string TeamCommEncoder::decode(const string& anscii)
{
  unsigned int messageSize = anscii.size();
  if ( messageSize == 15 || messageSize == 20 )
  {
    naothmessages::TeamCommMessage msg;
    msg.set_teamnumber( encoder.decodeUnsigned(anscii.substr(0, 1)) );
    msg.set_playernumber( encoder.decodeUnsigned(anscii.substr(1, 1)) );
    msg.set_ispenalized(false); // no penalize rule in SimSpark
    msg.set_bodyid(DataConversion::toStr(msg.playernumber())); // the same as player number
    msg.set_wasstriker(encoder.decodeUnsigned(anscii.substr(2, 1)) != 0);

    // robot pose
    Pose2D pose = encoder.decodePose2D(anscii.substr(3, 4), maxSize, anglePiece);
    DataConversion::toMessage(pose, *(msg.mutable_positiononfield()));

    // ball
    msg.set_timesinceballwasseen(encoder.decodeUnsigned(anscii.substr(7, 2))*60); // 60 is vision frame rate
    Vector2d ballOnField = encoder.decodeVector2D(anscii.substr(9, 3), maxSize);
    Vector2d ballLocal = pose.invert() * ballOnField;
    DataConversion::toMessage(ballLocal, *(msg.mutable_ballposition()));
    msg.set_isfallendown(encoder.decodeUnsigned(anscii.substr(12, 1)) > 0);

    msg.set_timetoball(encoder.decodeUnsigned(anscii.substr(13, 2))*20);

    if ( messageSize == 20 )
    {
      msg.mutable_opponent()->set_number(encoder.decodeUnsigned(anscii.substr(15, 1)));
      Pose2D pose = encoder.decodePose2D(anscii.substr(16,4), maxSize, anglePiece);
      DataConversion::toMessage(pose, *(msg.mutable_opponent()->mutable_poseonfield()));
    }

    return msg.SerializeAsString();
  }
  else
  {
    //THROW("msg ("<<anscii.size()<<" "<<") = "<< anscii);
    return "";
  }
}
