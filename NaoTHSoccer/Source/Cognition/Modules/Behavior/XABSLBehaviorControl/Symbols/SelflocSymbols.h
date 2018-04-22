/**
* @file MotionSymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class SelflocSymbols */

#ifndef _SelflocSymbols_h_
#define _SelflocSymbols_h_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/CompassDirection.h"
#include "Representations/Infrastructure/GPSData.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Motion/MotionStatus.h"

#include "Tools/Math/Common.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include <Representations/Modeling/KinematicChain.h>

BEGIN_DECLARE_MODULE(SelflocSymbols)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)

  REQUIRE(RobotPose)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(PlayerInfo)
  REQUIRE(GPSData)
  REQUIRE(MotionStatus)
  REQUIRE(CompassDirection)
  REQUIRE(FieldInfo)
  REQUIRE(KinematicChain)

END_DECLARE_MODULE(SelflocSymbols)

class SelflocSymbols: public SelflocSymbolsBase
{

public:
  SelflocSymbols()
  :
    angleOnField(Math::toDegrees(getRobotPose().rotation)),
    angleOnFieldPlanned(Math::toDegrees(getRobotPose().rotation)),
    toCloseToBorder(false)
  {
    theInstance = this;

    DEBUG_REQUEST_REGISTER("SelflocSymbols:draw_global_origin", "", false);
  };

  virtual ~SelflocSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** Updates the goal symbols */
  virtual void execute();

private:

  struct Goal
  {
    Goal()
    :
      angleToCenter(0.0),
      distanceToCenter(0.0)
    {} // needed for first initalization

    Goal(const Vector2d& leftPost, const Vector2d& rightPost)
      : leftPost(leftPost),
        rightPost(rightPost)
    {
      center = (leftPost+rightPost)*0.5;
      angleToCenter = Math::toDegrees(center.angle());
      distanceToCenter = center.abs();
    }

    double angleToCenter;
    double distanceToCenter;
    Vector2d center;
    Vector2d leftPost;
    Vector2d rightPost;
  };

  Goal oppGoal;
  Goal ownGoal;

  static double getFieldToRelativeX();
  static double getFieldToRelativeY();
  Vector2d parameterVector;

  static SelflocSymbols* theInstance;

  // rotation of robotPose in degrees
  double angleOnField;

  Pose2D robotPosePlanned;
  double angleOnFieldPlanned;

  double look_in_direction_factor;

  Vector2d safePoint;
  bool toCloseToBorder;
};// End class SelflocSymbols


#endif // _SelflocSymbols_h_

