/**
* @file PathPlanner.h
*
* @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
* Definition of class PathPlanner
*/

#ifndef _PathPlanner_H_
#define _PathPlanner_H_

#include <ModuleFramework/Module.h>

// representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Perception/BallPercept.h"
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

#include <Representations/Infrastructure/WhistlePercept.h>
#include "Representations/Infrastructure/WhistleControl.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
//#include "Tools/Debug/DebugDrawings.h"


BEGIN_DECLARE_MODULE(PathPlanner)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)

  REQUIRE(FrameInfo)
  REQUIRE(MotionStatus)
  REQUIRE(BallPercept)
  REQUIRE(MultiBallPercept)
  REQUIRE(BallModel)
  REQUIRE(OdometryData)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
END_DECLARE_MODULE(PathPlanner)

class PathPlanner: public PathPlannerBase
{
public:
  PathPlanner();
  ~PathPlanner(){};

  virtual void execute();

private:
  bool isWalking;
};

#endif // _PathPlanner_H_
