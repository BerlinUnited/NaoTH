/**
* @file Simulation.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class Simulation
*/

#ifndef _Simulation_H
#define _Simulation_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/CompassDirection.h"
#include "Representations/Modeling/KickActionModel.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/ObstacleModel.h"

//Tools
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Probabilistics.h>
#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"
#include <Representations/Debug/Stopwatch.h>

// Debug
#include <Tools/Debug/DebugRequest.h>

BEGIN_DECLARE_MODULE(Simulation)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  PROVIDE(StopwatchManager)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(PlayerInfo)
  REQUIRE(ObstacleModel)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(CompassDirection)
  REQUIRE(MotionStatus)

  PROVIDE(KickActionModel)
END_DECLARE_MODULE(Simulation)

class Simulation: public SimulationBase
{
public:
  Simulation();
  ~Simulation();

  virtual void execute();

  class ActionParams
  {
    public:
      ActionParams():
        speed(0.0),
        speed_std(0.0),
        angle(0.0),
        angle_std(0.0)
      {}
    public:
      double speed;
      double speed_std;
      double angle;
      double angle_std;
  };

   /** parameters for the module */
  class Parameters: public ParameterList
  {
  public:
	
    Parameters() : ParameterList("PotentialActionParameters")
    {
      PARAMETER_REGISTER(sidekick_right.speed) = 750;
      PARAMETER_REGISTER(sidekick_right.speed_std) = 150;
      PARAMETER_REGISTER(sidekick_right.angle) = Math::fromDegrees(-85);
      PARAMETER_REGISTER(sidekick_right.angle_std) = Math::fromDegrees(15);
      PARAMETER_REGISTER(sidekick_left.speed) = 750;
      PARAMETER_REGISTER(sidekick_left.speed_std) = 150;
      PARAMETER_REGISTER(sidekick_left.angle) = Math::fromDegrees(85);
      PARAMETER_REGISTER(sidekick_left.angle_std) = Math::fromDegrees(15);
      PARAMETER_REGISTER(kick_short.speed) = 780;
      PARAMETER_REGISTER(kick_short.speed_std) = 150;
      PARAMETER_REGISTER(kick_short.angle) = Math::fromDegrees(0.0);
      PARAMETER_REGISTER(kick_short.angle_std) = Math::fromDegrees(10);
      PARAMETER_REGISTER(kick_long.speed) = 1020;
      PARAMETER_REGISTER(kick_long.speed_std) = 150;
      PARAMETER_REGISTER(kick_long.angle) = Math::fromDegrees(0.0);
      PARAMETER_REGISTER(kick_long.angle_std) = Math::fromDegrees(10);
      PARAMETER_REGISTER(friction) = 0.0275;

      PARAMETER_REGISTER(good_threshold_percentage) = 0.85;
      PARAMETER_REGISTER(numParticles) = 30; 
      
      syncWithConfig();
    }
    
    ActionParams sidekick_right;
    ActionParams sidekick_left;
    ActionParams kick_short;
    ActionParams kick_long;
    double friction;
    double good_threshold_percentage;
    double numParticles;

  } theParameters;


  class Action
  {
  private:
    KickActionModel::ActionId _id;
    std::string _name;
    double action_speed;
    double action_speed_std;
    double action_angle;
    double action_angle_std;
    double friction;

    
  public:
    Action(KickActionModel::ActionId _id, const ActionParams& params, double friction) : 
		  _id(_id), 
      _name(KickActionModel::getName(_id)),
      action_speed(params.speed),
      action_speed_std(params.speed_std),
      action_angle(params.angle),
      action_angle_std(params.angle_std),
      friction(friction)
	  {
    }

    Vector2d predict(const Vector2d& ball) const;
    KickActionModel::ActionId id() const { return _id; }
    const std::string& name() const { return _name; }
  };
  
  enum BallPositionCategory
  {
    INFIELD,
    OPPOUT,
    OWNOUT,
    LEFTOUT,
    RIGHTOUT,
    OPPGOAL,
    OWNGOAL,
    COLLISION
  };

  class CategorizedBallPosition
  {
    private:
      Vector2d ballPosition;
      BallPositionCategory category;
    public:
      CategorizedBallPosition(const Vector2d& position, BallPositionCategory cat):
        ballPosition(position),
        category(cat)
      {}
      BallPositionCategory cat() const {return category;} 
      const Vector2d& pos() const {return ballPosition;} 
  };


private:

  std::vector<Action> action_local;
  std::vector<std::vector<CategorizedBallPosition> > actionsConsequences;


  void simulateConsequences(const Action & action, std::vector<CategorizedBallPosition>& categorizedBallPositions) const;

  size_t decide(const std::vector<std::vector<CategorizedBallPosition> >& actionsConsequences) const;

  Vector2d outsideField(const Vector2d& relativePoint) const;

  double evaluateAction(const Vector2d& a) const;

  void draw_potential_field() const;
};

#endif  /* _Simulation_H */
