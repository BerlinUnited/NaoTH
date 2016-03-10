/**
* @file SimulationTest.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class SimulationTest
*/

#ifndef _SimulationTest_H
#define _SimulationTest_H

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

//
#include "Simulation.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"

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

// Debug
#include <Tools/Debug/DebugRequest.h>

BEGIN_DECLARE_MODULE(SimulationTest)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  PROVIDE(StopwatchManager)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(ObstacleModel)

  PROVIDE(BallModel) // fake the ball model
  PROVIDE(RobotPose) // fake the obot pose

  REQUIRE(KickActionModel)
END_DECLARE_MODULE(SimulationTest)

class SimulationTest: public SimulationTestBase, public ModuleManager
{
public:
  SimulationTest();
  ~SimulationTest();

  virtual void execute();

private:
  ModuleCreator<Simulation>* simulationModule;

  void draw_function(const std::vector<Vector3d>& function) const;
	double globRot;
};

#endif  /* _SimulationTest_H */
