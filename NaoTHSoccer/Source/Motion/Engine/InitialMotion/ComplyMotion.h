/**
* @file ComplyMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef COMPLYMOTION_H
#define COMPLYMOTION_H

#include "Motion/Engine/AbstractMotion.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>


BEGIN_DECLARE_MODULE(ComplyMotion)
  REQUIRE(FrameInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(ComplyMotion)

class ComplyMotion : private ComplyMotionBase, public AbstractMotion
{
public:
  ComplyMotion() : AbstractMotion(motion::comply, getMotionLock()) {}
  virtual ~ComplyMotion(){}

  void execute() 
  {
    // copy sensor positions
    for (size_t i = 0; i < JointData::numOfJoint; ++i) {
      getMotorJointData().position[i] = getSensorJointData().position[i];
    }
    
    if(getMotionRequest().id == getId()) {
      setCurrentState(motion::running);
    } else {
      setCurrentState(motion::stopped);
    }
  }

};

#endif  /* COMPLYMOTION_H */

