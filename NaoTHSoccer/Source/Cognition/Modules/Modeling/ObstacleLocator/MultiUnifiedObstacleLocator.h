#ifndef MULTI_UNIFIED_OBSTACLE_LOCATOR_H
#define MULTI_UNIFIED_OBSTACLE_LOCATOR_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"

#include <Representations/Perception/ObstaclePercept.h>

#include <Representations/Modeling/ObstacleModel.h>
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Motion/MotionStatus.h"

BEGIN_DECLARE_MODULE(MultiUnifiedObstacleLocator)
// debug stuff
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(OdometryData)
  REQUIRE(UltrasonicObstaclePercept)

// for preview stuff
  REQUIRE(MotionStatus)

  PROVIDE(ObstacleModel)
END_DECLARE_MODULE(MultiUnifiedObstacleLocator)

class MultiUnifiedObstacleLocator : MultiUnifiedObstacleLocatorBase
{
public:
    MultiUnifiedObstacleLocator();
    virtual ~MultiUnifiedObstacleLocator();

    virtual void execute();

private:
    OdometryData lastRobotOdometry;

    // TODO: make a hypothesis class
    typedef std::vector<Obstacle> Obstacles;
    Obstacles obstacles;

    void provide_obstacle_model();
    void apply_odometry(Obstacle& obs);

    void do_debug();
};

#endif // MULTI_UNIFIED_OBSTACLE_LOCATOR_H
