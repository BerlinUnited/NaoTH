
/**
 * @file TeamBallLocator.h
 *
 * Implementation of class TeamBallLocator
 */

#include "TeamBallLocator.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Math/Pose2D.h"

TeamBallLocator::TeamBallLocator()
{
  DEBUG_REQUEST_REGISTER("TeamBallLocator:draw_ball_on_field", "draw the team ball model on the field", false);
  DEBUG_REQUEST_REGISTER("TeamBallLocator:draw_ball_locally", "draw the team ball model on the local coordination", false);

  for (unsigned int i = 0; i < 4; i++) {
      ballInfoByNumber[i].isStriker = 0;
      ballInfoByNumber[i].timeWhenSeen = 1000000;
  }
}

void TeamBallLocator::execute()
{
  int num = 0;
  //for (int i = 0; i < MAX_NUM_PLAYERS; i++) {

  int idOfStriker = 0; //goalie if no one is striker

  msgData = getTeamMessage().data;

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=msgData.begin(); i != msgData.end(); ++i) {

    //if (getTeamMessage().messageReceived[i])
    //{
      //const naothmessages::TeamCommMessage& msg = getTeamMessage().message[i];
      const naothmessages::TeamCommMessage& msg = i->second.message;
      if ( msg.has_ballposition() )
      {

        Vector2<double> ballPos;
        ballPos.x = msg.ballposition().x();
        ballPos.y = msg.ballposition().y();
        Pose2D robotPos;
        robotPos.rotation = msg.positiononfield().rotation();
        robotPos.translation.x = msg.positiononfield().translation().x();
        robotPos.translation.y = msg.positiononfield().translation().y();
        ballPos = robotPos * ballPos;

        if ( i->second.frameInfo.getTimeInSeconds() > getTeamBallModel().time )
        {
          getTeamBallModel().positionOnField =  ballPos ;
          //getTeamBallModel().time = getTeamMessage().timeWhenReceived[i];
          ASSERT(i->second.frameInfo.getTimeInSeconds() >= 0);
          getTeamBallModel().time = (unsigned int)i->second.frameInfo.getTimeInSeconds();
          num = 1;
        }
        else if ( i->second.frameInfo.getTimeInSeconds() == getTeamBallModel().time )
        {
          getTeamBallModel().positionOnField += ballPos;
          num++;
        }

        int pn = i->first;;
        assert(pn > 0);
        assert(pn <= 4);
        ballInfoByNumber[pn-1].globalBall = ballPos;
        ballInfoByNumber[pn-1].timeWhenSeen = i->second.frameInfo.getTimeInSeconds();
        ballInfoByNumber[pn-1].isStriker = msg.wasstriker();

        if (msg.wasstriker() == true) {
            idOfStriker = pn-1;
        }
      }
    //}
  }

  if ( num > 0 )
  {
    getTeamBallModel().positionOnField /= num;
  }
  getTeamBallModel().position = getRobotPose() / getTeamBallModel().positionOnField;

  getTeamBallModel().goaliePositionOnField = ballInfoByNumber[0].globalBall;
  getTeamBallModel().goaliePosition = getRobotPose() / getTeamBallModel().goaliePositionOnField;
  getTeamBallModel().goalieTime = ballInfoByNumber[0].timeWhenSeen;

  getTeamBallModel().strikerPositionOnField = ballInfoByNumber[idOfStriker].globalBall;
  getTeamBallModel().strikerPosition = getRobotPose() / getTeamBallModel().strikerPositionOnField;
  getTeamBallModel().strikerTime = ballInfoByNumber[idOfStriker].timeWhenSeen;

  DEBUG_REQUEST("TeamBallLocator:draw_ball_on_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    CIRCLE(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, 45);
    TEXT_DRAWING(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, num);
    );

  DEBUG_REQUEST("TeamBallLocator:draw_ball_locally",
    FIELD_DRAWING_CONTEXT;
    PEN("FFFFFF", 30);
    CIRCLE(getTeamBallModel().position.x, getTeamBallModel().position.y, 45);
    );
}//end execute
