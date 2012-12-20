#include "KinematicChainProvider.h"
#include <Motion/MorphologyProcessor/ForwardKinematics.h>

KinematicChainProvider::KinematicChainProvider()
{
}

void KinematicChainProvider::execute()
{
  double deltaTime = ( getFrameInfo().getTime() - udpateTime ) * 0.001;
  udpateTime = getFrameInfo().getTime();

  // calculate the kinematic chain
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    getInertialModel().orientation,
    getAccelerometerData().getAcceleration(),
    getKinematicChain(),
    theFSRPos,
    deltaTime);

  //getKinematicChain().updateCoM();
}

KinematicChainProvider::~KinematicChainProvider()
{

}
