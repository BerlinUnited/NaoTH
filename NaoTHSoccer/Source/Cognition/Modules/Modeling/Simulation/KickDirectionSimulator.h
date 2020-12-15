/**
* @file KickDirectionSimulator.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class KickDirectionSimulator
*/

#ifndef _KickDirectionSimulator_H
#define _KickDirectionSimulator_H

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/KickActionModel.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/SoccerStrategy.h"

//Tools
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Probabilistics.h>
#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"
#include <Representations/Debug/Stopwatch.h>
#include "Tools/Filters/AssymetricalBoolFilter.h"

//
#include "Tools/ActionSimulator.h"

// Debug
#include <Tools/Debug/DebugRequest.h>

BEGIN_DECLARE_MODULE(KickDirectionSimulator)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  PROVIDE(StopwatchManager)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(ObstacleModel)
  REQUIRE(BallModel)
  REQUIRE(RobotPose)
  //REQUIRE(SelfLocGoalModel)

  //Hack?
  PROVIDE(SoccerStrategy)

  PROVIDE(KickActionModel)
END_DECLARE_MODULE(KickDirectionSimulator)

class KickDirectionSimulator : public KickDirectionSimulatorBase, public ModuleManager
{
public:
  KickDirectionSimulator();
  ~KickDirectionSimulator();

  virtual void execute();

  /** parameters for the module */
  class Parameters : public ParameterList
  {
  public:

    Parameters() : ParameterList("KickDirectionSimulator")
    {
      PARAMETER_REGISTER(sidekick_right.speed) = 750;
      PARAMETER_REGISTER(sidekick_right.speed_std) = 150;
      PARAMETER_REGISTER(sidekick_right.angle) = -89.657943335302260;
      PARAMETER_REGISTER(sidekick_right.angle_std) = 10.553726275058064;

      PARAMETER_REGISTER(sidekick_left.speed) = 750;
      PARAMETER_REGISTER(sidekick_left.speed_std) = 150;
      PARAMETER_REGISTER(sidekick_left.angle) = 86.170795364136380;
      PARAMETER_REGISTER(sidekick_left.angle_std) = 10.669170653645670;

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
      PARAMETER_REGISTER(numParticles) = 30;
      PARAMETER_REGISTER(minGoalParticles) = 9;

      PARAMETER_REGISTER(obstacleFilter.g0) = 0.01;
      PARAMETER_REGISTER(obstacleFilter.g1) = 0.1;

      //KickDirection Stuff
      //Warum keine uniforme verteilung? damit wird gesichert das jede richtung mindestens ein sample bekomt
      PARAMETER_REGISTER(num_angle_particle) = 60;
      PARAMETER_REGISTER(iterations) = 10;

      syncWithConfig();
    }

    struct ObstacleFilter {
      double g0;
      double g1;
    } obstacleFilter;

    ActionSimulator::ActionParams sidekick_right;
    ActionSimulator::ActionParams sidekick_left;
    ActionSimulator::ActionParams kick_short;
    ActionSimulator::ActionParams kick_long;
    double friction;
    double good_threshold_percentage;
    int numParticles;
    int minGoalParticles;
    //KickDirection Stuff
    int num_angle_particle;
    int iterations; // Number of update calls of particle filter

  } params;

private:
  class Sample {
  public:
    Sample() : rotation(0.0), likelihood(0.0) {}
    double rotation;
    double likelihood;
  };

  typedef std::vector<Sample> SampleSet;
  SampleSet samples;


private:
  ModuleCreator<ActionSimulator>* simulationModule;

  std::vector<ActionSimulator::Action> action_local;
  //actionsConsequences now contain the angles in which the robot should turn -> rename it accordingly
  std::vector<double> actionsConsequences;
  std::vector<double> actionsConsequencesAbs;
  double calculate_best_direction(const ActionSimulator::Action& action);
  void update(const ActionSimulator::Action& action);
  void resample(SampleSet& sampleSet, double sigma) const;
  void normalize(SampleSet& samples) const;
  void resetSamples(SampleSet& samples, size_t n) const;  

public:
  double m_max;
  double m_min;
};

#endif  /* _KickDirectionSimulator_H */
