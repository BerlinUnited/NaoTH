/**
* @file SimpleMotionBehaviorControl.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* Definition of class SimpleMotionBehaviorControl
*/

#ifndef __SimpleMotionBehaviorControl_H_
#define __SimpleMotionBehaviorControl_H_

#include <ModuleFramework/Module.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Modeling/AttentionModel.h"

BEGIN_DECLARE_MODULE(SimpleMotionBehaviorControl)
  REQUIRE(FrameInfo)
  REQUIRE(BallPercept)
  REQUIRE(BallModel)
  REQUIRE(FieldInfo)
  REQUIRE(AttentionModel)
  REQUIRE(MotionStatus)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
END_DECLARE_MODULE(SimpleMotionBehaviorControl)

class SimpleMotionBehaviorControl: public SimpleMotionBehaviorControlBase
{
public:
  SimpleMotionBehaviorControl();
  ~SimpleMotionBehaviorControl(){};

  virtual void execute();

private:
  void testHead();
  void testMotion();

};//end class ImageProcessor

#endif // __ImageProcessor_H_
