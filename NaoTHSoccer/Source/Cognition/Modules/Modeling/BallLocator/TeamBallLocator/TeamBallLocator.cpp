
/**
 * @file TeamBallLocator.h
 *
 * Implementation of class TeamBallLocator
 */

#include "TeamBallLocator.h"

TeamBallLocator::TeamBallLocator()
{

  DEBUG_REQUEST_REGISTER("TeamBallLocator:draw_ball_on_field", "draw the team ball model on the field", false);
  DEBUG_REQUEST_REGISTER("TeamBallLocator:draw_ball_locally", "draw the team ball model on the local coordination", false);

}

void TeamBallLocator::execute()
{
  msgData = getTeamMessage().data;

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=msgData.begin(); i != msgData.end(); ++i) 
  {
    const TeamMessage::Data& msg = i->second;
    const unsigned int& playerNumber = i->first;
    
    // collect messages
    Vector2dTS ballPosTS;
    ballPosTS.vec = msg.pose * msg.ballPosition;
    ballPosTS.t = msg.frameInfo.getTimeInSeconds();
    ballPosHist.push_back(ballPosTS);

    // set time and legacy stuff
    if (msg.ballAge >= 0)
    {

      if (msg.frameInfo.getTimeInSeconds() > getTeamBallModel().time )
      {
        ASSERT(msg.frameInfo.getTimeInSeconds() >= 0);
        getTeamBallModel().time = (unsigned int)msg.frameInfo.getTimeInSeconds();
      }

      // goalie
      if(playerNumber == 1) 
      {
        getTeamBallModel().goaliePositionOnField = ballPos;
        getTeamBallModel().goaliePosition = getRobotPose() / getTeamBallModel().goaliePositionOnField;
        getTeamBallModel().goalieTime = msg.frameInfo.getTime();
      }

      // striker
      if (msg.wasStriker)
      {
        getTeamBallModel().strikerPositionOnField = ballPos;
        getTeamBallModel().strikerPosition = getRobotPose() / getTeamBallModel().strikerPositionOnField;
        getTeamBallModel().strikerTime = msg.frameInfo.getTime();
      }
    }
  }

  // find oldest messages and erase them
  double maxTimeOffset = 1.0;
  sort(ballPosHist.begin(), ballPosHist.end());
  std::vector<Vector2dTS>::iterator cutOff;
  size_t cutOff = 0;
  for(cutOff = ballPosHist.begin(); cutOff != ballPosHist.end(); cutOff++)
  {
    if(cutOff->t >= getTeamBallModel().time - maxTimeOffset)
    {
      break;
    }
  }
  ballPosHist.erase(ballPosHist.begin(), cutOff);
  
  // median in x and y
  std::vector<double> x(ballPosHist.size());
  std::vector<double> y(ballPosHist.size());
  for(size_t i = 0; i < ballPosHist.size(); i++)
  {
    x.push_back(ballPosHist.vec.x);
    y.push_back(ballPosHist.vec.y);
  }
  sort(x.begin(), x.end());
  sort(y.begin(), y.end());
  Vector2d teamball;
  teamball.x = x[x.size()/2];
  teamball.y = y[y.size()/2];
  
  // write result and transform  
  getTeamBallModel().positionOnField = teamball;
  getTeamBallModel().position = getRobotPose() / getTeamBallModel().positionOnField;

  DEBUG_REQUEST("TeamBallLocator:draw_ball_on_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    CIRCLE(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, 45);
    TEXT_DRAWING(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, ballPosHist.size());
  );
}
