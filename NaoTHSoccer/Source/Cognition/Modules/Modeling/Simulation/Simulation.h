/**
* @file Simulation.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class Simulation
*/

#ifndef _Simulation_H
#define _Simulation_H

#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Tools/Math/Vector2.h>
#include "Representations/Modeling/Action.h"

BEGIN_DECLARE_MODULE(Simulation)
  REQUIRE(RobotPose)
  REQUIRE(BallModel)
END_DECLARE_MODULE(Simulation)

class Simulation: public SimulationBase
{
public:
 Simulation();
 ~Simulation();

 virtual void execute();

private:
  Vector2d simulate
    (
    const BallModel& ballmodel,
    const RobotPose& robotPose
    );
};

#endif  /* _Simulation_H */