#ifndef KINEMATICCHAINPROVIDER_H
#define KINEMATICCHAINPROVIDER_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Modeling/IMUData.h>
#include <Representations/Modeling/KinematicChain.h>

BEGIN_DECLARE_MODULE(KinematicChainProvider)
  REQUIRE(FrameInfo)
  REQUIRE(IMUData)
  PROVIDE(SensorJointData) // HACK

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
  Vector3d theFSRPos[FSRData::numOfFSR];
};

#endif // KINEMATICCHAINPROVIDER_H
