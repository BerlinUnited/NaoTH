#ifndef _bodyCollisionDetector_H_
#define _bodyCollisionDetector_H_

//Include module
#include "ModuleFramework/Module.h"

//Include representations
#include "Representations/Motion/CollisionPercept.h"
#include "Representations/Motion/Walk2018/Walk2018Parameters.h"
#include "Representations/Motion/Walk2018/StepBuffer.h"
#include "Representations/Motion/Walk2018/TargetHipFeetPose.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/CalibrationData.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/RobotInfo.h"

#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/IMUData.h"


//Include tools, datastructures and algorithms

//Include debugging and plotting tools
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/Color.h"


//Module declaration
BEGIN_DECLARE_MODULE(bodyCollisionDetector)
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

    //Debug
    PROVIDE(DebugRequest)
    PROVIDE(DebugPlot)
    PROVIDE(DebugDrawings)
END_DECLARE_MODULE(bodyCollisionDetector)

class bodyCollisionDetector : private bodyCollisionDetectorBase
{
public:
    bodyCollisionDetector() : 
        torsoParams(getWalk2018Parameters().torsoRotationStabilizerParams)
    {};
    ~bodyCollisionDetector();
    void execute();

private:
    const TorsoRotationStabilizerParameters& torsoParams;
};

#endif