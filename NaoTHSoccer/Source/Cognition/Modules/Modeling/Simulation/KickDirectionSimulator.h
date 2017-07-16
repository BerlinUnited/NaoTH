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

  PROVIDE(KickActionModel)
END_DECLARE_MODULE(KickDirectionSimulator)

class KickDirectionSimulator : public KickDirectionSimulatorBase, public ModuleManager
{
public:
  KickDirectionSimulator();
  ~KickDirectionSimulator();

  virtual void execute();


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

  void resetSamples(SampleSet& samples, size_t n) const;
  void normalize(SampleSet& samples) const;
  void update(SampleSet& samples);
  void resample(SampleSet& sampleSet, int n, double sigma) const;
  void KickDirectionSimulator::calculate_best_direction(size_t iterations, size_t num_angle_particle);

public:
  double m_max;
  double m_min;
};

#endif  /* _KickDirectionSimulator_H */
