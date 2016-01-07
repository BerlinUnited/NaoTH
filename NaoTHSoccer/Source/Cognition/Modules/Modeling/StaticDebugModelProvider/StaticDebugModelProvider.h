/**
* @file StaticDebugModelProvider.h
*/

#ifndef _StaticDebugModelProvider_H
#define _StaticDebugModelProvider_H

#include <ModuleFramework/Module.h>

#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/GoalModel.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"

BEGIN_DECLARE_MODULE(StaticDebugModelProvider)
    REQUIRE(FrameInfo)
    REQUIRE(FieldInfo)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)
    PROVIDE(DebugModify)
    PROVIDE(RobotPose)
    PROVIDE(SelfLocGoalModel)
    PROVIDE(BallModel)
END_DECLARE_MODULE(StaticDebugModelProvider)

class StaticDebugModelProvider: public StaticDebugModelProviderBase
{
public:
 StaticDebugModelProvider();

 ~StaticDebugModelProvider();

 virtual void execute();
};

#endif  /* _StaticDebugModelProvider_H */