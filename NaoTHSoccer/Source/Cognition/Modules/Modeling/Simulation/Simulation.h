/**
* @file Simulation.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Stella Alice Schlotter</a>
* Declaration of class Simulation
*/

#ifndef Simulation_H
#define Simulation_H

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/KickActionModel.h"

//Tools
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Probabilistics.h>
#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"
#include <Representations/Debug/Stopwatch.h>
#include "Tools/Filters/AssymetricalBoolFilter.h"

// Debug
#include <Tools/Debug/DebugRequest.h>

//
#include "Tools/ActionSimulator.h"

BEGIN_DECLARE_MODULE(Simulation)
PROVIDE(DebugModify)
PROVIDE(DebugRequest)
PROVIDE(DebugDrawings)
PROVIDE(DebugParameterList)
PROVIDE(StopwatchManager)

REQUIRE(FrameInfo)
REQUIRE(FieldInfo)
REQUIRE(BallModel)
REQUIRE(RobotPose)
//REQUIRE(SelfLocGoalModel)

PROVIDE(KickActionModel)
END_DECLARE_MODULE(Simulation)

class Simulation : public SimulationBase, public ModuleManager
{
public:
  Simulation();
  ~Simulation();

  virtual void execute();


  /** parameters for the module */
  class Parameters : public ParameterList
  {
  public:

    Parameters() : ParameterList("Simulation")
    {
      //PARAMETER_REGISTER(sidekick_right.speed) = 750;
      //PARAMETER_REGISTER(sidekick_right.speed_std) = 150;
      //PARAMETER_REGISTER(sidekick_right.angle) = -89.657943335302260;
      //PARAMETER_REGISTER(sidekick_right.angle_std) = 10.553726275058064;

      //PARAMETER_REGISTER(sidekick_left.speed) = 750;
      //PARAMETER_REGISTER(sidekick_left.speed_std) = 150;
      //PARAMETER_REGISTER(sidekick_left.angle) = 86.170795364136380;
      //PARAMETER_REGISTER(sidekick_left.angle_std) = 10.669170653645670;

      PARAMETER_REGISTER(kick_short.speed) = 1280;
      PARAMETER_REGISTER(kick_short.speed_std) = 150;
      PARAMETER_REGISTER(kick_short.angle) = 8.454482265522328;
      PARAMETER_REGISTER(kick_short.angle_std) = 6.992268841997358;

      //PARAMETER_REGISTER(kick_long.speed) = 1020;
      //PARAMETER_REGISTER(kick_long.speed_std) = 150;
      //PARAMETER_REGISTER(kick_long.angle) = 8.454482265522328;
      //PARAMETER_REGISTER(kick_long.angle_std) = 6.992268841997358;

      PARAMETER_REGISTER(friction) = 0.0275;

      PARAMETER_REGISTER(good_threshold_percentage) = 0.85;
      PARAMETER_REGISTER(significance_thresh) = 0.1;
      PARAMETER_REGISTER(numParticles) = 30;
      PARAMETER_REGISTER(minGoalParticles) = 9;

      PARAMETER_REGISTER(obstacleFilter.g0) = 0.01;
      PARAMETER_REGISTER(obstacleFilter.g1) = 0.1;

      syncWithConfig();
    }

    struct ObstacleFilter {
      double g0;
      double g1;
    } obstacleFilter;

    //ActionSimulator::ActionParams sidekick_right;
    //ActionSimulator::ActionParams sidekick_left;
    ActionSimulator::ActionParams kick_short;
    // currently not used
    // ActionSimulator::ActionParams kick_long;
    double friction;
    double good_threshold_percentage;
    double significance_thresh;
    int numParticles; //should be size_t
    int minGoalParticles;

  } theParameters;


private:
  //AssymetricalBoolHysteresisFilter obstacleFilter;

  ModuleCreator<ActionSimulator>* simulationModule;

  size_t decide_smart(const std::vector<ActionSimulator::ActionResults>& actionsConsequences) const;

  std::vector<ActionSimulator::Action> action_local;
  std::vector<ActionSimulator::ActionResults> actionsConsequences;

  void draw_action_results(const ActionSimulator::ActionResults& actionsResults, const Color& color) const;

};

#endif  /* Simulation_H */
