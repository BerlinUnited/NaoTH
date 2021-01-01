#ifndef KINEMATICCHAINPROVIDER_H
#define KINEMATICCHAINPROVIDER_H

#include <ModuleFramework/Module.h>
#include "Tools/Debug/DebugParameterList.h"

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Modeling/IMUData.h>
#include <Representations/Modeling/KinematicChain.h>

#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Modeling/InertialModel.h>

BEGIN_DECLARE_MODULE(KinematicChainProvider)
  REQUIRE(FrameInfo)
  REQUIRE(IMUData)
  PROVIDE(SensorJointData) // HACK

  REQUIRE(InertialModel)
  REQUIRE(AccelerometerData)

  PROVIDE(KinematicChain)

  PROVIDE(DebugParameterList)
END_DECLARE_MODULE(KinematicChainProvider)

class KinematicChainProvider : public KinematicChainProviderBase
{
public:
  KinematicChainProvider();
  virtual void execute();
  virtual ~KinematicChainProvider();

private:
  unsigned int udpateTime;
  Vector3d theFSRPos[FSRData::numOfFSR];

  class Parameter : public ParameterList
  {
  public:
    Parameter() : ParameterList("KinematicChainProvider")
    {
      PARAMETER_REGISTER(useIMUData) = true;
      syncWithConfig();
    }

    bool useIMUData;

  } params;
};

#endif // KINEMATICCHAINPROVIDER_H
