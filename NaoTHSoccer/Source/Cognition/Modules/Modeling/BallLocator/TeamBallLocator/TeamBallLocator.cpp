
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
}

void TeamBallLocator::execute()
{
  msgData = getTeamMessage().data;

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator i=msgData.begin(); i != msgData.end(); ++i) 
  {
    const TeamMessage::Data& msg = i->second;
    const unsigned int& playerNumber = i->first;
    
    // -1 means invalid ball
    if(msg.ballAge >= 0)
    {
      // collect messages
      Vector2dTS ballPosTS;
      ballPosTS.vec = msg.pose * msg.ballPosition;
      ballPosTS.t = msg.frameInfo.getTimeInSeconds(); // maybe +msg.BallAge ?
      ballPosHist.push_back(ballPosTS);

      // set time
      if (msg.frameInfo.getTimeInSeconds() > getTeamBallModel().time )
      {
        ASSERT(msg.frameInfo.getTimeInSeconds() >= 0);
        // are full seconds really the way to go?
        getTeamBallModel().time = (unsigned int)msg.frameInfo.getTimeInSeconds();
      }

      // goalie
      if(playerNumber == 1) 
      {
        getTeamBallModel().goaliePositionOnField = msg.pose * msg.ballPosition;
        getTeamBallModel().goaliePosition = getRobotPose() / getTeamBallModel().goaliePositionOnField;
        getTeamBallModel().goalieTime = msg.frameInfo.getTime();
      }

      // striker
      if (msg.wasStriker)
      {
        getTeamBallModel().strikerPositionOnField = msg.pose * msg.ballPosition;
        getTeamBallModel().strikerPosition = getRobotPose() / getTeamBallModel().strikerPositionOnField;
        getTeamBallModel().strikerTime = msg.frameInfo.getTime();
      }
    }
  }

  // find oldest messages and erase them
  double maxTimeOffset = 1.0;
  sort(ballPosHist.begin(), ballPosHist.end());
  std::vector<Vector2dTS>::iterator cutOff;
  // we are iterating through the sorted array from small (old) to high (new) times
  for(cutOff = ballPosHist.begin(); cutOff != ballPosHist.end(); cutOff++)
  {
    // take care: getTeamBallModel().time are unsigned int seconds
    if(cutOff->t >= getTeamBallModel().time - maxTimeOffset)
    {
      break;
    }
  }
  ballPosHist.erase(ballPosHist.begin(), cutOff);
  
  DEBUG_REQUEST("TeamBallLocator:draw_teamball_input",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    for(size_t i = 0; i < ballPosHist.size(); i++)
    {
      CIRCLE(ballPosHist[i].vec.x, ballPosHist[i].vec.y, 50);
    }
  );
  
  // median in x and y
//  std::vector<double> xHist(ballPosHist.size());
//  std::vector<double> yHist(ballPosHist.size());
//  for(size_t i = 0; i < ballPosHist.size(); i++)
//  {
//    xHist.push_back(ballPosHist[i].vec.x);
//    yHist.push_back(ballPosHist[i].vec.y);
//  }
//  sort(xHist.begin(), xHist.end());
//  sort(yHist.begin(), yHist.end());
//  Vector2d teamball;
//  teamball.x = xHist[xHist.size()/2];
//  teamball.y = yHist[yHist.size()/2];
 
  // canopy clustering
  CanopyClustering<SampleSet> canopyClustering(500, 10);
  SampleSet sampleSet(ballPosHist.size());
  for(size_t i = 0; i < ballPosHist.size(); i++)
  {
    sampleSet[i].translation = ballPosHist[i].vec;
  }
  canopyClustering.cluster(sampleSet);
  Vector2d teamball(canopyClustering.getLargestCluster().center());

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
