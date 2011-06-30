
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
}

void TeamBallLocator::execute()
{
  int num = 0;
  //for (int i = 0; i < MAX_NUM_PLAYERS; i++) {

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
      }
    //}
  }

  if ( num > 0 )
  {
    getTeamBallModel().positionOnField /= num;
  }
  getTeamBallModel().position = getRobotPose() / getTeamBallModel().positionOnField;

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
