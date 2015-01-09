#include "KinematicChainProvider.h"
#include <Motion/MorphologyProcessor/ForwardKinematics.h>

KinematicChainProvider::KinematicChainProvider()
{
  udpateTime = getFrameInfo().getTime();

  // HACK
  if(!getKinematicChain().is_initialized())
  {
    getKinematicChain().init(getSensorJointData());
  }
}

void KinematicChainProvider::execute()
{
  double deltaTime = ( getFrameInfo().getTime() - udpateTime ) * 0.001;
  udpateTime = getFrameInfo().getTime();

  // calculate the kinematic chain
  Kinematics::ForwardKinematics::updateKinematicChainAll(
    getInertialModel().orientation,
    getAccelerometerData().getAcceleration(),
    deltaTime,
    getKinematicChain(),
    theFSRPos);
}

KinematicChainProvider::~KinematicChainProvider()
{

}
