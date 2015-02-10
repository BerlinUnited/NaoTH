/**
* @file BasicTestBehavior.h
*
* @author <a href="mailto:berndale@informatik.hu-berlin.de">Alexander Berndt </a>
* Definition of class RemoteControlBehavior
*/

#ifndef _RemoteControlBehavior_H_
#define _RemoteControlBehavior_H_

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

BEGIN_DECLARE_MODULE(RemoteControlBehavior)
  //PROVIDE(DebugRequest)
  //PROVIDE(DebugModify)
  //PROVIDE(DebugDrawingsField)
  
  //REQUIRE(FrameInfo)
  //REQUIRE(FieldInfo)
  //REQUIRE(MotionStatus)
  //REQUIRE(BallPercept)
  //REQUIRE(BallModel)
  //REQUIRE(AttentionModel)
  //REQUIRE(PlayersPercept)

  //PROVIDE(HeadMotionRequest)
  //PROVIDE(MotionRequest)
  //PROVIDE(SoundPlayData)
  //PROVIDE(BehaviorLEDRequest)
END_DECLARE_MODULE(RemoteControlBehavior)

class RemoteControlBehavior: public RemoteControlBehaviorBase
{
public:
  RemoteControlBehavior();
  ~RemoteControlBehavior(){};

  virtual void execute();

private:

};//end class RemoteControlBehavior
#endif // _RemoteControlBehavior_H_
