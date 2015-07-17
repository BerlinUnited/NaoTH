
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
  for(
    std::map<unsigned int, TeamMessage::Data>::const_iterator it=getTeamMessage().data.begin();
    it != getTeamMessage().data.end(); 
    it++) 
  {
    const unsigned int playerNumber = it->first;
    const TeamMessage::Data& msg = it->second;
    
    // -1 means invalid ball
    if(msg.ballAge >= 0 && lastMessages[playerNumber] < msg.frameInfo.getTime())
    {
      lastMessages[playerNumber] = msg.frameInfo.getTime();
      // collect messages
      Vector2dTS ballPosTS;
      ballPosTS.vec = msg.pose * msg.ballPosition;
      ballPosTS.t = msg.frameInfo.getTime() - msg.ballAge;
      ballPosHist.push_back(ballPosTS);

      // set time
      if (msg.frameInfo.getTime() > getTeamBallModel().time )
      {
        getTeamBallModel().time = msg.frameInfo.getTime();
      }
    }
  }
 
//  std::cout << "getTeamBallModel().time: " << getTeamBallModel().time << std::endl;
//  std::cout << "ballPosHist before erase: " << ballPosHist.size() << " " << std::endl;
//  for(size_t i = 0; i < ballPosHist.size(); i++)
//  {
//    std::cout << ballPosHist[i].t << " ";
//  }
//  std::cout << std::endl;

  // find oldest messages and erase them
  int maxTimeOffset = 1000;
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
  
//  std::cout << "ballPosHist after erase: " << ballPosHist.size() << " " << std::endl;
//  for(size_t i = 0; i < ballPosHist.size(); i++)
//  {
//    std::cout << ballPosHist[i].t << " ";
//  }
//  std::cout << std::endl;
  
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
//    for(size_t i = 0; i < ballPosHist.size(); i++)
//    {
//      std::cout << "(" << xHist[i] << "," << yHist[i] << ") ";
//    }
//    std::cout << std::endl;

    Vector2d teamball;
    teamball.x = xHist[xHist.size()/2];
    teamball.y = yHist[yHist.size()/2];
 
//    std::cout << "teamball: " << teamball.x << " " << teamball.y << std::endl;
    // write result and transform  
    getTeamBallModel().positionOnField = teamball;
    getTeamBallModel().position = getRobotPose() / getTeamBallModel().positionOnField;
  }
 
  // canopy clustering
//  CanopyClustering<SampleSet> canopyClustering(500, 10);
//  SampleSet sampleSet(ballPosHist.size());
//  for(size_t i = 0; i < ballPosHist.size(); i++)
//  {
//    sampleSet[i].translation = ballPosHist[i].vec;
//  }
//  canopyClustering.cluster(sampleSet);
//  Vector2d teamball(canopyClustering.getLargestCluster().center());
//
//  // write result and transform  
//  getTeamBallModel().positionOnField = teamball;
//  getTeamBallModel().position = getRobotPose() / getTeamBallModel().positionOnField;

  DEBUG_REQUEST("TeamBallLocator:draw_ball_on_field",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 20);
    FILLOVAL(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, 50, 50);
    TEXT_DRAWING(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, ballPosHist.size());
  );
}
