/**
* @file PlayersLocator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class PlayersLocator
*/

#ifndef __PlayersLocator_h_
#define __PlayersLocator_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/PlayersModel.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PlayersLocator)
  REQUIRE(FrameInfo)
  REQUIRE(RobotPose)
  REQUIRE(PlayersPercept)
  REQUIRE(BallModel)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamMessage)
  PROVIDE(PlayersModel)
END_DECLARE_MODULE(PlayersLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PlayersLocator : public PlayersLocatorBase
{
public:

  PlayersLocator();
  ~PlayersLocator(){}


  /** executes the module */
  void execute();

private:

  void updateByPlayersPercept();
  void updateByTeamMessage();
  PlayersModel::Player calculateClosestPlayerToBall(const std::vector<PlayersModel::Player>& team) const;

  // some simple interval arithmetics
  class Interval
  {
  public:
    double a; // min value
    double b; // max value
    Interval(double a, double b): a(std::min(a,b)), b(std::max(a,b)){}

    bool in(double d){ return d > a && d < b;}

    Interval& operator+(const Interval& other)
    {
      a += other.a;
      b += other.b;
      return *this;
    }

    Interval& operator-(const Interval& other)
    {
      a -= other.a;
      b -= other.b;
      return *this;
    }

    Interval& operator*(const Interval& other)
    {
      a *= other.a;
      b *= other.b;
      return *this;
    }

    Interval& operator*(double d)
    {
      a *= d;
      b *= d;
      return *this;
    }

    Interval& operator/(const Interval& other)
    {
      a /= other.a;
      b /= other.b;
      return *this;
    }
  };

  std::list<Interval> freeRegions;
};

#endif //__PlayersLocator_h_
