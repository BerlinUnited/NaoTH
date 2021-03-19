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
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Perception/MultiBallPercept.h"
//#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Modeling/BallModel.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"

//#include "Representations/Modeling/AttentionModel.h"

#include <Representations/Infrastructure/LEDRequest.h>
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Modeling/BodyStatus.h"

#include <Representations/Infrastructure/RemoteControlCommand.h>

#include <Representations/Perception/WhistlePercept.h>
#include "Representations/Infrastructure/AudioControl.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
//#include "Tools/Debug/DebugDrawings.h"


BEGIN_DECLARE_MODULE(BasicTestBehavior)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  //PROVIDE(DebugDrawingsField)
  REQUIRE(SensorJointData)  
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(MotionStatus)
  REQUIRE(MultiBallPercept)
  REQUIRE(BallModel)
  REQUIRE(OdometryData)
  //REQUIRE(AttentionModel)
  //REQUIRE(PlayersPercept)
  REQUIRE(WhistlePercept)

  REQUIRE(RemoteControlCommand)


  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(SoundPlayData)
  PROVIDE(BehaviorLEDRequest)
  PROVIDE(BodyStatus)
  PROVIDE(AudioControl)
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
  void testAudio();
};

#endif // _BasicTestBehavior_H_
