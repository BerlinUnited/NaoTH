/**
 * @file WholeGoalLocator.h
 *
 * @author <a href="mailto:kaptur@informatik.hu-berlin.de">Christian Kaptur</a>
 * Declaration of class WholeGoalLocator
 */

#ifndef _WholeGoalLocator_h_
#define _WholeGoalLocator_h_

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

#include "Tools/CameraGeometry.h"
#include "Tools/DoubleCamHelpers.h"

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


//#include "PlatformInterface/Platform.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(WholeGoalLocator)
  REQUIRE(FieldSidePercept)
  
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(OdometryData)
  REQUIRE(FieldInfo)
  REQUIRE(BodyState)
  REQUIRE(CameraInfo)
  REQUIRE(CompassDirection)

  REQUIRE(GoalPercept)
  REQUIRE(GoalPerceptTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  PROVIDE(CameraMatrixOffset)
  PROVIDE(SensingGoalModel)
END_DECLARE_MODULE(WholeGoalLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class WholeGoalLocator : private WholeGoalLocatorBase
{
public:
  WholeGoalLocator();
  virtual ~WholeGoalLocator(){}

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
  void execute(CameraInfo::CameraID id);

private:
  OdometryData lastRobotOdometry;

  bool checkAndCalculateSingleGoal(
    const GoalPercept::GoalPost& post1,
    const GoalPercept::GoalPost& post2);

  void correct_the_goal_percept(
    Vector2<double>& offset,
    GoalPercept::GoalPost& post1,
    GoalPercept::GoalPost& post2) const;

  double projectionError(
    double offsetX,
    double offsetY,
    const GoalPercept::GoalPost& post1,
    const GoalPercept::GoalPost& post2) const;

private:
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(WholeGoalLocator, GoalPercept);
  DOUBLE_CAM_REQUIRE(WholeGoalLocator, CameraMatrix);
};

#endif //_WholeGoalLocator_h_
