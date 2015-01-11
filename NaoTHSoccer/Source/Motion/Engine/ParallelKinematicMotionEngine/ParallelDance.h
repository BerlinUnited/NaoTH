/**
* @file ParallelDance.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#ifndef _ParallelDance_H
#define	_ParallelDance_H

#include "Motion/Engine/AbstractMotion.h"
#include "Motion/MorphologyProcessor/ParallelKinematic.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>

BEGIN_DECLARE_MODULE(ParallelDance)
  REQUIRE(RobotInfo)
  //REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(ParallelDance)

class ParallelDance : private ParallelDanceBase, public AbstractMotion
{
public:
  ParallelDance();
  
  virtual ~ParallelDance(){}

  void execute();
  
private:
  double radius; // current radius of the circle in mm
  double speed; // current speed of the motion in degreees
  double t; // current time

  Kinematics::ParallelKinematic theParallelKinematic;

  double taylorSine(double num, double precision = 19);
  double factorial(int num);
};

#endif	/* _ParallelDance_H */

