/**
* @file KinematicChainProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Calculates the kinematic chain based on the sensor data
*/

#ifndef _KinematicChainProviderMotion_H
#define _KinematicChainProviderMotion_H


#include <ModuleFramework/Module.h>
#include "Tools/Debug/DebugParameterList.h"

// representations
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/RobotInfo.h>

#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/FSRPositions.h"
#include "Representations/Modeling/IMUData.h"

#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Modeling/InertialModel.h>

BEGIN_DECLARE_MODULE(KinematicChainProviderMotion)
  PROVIDE(SensorJointData) // HACK
  PROVIDE(MotorJointData)  // HACK
  
  REQUIRE(RobotInfo)
  REQUIRE(IMUData)

  REQUIRE(InertialModel)
  REQUIRE(AccelerometerData)

  PROVIDE(FSRPositions)
  PROVIDE(KinematicChainSensor)
  PROVIDE(KinematicChainMotor)

  PROVIDE(DebugParameterList)
END_DECLARE_MODULE(KinematicChainProviderMotion)


class KinematicChainProviderMotion: private KinematicChainProviderMotionBase
{
public:
  KinematicChainProviderMotion();
  ~KinematicChainProviderMotion();

  void execute();

private:

  class Parameter : public ParameterList
  {
  public:
    Parameter() : ParameterList("KinematicChainProviderMotion")
    {
      PARAMETER_REGISTER(useIMUData) = true;
      syncWithConfig();
    }

    bool useIMUData;

  } parameter;
};

#endif  /* _KinematicChainProviderMotion_H */

