/**
* @file DebugPlayersProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* provides soccer strategy
*/

#include "DebugPlayersProvider.h"

using namespace std;

DebugPlayersProvider::DebugPlayersProvider()
{
}

void DebugPlayersProvider::execute()
{
  getPlayersModel().reset();
  
  for(const auto& trackable: getOptiTrackData().trackables) 
  {
    // starts with "nao" => own player
    if(trackable.first.compare(0, 3, "nao") == 0) 
    {
      PlayersModel::Player player;
      trackableToPlayer(trackable.second, player);
      getPlayersModel().teammates.push_back(player);
    } 
    // starts with "opp" => opponent player
    else if(trackable.first.compare(0, 3, "opp") == 0)
    {
      PlayersModel::Player player;
      trackableToPlayer(trackable.second, player);
      getPlayersModel().opponents.push_back(player);
    }
  }
  
  
  
}//end execute


void DebugPlayersProvider::trackableToPlayer(const Pose3D& pose, PlayersModel::Player& player) const {
  player.globalPose = Pose2D(-pose.rotation.getYAngle(), pose.translation.x, pose.translation.y);
  player.pose = getRobotPose().invert()*player.globalPose;
  player.frameInfoWhenWasSeen = getFrameInfo();
}
