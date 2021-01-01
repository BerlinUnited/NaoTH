
/**
 * @file TeamBallLocatorMedian.h
 *
 * Implementation of class TeamBallLocatorMedian
 */

#include "TeamBallLocatorMedian.h"

TeamBallLocatorMedian::TeamBallLocatorMedian()
{
  DEBUG_REQUEST_REGISTER("TeamBallLocatorMedian:draw_ball_on_field", "draw the team ball model on the field", false);
  DEBUG_REQUEST_REGISTER("TeamBallLocatorMedian:draw_teamball_input", "draw all the balls uses for teamball", false);
  DEBUG_REQUEST_REGISTER("TeamBallLocatorMedian:draw_ownball", "draw all the balls uses for teamball", false);
  
  getDebugParameterList().add(&params);
}

TeamBallLocatorMedian::~TeamBallLocatorMedian()
{
    getDebugParameterList().remove(&params);
}

void TeamBallLocatorMedian::execute()
{
  // collect all balls seen by teammates and myself
  for (auto const& it: getTeamMessage().data) {
    if(it.first != getPlayerInfo().playerNumber)
    {
      const unsigned int& playerNumber = it.first;
      const TeamMessageData& msg = it.second;

      // check if the robot is able to play (inactive robots)
      if(!getTeamMessagePlayersState().isPlaying(playerNumber)) { continue; }
      
      // -1 means "ball never seen" and only "new" messages
      if(msg.ballAge >= 0 && lastMessages[playerNumber] < msg.frameInfo.getTime())
      {
        lastMessages[playerNumber] = msg.frameInfo.getTime();
        // global position of the ball and time last seen
        Vector2dTS ballPosTS;
        ballPosTS.vec = msg.pose * msg.ballPosition;
        ballPosTS.t = msg.frameInfo.getTime() - static_cast<int>(msg.ballAge);
        // collect balls
        ballPosHist.push_back(ballPosTS);

        // set time to the latest received message
        if (msg.frameInfo.getTime() > getTeamBallModel().time )
        {
          getTeamBallModel().time = msg.frameInfo.getTime();
        }
      }
    } else {
      // me myself and only "new" messages
      const unsigned int& playerNumber = it.first;
      const TeamMessageData& msg = it.second;
      
      // -1 means "ball never seen"
      if(msg.ballAge >= 0 && lastMessages[playerNumber] < msg.frameInfo.getTime())
      {
        lastMessages[playerNumber] = msg.frameInfo.getTime();
        // global position of the ball and time last seen
        Vector2dTS ballPosTS;
        ballPosTS.vec = msg.pose * msg.ballPosition;
        ballPosTS.t = msg.frameInfo.getTime() - static_cast<int>(msg.ballAge);
        // collect balls
        ownballPosHist.push_back(ballPosTS);
      }
    }
  }

  // find oldest balls and erase the ones, which ar older than 'maxTimeOffset'
  sort(ballPosHist.begin(), ballPosHist.end());
  std::vector<Vector2dTS>::iterator cutOff;
  // we are iterating through the sorted array from small (old) to high (new) times
  for(cutOff = ballPosHist.begin(); cutOff != ballPosHist.end(); cutOff++)
  {
    if (cutOff->t >= getTeamBallModel().time - params.maxTimeOffset)
    {
      break;
    }
  }

  ballPosHist.erase(ballPosHist.begin(), cutOff);
  
  // find oldest balls and erase the ones, which ar older than 'maxTimeOffset'
  sort(ownballPosHist.begin(), ownballPosHist.end());
  // we are iterating through the sorted array from small (old) to high (new) times
  for(cutOff = ownballPosHist.begin(); cutOff != ownballPosHist.end(); cutOff++)
  {
    if (cutOff->t >= getTeamBallModel().time - params.maxTimeOffset)
    {
      break;
    }
  }
  ownballPosHist.erase(ownballPosHist.begin(), cutOff);
  
  DEBUG_REQUEST("TeamBallLocatorMedian:draw_teamball_input",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 20);
    for(size_t i = 0; i < ballPosHist.size(); i++)
    {
      CIRCLE(ballPosHist[i].vec.x, ballPosHist[i].vec.y, 50);
    }
  );
  
  DEBUG_REQUEST("TeamBallLocatorMedian:draw_ownball",
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
  getTeamBallModel().valid = (getTeamBallModel().time + params.maxTimeValid >= getFrameInfo().getTime());

  DEBUG_REQUEST("TeamBallLocatorMedian:draw_ball_on_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    FILLOVAL(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, 50, 50);
    TEXT_DRAWING(getTeamBallModel().positionOnField.x+100, getTeamBallModel().positionOnField.y+100, ballPosHist.size());
    TEXT_DRAWING(getTeamBallModel().positionOnField.x+100, getTeamBallModel().positionOnField.y-100, getTeamBallModel().rmse);
  );
}
