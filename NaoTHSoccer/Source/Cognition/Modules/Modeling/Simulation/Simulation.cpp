/**
* @file Simulation.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class Simulation
*/

#include "Simulation.h"

  Simulation::Simulation()
    {
    // initialize some stuff here
    }

  Simulation::~Simulation(){}
  void Simulation::execute()
  {
  // do some stuff here
  //simulate(ballmodel,robotPose);
  }
  Vector2d Simulation::simulate(
    const BallModel& ballmodel,
    const RobotPose& robotPose
    )
  {
    return Vector2d(1,1);
  }