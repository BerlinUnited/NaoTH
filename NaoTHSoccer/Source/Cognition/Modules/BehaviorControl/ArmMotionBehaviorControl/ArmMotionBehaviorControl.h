/**
* @file ArmMotionBehaviorControl.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Definition of class ArmMotionBehaviorControl
*/

#ifndef __ArmMotionBehaviorControl_H_
#define __ArmMotionBehaviorControl_H_

#include <ModuleFramework/Module.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"

#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"


BEGIN_DECLARE_MODULE(ArmMotionBehaviorControl)
  REQUIRE(FrameInfo)
  REQUIRE(MotionStatus)

  PROVIDE(MotionRequest)
END_DECLARE_MODULE(ArmMotionBehaviorControl)

class ArmMotionBehaviorControl: public ArmMotionBehaviorControlBase
{
public:
  ArmMotionBehaviorControl();
  ~ArmMotionBehaviorControl(){};

  virtual void execute();

private:
  void execArms();

};//end class ArmMotionBehaviorControl

#endif // __ArmMotionBehaviorControl_H_
