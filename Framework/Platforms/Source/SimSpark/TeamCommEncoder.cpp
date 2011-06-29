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

using namespace std;

TeamCommEncoder::TeamCommEncoder()
{
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

        /*
          s += encoder.encode(static_cast<unsigned int>(msg.isfallendown()), 1);

          // ball
          s += encoder.encode(static_cast<unsigned int> (msg.timesinceballwasseen())/20, 2);
          s += encoder.encode(static_cast<unsigned int> (msg.timetoball())/20, 2);
          if (msg.has_ballposition())
          {
            Vector2<double> ballPos(msg.ballposition().x(), msg.ballposition().y());
            ballPos.x = Math::clamp(ballPos.x, -fieldSize.x, fieldSize.x);
            ballPos.y = Math::clamp(ballPos.y, -fieldSize.y, fieldSize.y);
            s += encoder.encode(ballPos, fieldSize, 3);
          }

          if (msg.has_positiononfield())
          {
            Pose2D pose(msg.positiononfield().rotation(),
              msg.positiononfield().translation().x(),
              msg.positiononfield().translation().y());
            pose.translation.x = Math::clamp(pose.translation.x, -fieldSize.x, fieldSize.x);
            pose.translation.y = Math::clamp(pose.translation.y, -fieldSize.y, fieldSize.y);
            s += encoder.encode(pose, fieldSize, anglePiece, 4);
          }

          if ( msg.has_opponent() )
          {
            unsigned int num = msg.opponent().number();
            Pose2D pose(msg.opponent().poseonfield().rotation(),
              msg.opponent().poseonfield().translation().x(),
              msg.opponent().poseonfield().translation().y());
            pose.translation.x = Math::clamp(pose.translation.x, -fieldSize.x, fieldSize.x);
            pose.translation.y = Math::clamp(pose.translation.y, -fieldSize.y, fieldSize.y);

            s += encoder.encode(num, 1);
            s += encoder.encode(pose, fieldSize, anglePiece, 4);
          }*/

  return anscii;
}

// from ASCII to protobuf
string TeamCommEncoder::decode(const string& anscii)
{
  naothmessages::TeamCommMessage msg;
  msg.set_teamnumber( encoder.decodeUnsigned(anscii.substr(0, 1)) );
  msg.set_playernumber( encoder.decodeUnsigned(anscii.substr(1, 1)) );
  msg.set_ispenalized(false); // no penalize rule in SimSpark
  msg.set_wasstriker(encoder.decodeUnsigned(anscii.substr(2, 1)) != 0);

  string data;
  msg.SerializeToString(&data);
  return data;
}
