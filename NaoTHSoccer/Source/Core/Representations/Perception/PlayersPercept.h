/**
* @file PlayersPercept.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of class PlayersPercept
*/

#ifndef __PlayersPercept_h_
#define __PlayersPercept_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/Debug/DebugDrawings.h"

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"

#include <vector>

class PlayersPercept : public naoth::Printable
{ 
public:
  PlayersPercept()
  {}

  class Player
  {
  public:

    Player() 
      : 
    poseValid(false),
    poseRotationValid(false), 
    teamColorIsValid(false), 
    number(0),
    numberIsValid(false),
    isStanding(false)
    {}

    Pose2D pose;
    bool poseValid;
    bool poseRotationValid;
    naoth::GameData::TeamColor teamColor;
    bool teamColorIsValid;
    unsigned int number;
    bool numberIsValid;
    bool isStanding;
    Vector2<double> angleTo;

    /** relative position of the seen foots on the field */
    std::vector<Vector2<double> > footPosition;

  }; //end class player

  /** list of seen players */
  std::list<Player> playersList;
 
  naoth::FrameInfo theFrameInfo;

  void addPlayer(const Player& player)
  {
    playersList.push_back(player);
  }//end add

  /* reset percept */
  void reset()
  {
    playersList.clear();
  }//end reset

  virtual void print(std::ostream& stream) const
  {
    for(std::list<Player>::const_iterator iter=playersList.begin(); iter!=playersList.end(); ++iter)
    {
      stream << naoth::GameData::teamColorToString(iter->teamColor)<<" : "<<iter->poseValid<<" : "<< iter->pose<<" : "<<iter->angleTo<<'\n';
    }
  }//end print

  virtual void draw() const
  {
    FIELD_DRAWING_CONTEXT;
    for(std::list<Player>::const_iterator iter=playersList.begin(); iter!=playersList.end(); ++iter)
    {
      if(iter->teamColorIsValid)
      {
        if(iter->teamColor == naoth::GameData::red)
        {
          PEN("FF0000", 20);
        }
        else
        {
          PEN("0000FF", 20);
        }
      }
      else
      {
        PEN("FF00FF", 20);
      }
      ROBOT(iter->pose.translation.x, iter->pose.translation.y, iter->pose.rotation);
    }
  }
};

class PlayersPerceptTop : public PlayersPercept
{
public:
  virtual ~PlayersPerceptTop() {}
};

namespace naoth
{
  template<>
  class Serializer<PlayersPercept>
  {
  public:
    static void serialize(const PlayersPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, PlayersPercept& representation);
  };

  template<>
  class Serializer<PlayersPerceptTop> : public Serializer<PlayersPercept>
  {
  };
}

#endif //__PlayersPercept_h_


