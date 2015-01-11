/**
* @file KinematicChainProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Calculates the kinematic chain based on the sensor data
*/

#ifndef _KinematicChainProviderMotion_H
#define _KinematicChainProviderMotion_H


#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/RobotInfo.h>

#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/FSRPositions.h"
#include "Representations/Modeling/InertialModel.h"

// tools


BEGIN_DECLARE_MODULE(KinematicChainProviderMotion)
  PROVIDE(SensorJointData) // HACK
  PROVIDE(MotorJointData) // HACK
  
  REQUIRE(RobotInfo)
  PROVIDE(InertialSensorData)
  PROVIDE(AccelerometerData)
  PROVIDE(GyrometerData)
  REQUIRE(InertialModel)
  REQUIRE(FSRData)

  PROVIDE(FSRPositions)
  PROVIDE(KinematicChainSensor)
  PROVIDE(KinematicChainMotor)
END_DECLARE_MODULE(KinematicChainProviderMotion)


class KinematicChainProviderMotion: private KinematicChainProviderMotionBase
{
public:
  KinematicChainProviderMotion();
  ~KinematicChainProviderMotion(){};

  void execute();
};

#endif  /* _KinematicChainProviderMotion_H */

