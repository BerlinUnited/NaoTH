/**
* @file BasicTestBehavior.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* Definition of class BasicTestBehavior
*/

#ifndef _BasicTestBehavior_H_
#define _BasicTestBehavior_H_

#include <ModuleFramework/Module.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/SoundData.h"

#include "Representations/Perception/BallPercept.h"
//#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Modeling/BallModel.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"

//#include "Representations/Modeling/AttentionModel.h"

#include <Representations/Infrastructure/LEDRequest.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
//#include "Tools/Debug/DebugDrawings.h"

BEGIN_DECLARE_MODULE(BasicTestBehavior)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  //PROVIDE(DebugDrawingsField)
  
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(MotionStatus)
  REQUIRE(BallPercept)
  REQUIRE(BallModel)
  //REQUIRE(AttentionModel)
  //REQUIRE(PlayersPercept)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(SoundPlayData)
  PROVIDE(BehaviorLEDRequest)
END_DECLARE_MODULE(BasicTestBehavior)

class BasicTestBehavior: public BasicTestBehaviorBase
{
public:
  BasicTestBehavior();
  ~BasicTestBehavior(){};

  virtual void execute();

private:
  void testHead();
  void testMotion();
  void testLED();

};//end class ImageProcessor

#endif // _BasicTestBehavior_H_
