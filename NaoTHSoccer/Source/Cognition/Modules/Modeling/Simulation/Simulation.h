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

//Tools
#include <Tools/Math/Vector2.h>
#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"

// Debug
#include <Tools/Debug/DebugRequest.h>
BEGIN_DECLARE_MODULE(Simulation)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(PlayerInfo)

  REQUIRE(RobotPose)
  REQUIRE(BallModel)
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

   /** parameters for the module */
  class Parameters: public ParameterList
  {
  public:
	
    Parameters() : ParameterList("PotentialActionParameters")
    {
      PARAMETER_REGISTER(action_sidekick_distance) = 310;
      PARAMETER_REGISTER(action_short_kick_distance) = 2200;
      PARAMETER_REGISTER(action_long_kick_distance) = 2750;

      PARAMETER_REGISTER(distance_variance) = 0.1;
      PARAMETER_REGISTER(angle_variance) = Math::fromDegrees(5);

      PARAMETER_REGISTER(goal_percentage) = 0.85;
      syncWithConfig();
    }
    
    double action_sidekick_distance;
    double action_short_kick_distance;
    double action_long_kick_distance;
    double distance_variance;
    double angle_variance;

    double goal_percentage;

  } theParameters;


  class Action
  {
  private:
    KickActionModel::ActionId _id;
    Vector2d actionVector;
    std::string _name;

  public:
    Action(KickActionModel::ActionId _id, const Vector2d& actionVector) : 
		  _id(_id), 
      actionVector(actionVector)
	  {
      switch(_id)
      {
        case KickActionModel::none:
          _name = "none";
          break;
        case KickActionModel::kick_short:
          _name = "kick_short";
          break;
        case KickActionModel::kick_long:
          _name = "kick_long";
          break;
        case KickActionModel::sidekick_left:
          _name = "sidekick_left";
          break;
        case KickActionModel::sidekick_right:
          _name = "sidekick_right";
          break;
        default:
          _name = "undefined";
      }
    }
	
	  Vector2d predict(const Vector2d& ball, double distance, double angle) const;
    KickActionModel::ActionId id() { return _id; }
    std::string name() { return _name; }

  };
  
  enum BallPositionCategory
  {
    INFIELD,
    OPPOUT,
    OWNOUT,
    LEFTOUT,
    RIGHTOUT,
    OPPGOAL,
    OWNGOAL
  };

  class CategorizedBallPosition
  {
    private:
      Vector2d ballPosition;
      BallPositionCategory category;
    public:
      CategorizedBallPosition(Vector2d position, BallPositionCategory cat):
        ballPosition(position),
        category(cat)
      {}
      BallPositionCategory cat() const {return category;} 
      const Vector2d& pos() const {return ballPosition;} 
  };

private:

  std::vector<Action> action_local;

  void categorizePosition(const std::vector<Vector2d>& ballPositionResults, std::vector<CategorizedBallPosition>& categorizedBallPositions) const;

  Vector2d outsideField(const Vector2d& relativePoint) const;

  double evaluateAction(const Vector2d& a) const;

  void draw_potential_field() const;
};

#endif  /* _Simulation_H */
