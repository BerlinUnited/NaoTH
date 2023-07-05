/**
* @file AttentionEngine.h
*/

#ifndef AttentionEngine_H
#define AttentionEngine_H

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

BEGIN_DECLARE_MODULE(AttentionEngine)
	REQUIRE(KinematicChain)
    REQUIRE(FrameInfo)
    REQUIRE(FieldInfo)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)
    PROVIDE(DebugModify)
    PROVIDE(RobotPose)
    PROVIDE(SelfLocGoalModel)
    PROVIDE(BallModel)
    PROVIDE(ObstacleModel)
END_DECLARE_MODULE(AttentionEngine)

class AttentionEngine: public AttentionEngineBase
{
public:
 AttentionEngine();

 ~AttentionEngine();

 virtual void execute();
};

#endif  /* AttentionEngine_H */