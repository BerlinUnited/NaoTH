#ifndef _BodyCollisionDetector_H_
#define _BodyCollisionDetector_H_

//Include module
#include "ModuleFramework/Module.h"

//Include representations
#include "Representations/Motion/CollisionPercept.h"
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"
#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetHipFeetPose.h"
#include <Representations/Motion/MotionStatus.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/RobotInfo.h"

#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/IMUData.h"


//Include tools, datastructures and algorithms
#include <Tools/Math/Vector2.h>

//Include debugging and plotting tools
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/Color.h"


//Module declaration
BEGIN_DECLARE_MODULE(BodyCollisionDetector)
    //Provides
    PROVIDE(CollisionPercept)
    PROVIDE(TargetHipFeetPose)

    //Requires
    REQUIRE(FrameInfo)
    REQUIRE(Walk2018Parameters)
    REQUIRE(InertialModel)
    REQUIRE(IMUData)
    REQUIRE(CalibrationData)
    REQUIRE(StepBuffer)
    REQUIRE(GyrometerData)
    REQUIRE(RobotInfo)
    REQUIRE(MotionStatus)

    //Debug
    PROVIDE(DebugRequest)
    PROVIDE(DebugPlot)
    PROVIDE(DebugDrawings)
END_DECLARE_MODULE(BodyCollisionDetector)

class BodyCollisionDetector : private BodyCollisionDetectorBase
{
public:
  BodyCollisionDetector() :
        torsoParams(getWalk2018Parameters().torsoRotationStabilizerParams)
    {
        DEBUG_REQUEST_REGISTER("BodyCollisionDetector:drawImpactVector", "Impact vector", false);
    };
    ~BodyCollisionDetector();
    void execute();

private:
    void draw_impact_vector(Vector2d);
    const TorsoRotationStabilizerParameters& torsoParams;
};

#endif