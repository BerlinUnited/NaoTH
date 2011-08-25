/**
* @file SensorBehaviorControl.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* Definition of class SensorBehaviorControl
*/

#ifndef __SensorBehaviorControl_H_
#define __SensorBehaviorControl_H_

#include <ModuleFramework/Module.h>


#include "Tools/Math/Pose2D.h"

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/BallPercept.h"

#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Motion/MotionRequest.h"

BEGIN_DECLARE_MODULE(SensorBehaviorControl)
  REQUIRE(FrameInfo)

  REQUIRE(BallPercept)

  PROVIDE(SoundPlayData)
  PROVIDE(MotionRequest)
END_DECLARE_MODULE(SensorBehaviorControl)

class SensorBehaviorControl: public SensorBehaviorControlBase
{
public:
  SensorBehaviorControl();
  ~SensorBehaviorControl(){}

  virtual void execute();

private:
  void testBehavior();

  FrameInfo lastPlayerFrameInfo;
};//end class ImageProcessor

#endif // __ImageProcessor_H_
