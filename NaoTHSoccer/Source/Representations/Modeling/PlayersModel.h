/**
* @file PlayersModel.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of class PlayersModel
*/

#ifndef _PlayersModel_h_
#define _PlayersModel_h_

#include <vector>
#include "Tools/Math/Pose2D.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include <Tools/DataStructures/Printable.h>

class PlayersModel : public naoth::Printable
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
    Player() : number(0) {}
    
    unsigned int number; // 0 means unknow
    Pose2D pose;
    Pose2D globalPose;
    naoth::FrameInfo frameInfoWhenWasSeen;
  };

  virtual void print(std::ostream& stream) const 
  {
    stream << "Teammates:" << std::endl;
    for(const auto& player : teammates) {
      stream << "\ttime when seen: " << player.frameInfoWhenWasSeen.getTime() << std::endl;
      stream << "\tpose: " << player.pose << std::endl;
      stream << "\tglobalPose: " << player.globalPose << std::endl;
      stream << std::endl;
    }

    stream << "Opponents:" << std::endl;
    for(const auto& player : opponents) {
      stream << "\ttime when seen: " << player.frameInfoWhenWasSeen.getTime() << std::endl;
      stream << "\tpose: " << player.pose << std::endl;
      stream << "\tglobalPose: " << player.globalPose << std::endl;
      stream << std::endl;
    }
  }

  void reset() {
    opponents.clear();
    teammates.clear();
  }

  std::vector<Player> opponents;
  std::vector<Player> teammates;

  Player oppClosestToBall;
  Player ownClosestToBall;
};//end class PlayersModel

#endif// __PlayersModel_h_
