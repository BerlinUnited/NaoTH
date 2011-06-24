/**
 * @file SimSparkTeamComm.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "SimSparkTeamComm.h"

SimSparkTeamComm::SimSparkTeamComm()
{
}

SimSparkTeamComm::~SimSparkTeamComm()
{
}

void SimSparkTeamComm::execute()
{
  readMessage();
  sendMessage();
}

void SimSparkTeamComm::readMessage()
{
  unsigned int time = getFrameInfo().time;
  for(vector<string>::const_iterator i=getTeamMessageData().data.begin();
      i!=getTeamMessageData().data.end(); ++i)
  {
    unsigned int teamNumber = encoder.decodeUnsigned(i->substr(0, 1));
    if ( teamNumber == getPlayerInfo().gameData.teamNumber )
    {
      unsigned int playerNumber = encoder.decodeUnsigned(i->substr(1, 1));
      TeamMessage::Data& data = getTeamMessage().data[playerNumber];

      data.frameInfo.time = time;
      data.frameInfo.frameNumber++;

      naothmessages::TeamCommMessage& message = data.message;
      message.set_ispenalized(false); // no penalize rule in SimSpark
      message.set_teamnumber(teamNumber);
      message.set_playernumber(playerNumber);
      message.set_ispenalized(encoder.decodeUnsigned(i->substr(2, 1)) != 0);
    }
  }
}

void SimSparkTeamComm::sendMessage()
{
  string& data = getRobotMessageData().data;
  data.clear();

  data += encoder.encode(static_cast<unsigned int>(getPlayerInfo().gameData.teamNumber), 1);
  data += encoder.encode(static_cast<unsigned int>(getPlayerInfo().gameData.playerNumber), 1);
  data += encoder.encode(static_cast<unsigned int>(getPlayerInfo().isPlayingStriker), 1);
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
}
