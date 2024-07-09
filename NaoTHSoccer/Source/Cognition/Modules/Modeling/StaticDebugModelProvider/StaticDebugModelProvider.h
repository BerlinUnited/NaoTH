/**
* @file StaticDebugModelProvider.h
*/

#ifndef STATICDEBUGMODELPROVIDER_H
#define STATICDEBUGMODELPROVIDER_H

#include <ModuleFramework/Module.h>

#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Modeling/KinematicChain.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"

BEGIN_DECLARE_MODULE(StaticDebugModelProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugModify)
  
  REQUIRE(KinematicChain)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)

  PROVIDE(RobotPose)
  PROVIDE(SelfLocGoalModel)
  PROVIDE(BallModel)
  PROVIDE(ObstacleModel)
END_DECLARE_MODULE(StaticDebugModelProvider)

class StaticDebugModelProvider: public StaticDebugModelProviderBase
{
public:
  StaticDebugModelProvider();

  ~StaticDebugModelProvider(){}

 virtual void execute();
};

#endif  /* STATICDEBUGMODELPROVIDER_H */