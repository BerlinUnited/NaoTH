#ifndef KINEMATICCHAINPROVIDER_H
#define KINEMATICCHAINPROVIDER_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Modeling/KinematicChain.h>

BEGIN_DECLARE_MODULE(KinematicChainProvider)
  REQUIRE(AccelerometerData)
  REQUIRE(InertialSensorData)

  PROVIDE(KinematicChain)
END_DECLARE_MODULE(KinematicChainProvider)

class KinematicChainProvider : public KinematicChainProviderBase
{
public:
  KinematicChainProvider();
  virtual void execute();
  virtual ~KinematicChainProvider();

private:
  unsigned int udpateTime;
  Vector3<double> theFSRPos[FSRData::numOfFSR];
};

#endif // KINEMATICCHAINPROVIDER_H
