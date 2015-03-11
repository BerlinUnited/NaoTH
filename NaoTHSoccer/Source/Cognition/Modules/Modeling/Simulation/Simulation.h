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
#include "Representations/Modeling/ActionNew.h"
#include "Representations/Motion/MotionStatus.h"

//Tools
#include <Tools/Math/Vector2.h>
#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>

// Debug
#include <Tools/Debug/DebugRequest.h>
BEGIN_DECLARE_MODULE(Simulation)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(PlayerInfo)

  REQUIRE(RobotPose)
  REQUIRE(BallModel)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(LocalGoalModel)
  REQUIRE(CompassDirection)
  REQUIRE(PlayersModel)
  REQUIRE(MotionStatus)
  PROVIDE(ActionNew)
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

      syncWithConfig();
    }
    
    double action_sidekick_distance;
    double action_short_kick_distance;
    double action_long_kick_distance;

  } theParameters;


  class Action
  {
  private:
    ActionNew::ActionId _id;
    Vector2d actionVector;

  public:
    Action(ActionNew::ActionId _id, const Vector2d& actionVector) : 
		  _id(_id), 
      actionVector(actionVector)
		  //potential(-1), //not used 
		  //goodness(0)    //not used
	  {}
	
	  Vector2d predict(const Vector2d& ball, double distance, double angle) const;
    ActionNew::ActionId id() { return _id; }

    Vector2d target;
    //double potential;
    //double goodness;
  };


private:

  std::vector<Action> action_local;
  Math::LineSegment shootLine;
  Vector2d calculateOneAction(const Action& lonely_action) const;

  Vector2d outsideField(const Vector2d& relativePoint) const;
};

#endif  /* _Simulation_H */