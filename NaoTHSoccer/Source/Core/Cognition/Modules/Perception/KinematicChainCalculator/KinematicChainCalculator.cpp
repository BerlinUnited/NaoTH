#include "KinematicChainCalculator.h"

KinematicChainCalculator::KinematicChainCalculator()
{
}

void KinematicChainCalculator::execute()
{
  double deltaTime = ( getFrameInfo().getTime() - udpateTime ) * 0.001;
  udpateTime = getFrameInfo().getTime();

  // calculate the kinematic chain
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    getAccelerometerData(),
    getInertialModel().orientation,
    getKinematicChain(),
    theFSRPos,
    deltaTime);

  getKinematicChain().updateCoM();
}

KinematicChainCalculator::~KinematicChainCalculator()
{

}
