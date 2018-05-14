/**
* @file ArmCollisionDetector.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* ArmCollisionDetector: detect collisions with arms based on joint position deviation.
*/


#include "ArmCollisionDetector.h"

using namespace naoth;

ArmCollisionDetector::ArmCollisionDetector()
{
  getDebugParameterList().add(&params);
}

ArmCollisionDetector::~ArmCollisionDetector()
{
  getDebugParameterList().remove(&params);
}

void ArmCollisionDetector::execute()
{
  // hack: we cannot maesure collisions with arms back
  const bool armModeOK = 
    getMotionRequest().armMotionRequest.id == ArmMotionRequest::arms_synchronised_with_walk ||
    getMotionRequest().armMotionRequest.id == ArmMotionRequest::arms_down;

  const bool motionModeOK = getMotionStatus().currentMotion == motion::walk || getMotionStatus().currentMotion == motion::stand;

  if (!armModeOK || !motionModeOK)
  {
    // clear the joind command history
    jointDataBufferLeft.clear();
    jointDataBufferRight.clear();

    // clear collected errors
    collisionBufferLeft.clear();
    collisionBufferRight.clear();
    return;
  }

  jointDataBufferLeft.add(getMotorJointData().position[JointData::LShoulderPitch]);
  jointDataBufferRight.add(getMotorJointData().position[JointData::RShoulderPitch]);


  if (jointDataBufferLeft.isFull()) {
    double e = jointDataBufferLeft.first() - getSensorJointData().position[JointData::LShoulderPitch];
    collisionBufferLeft.add(std::fabs(e));
  }
  if (jointDataBufferRight.isFull()) {
    double e = jointDataBufferRight.first() - getSensorJointData().position[JointData::RShoulderPitch];
    collisionBufferRight.add(std::fabs(e));
  }
  PLOT("ArmCollisionDetector:collisionBufferRight", collisionBufferRight.getAverage());
  PLOT("ArmCollisionDetector:collisionBufferLeft", collisionBufferLeft.getAverage());


  double max_error = params.maxErrorStand;
  if (getMotionStatus().currentMotion == motion::walk) {
    max_error = params.maxErrorWalk;
  }

  // collision arm left
  if (collisionBufferLeft.isFull() && collisionBufferLeft.getAverage() > max_error) {
    getCollisionPercept().timeCollisionArmLeft = getFrameInfo().getTime();
  }

  // collision arm right
  if (collisionBufferRight.isFull() && collisionBufferRight.getAverage() > max_error) {
    getCollisionPercept().timeCollisionArmRight = getFrameInfo().getTime();
  } 

}//end execute

