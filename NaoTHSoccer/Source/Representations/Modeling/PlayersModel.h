/**
* @file PlayersModel.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of class PlayersModel
*/

#ifndef __PlayersModel_h_
#define __PlayersModel_h_

#include <vector>
#include "Tools/Math/Pose2D.h"
#include "Representations/Infrastructure/FrameInfo.h"

class PlayersModel
{
public:
  PlayersModel() 
  {
    opponents.resize(12);
    teammates.resize(12);
  };

  virtual ~PlayersModel(){}

  class Player
  {
  public:
    Player():number(0){}
    
    unsigned int number; // 0 means unknow
    Pose2D pose;
    Pose2D globalPose;
    naoth::FrameInfo frameInfoWhenWasSeen;
  };

  std::vector<Player> opponents;
  std::vector<Player> teammates;
  Player oppClosestToBall;
  Player ownClosestToBall;
};//end class PlayersModel

#endif// __PlayersModel_h_
