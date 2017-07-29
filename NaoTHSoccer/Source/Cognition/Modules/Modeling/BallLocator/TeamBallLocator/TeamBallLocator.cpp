
/**
 * @file TeamBallLocator.h
 *
 * Implementation of class TeamBallLocator
 */

#include "TeamBallLocator.h"

TeamBallLocator::TeamBallLocator()
{
  DEBUG_REQUEST_REGISTER("TeamBallLocator:draw_ball_on_field", "draw the team ball model on the field", false);
  DEBUG_REQUEST_REGISTER("TeamBallLocator:draw_teamball_input", "draw all the balls uses for teamball", false);
  DEBUG_REQUEST_REGISTER("TeamBallLocator:draw_ownball", "draw all the balls uses for teamball", false);
  
  getDebugParameterList().add(&theParameters);
}

void TeamBallLocator::execute()
{
  for (auto const& it: getTeamMessage().data) {
    if(it.first != getPlayerInfo().playerNumber)
    {
      const unsigned int& playerNumber = it.first;
      const TeamMessageData& msg = it.second;

      // check if the robot is able to play
      bool isRobotInactive = msg.fallen || msg.custom.isPenalized;

      // TODO: check for "DEAD" robots!? (see CleanRoleDecision)

      // inactive robots
      if(isRobotInactive) { continue; }
      
      // -1 means invalid ball
      if(msg.ballAge >= 0 && lastMessages[playerNumber] < msg.frameInfo.getTime())
      {
        lastMessages[playerNumber] = msg.frameInfo.getTime();
        // collect messages
        Vector2dTS ballPosTS;
        ballPosTS.vec = msg.pose * msg.ballPosition;
        ballPosTS.t = msg.frameInfo.getTime() - static_cast<int>(msg.ballAge);
        ballPosHist.push_back(ballPosTS);

        // set time to the latest received message
        if (msg.frameInfo.getTime() > getTeamBallModel().time )
        {
          getTeamBallModel().time = msg.frameInfo.getTime();
        }
      }
    } else
    {
      const unsigned int& playerNumber = it.first;
      const TeamMessageData& msg = it.second;
      
      // -1 means invalid ball
      if(msg.ballAge >= 0 && lastMessages[playerNumber] < msg.frameInfo.getTime())
      {
        lastMessages[playerNumber] = msg.frameInfo.getTime();
        // collect messages
        Vector2dTS ballPosTS;
        ballPosTS.vec = msg.pose * msg.ballPosition;
        ballPosTS.t = msg.frameInfo.getTime() - static_cast<int>(msg.ballAge);
        ownballPosHist.push_back(ballPosTS);
      }
    }
  }

  // find oldest messages and erase them
  sort(ballPosHist.begin(), ballPosHist.end());
  std::vector<Vector2dTS>::iterator cutOff;
  // we are iterating through the sorted array from small (old) to high (new) times
  for(cutOff = ballPosHist.begin(); cutOff != ballPosHist.end(); cutOff++)
  {
    if(cutOff->t >= getTeamBallModel().time - theParameters.maxTimeOffset)
    {
      break;
    }
  }

  ballPosHist.erase(ballPosHist.begin(), cutOff);
  
  // find oldest messages and erase them
  sort(ownballPosHist.begin(), ownballPosHist.end());
  // we are iterating through the sorted array from small (old) to high (new) times
  for(cutOff = ownballPosHist.begin(); cutOff != ownballPosHist.end(); cutOff++)
  {
    if(cutOff->t >= getTeamBallModel().time - theParameters.maxTimeOffset)
    {
      break;
    }
  }
  ownballPosHist.erase(ownballPosHist.begin(), cutOff);
  
  DEBUG_REQUEST("TeamBallLocator:draw_teamball_input",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 20);
    for(size_t i = 0; i < ballPosHist.size(); i++)
    {
      CIRCLE(ballPosHist[i].vec.x, ballPosHist[i].vec.y, 50);
    }
  );
  
  DEBUG_REQUEST("TeamBallLocator:draw_ownball",
    FIELD_DRAWING_CONTEXT;
    PEN("66FFFF", 20);
    for(size_t i = 0; i < ownballPosHist.size(); i++)
    {
      CIRCLE(ownballPosHist[i].vec.x, ownballPosHist[i].vec.y, 50);
    }
  );
  
  if(ballPosHist.size() > 0)
  {
    // median in x and y
    std::vector<double> xHist(ballPosHist.size());
    std::vector<double> yHist(ballPosHist.size());
    for(size_t i = 0; i < ballPosHist.size(); i++)
    {
      xHist[i] = ballPosHist[i].vec.x;
      yHist[i] = ballPosHist[i].vec.y;
    }
    sort(xHist.begin(), xHist.end());
    sort(yHist.begin(), yHist.end());
    Vector2d teamball;
    teamball.x = xHist[xHist.size()/2];
    teamball.y = yHist[yHist.size()/2];
    
    // root mean squared error
    getTeamBallModel().rmse = 0.0;
    for(size_t i = 0; i < ballPosHist.size(); i++)
    {
      getTeamBallModel().rmse += (teamball.x - xHist[i])*(teamball.x - xHist[i]) + (teamball.y - yHist[i])*(teamball.y - yHist[i]);
    }
    getTeamBallModel().rmse = getTeamBallModel().rmse / static_cast<double>(ballPosHist.size());
    getTeamBallModel().rmse = sqrt(getTeamBallModel().rmse);

    // write result and transform
    getTeamBallModel().positionOnField = teamball;
    getTeamBallModel().position = getRobotPose() / getTeamBallModel().positionOnField;
  }

  // set validity of the teamball -> invalidate after a certain time
  getTeamBallModel().valid = (getTeamBallModel().time + theParameters.maxTimeValid >= getFrameInfo().getTime());

  DEBUG_REQUEST("TeamBallLocator:draw_ball_on_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    FILLOVAL(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, 50, 50);
    TEXT_DRAWING(getTeamBallModel().positionOnField.x+100, getTeamBallModel().positionOnField.y+100, ballPosHist.size());
    TEXT_DRAWING(getTeamBallModel().positionOnField.x+100, getTeamBallModel().positionOnField.y-100, getTeamBallModel().rmse);
  );
}
