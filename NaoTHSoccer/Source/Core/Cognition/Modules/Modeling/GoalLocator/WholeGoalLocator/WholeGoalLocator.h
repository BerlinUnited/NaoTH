/**
 * @file WholeGoalLocator.h
 *
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 * Declaration of class WholeGoalLocator
 */

#ifndef __WholeGoalLocator_h_
#define __WholeGoalLocator_h_

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

#include "Tools/CameraGeometry.h"


// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/CompassDirection.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
#include <Representations/Perception/FieldSidePercept.h>


#include "PlatformInterface/Platform.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(WholeGoalLocator)
  REQUIRE(FieldSidePercept)
  REQUIRE(GoalPercept)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(OdometryData)
  REQUIRE(FieldInfo)
  REQUIRE(BodyState)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraInfo)
  REQUIRE(CompassDirection)

  PROVIDE(CameraMatrixOffset)
  PROVIDE(SensingGoalModel)
END_DECLARE_MODULE(WholeGoalLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class WholeGoalLocator : private WholeGoalLocatorBase
{
public:
  WholeGoalLocator();

  ~WholeGoalLocator()
  {
  };

  virtual void execute();

private:
  OdometryData lastRobotOdometry;

  bool checkAndCalculateSingleGoal(
    const GoalPercept::GoalPost& post1,
    const GoalPercept::GoalPost& post2);

  void correct_the_goal_percept(
    Vector2<double>& offset,
    GoalPercept::GoalPost& post1,
    GoalPercept::GoalPost& post2);

  double projectionError(
    double offsetX,
    double offsetY,
    const GoalPercept::GoalPost& post1,
    const GoalPercept::GoalPost& post2);
};

#endif //__WholeGoalLocator_h_
