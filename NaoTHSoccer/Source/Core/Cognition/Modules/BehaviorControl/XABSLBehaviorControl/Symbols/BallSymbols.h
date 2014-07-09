/**
* @file BallSymbols.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallSymbols
*/

#ifndef _BallSymbols_H_
#define _BallSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Pose2D.h"

// representations
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/SoccerStrategy.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(BallSymbols)
  REQUIRE(BallModel)
  REQUIRE(BallPercept)
  REQUIRE(BallPerceptTop)
  REQUIRE(FrameInfo)
  REQUIRE(RobotPose)
  REQUIRE(TeamBallModel)
  REQUIRE(FieldInfo)
  REQUIRE(KinematicChain)
  REQUIRE(SoccerStrategy)
END_DECLARE_MODULE(BallSymbols)

class BallSymbols: public BallSymbolsBase
{

public:
  BallSymbols() :
    ballPerceptSeen(false),
    ball_seen_filter(0.01, 0.1),
    ball_know_where_itis(false)
  {
    theInstance = this;
  }
  virtual ~BallSymbols(){}
  
  void registerSymbols(xabsl::Engine& engine);
  
  virtual void execute();

private:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("BallSymbols")
    {
      PARAMETER_REGISTER(ball_seen_filter.g0) = 0.03;
      PARAMETER_REGISTER(ball_seen_filter.g1) = 0.1;
      
      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    struct Ball_seen_filter {
      double g0;
      double g1;
    } ball_seen_filter;
    
    virtual ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }
  } parameters;

private:
  static BallSymbols* theInstance;

  // setter and getter
  static double getBallDistance();
  static double getBallAngle();
  static double getBallTimeSinceLastSeen();
  static double getBallTimeSeen();
  static double getTeamBallTimeSinceLastUpdate();
  static double getTeamBallGoalieTimeSinceLastUpdate();
  static double getTeamBallStrikerTimeSinceLastUpdate();

  // some local members
  Vector2d ballPositionField;

  Vector2d ballLeftFoot;
  Vector2d ballRightFoot;

  Vector2d ballPerceptPos;
  bool ballPerceptSeen;

  class AssymetricalBoolFilter
  {
  private:
    double g0;
    double g1;
    double state;

  public:
    AssymetricalBoolFilter(double g0, double g1) : g0(g0), g1(g1), state(0.0) 
    {}

    void setParameter(double g0, double g1) {
      this->g0 = g0; this->g1 = g1;
    }

    void update(bool v) {
      state += v ? g1*(1-state) : -g0*state;
    }

    double value() {
      return state;
    }
  };

  AssymetricalBoolFilter ball_seen_filter;
  bool ball_know_where_itis;

};//end class BallSymbols

#endif // _BallSymbols_H_
