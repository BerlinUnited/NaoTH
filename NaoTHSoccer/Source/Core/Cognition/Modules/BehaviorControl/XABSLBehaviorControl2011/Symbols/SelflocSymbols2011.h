/**
* @file MotionSymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class SelflocSymbols2011 */

#ifndef _SelflocSymbols2011_h_
#define _SelflocSymbols2011_h_

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

BEGIN_DECLARE_MODULE(SelflocSymbols2011)
  REQUIRE(RobotPose)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(PlayerInfo)
  REQUIRE(GPSData)
  REQUIRE(MotionStatus)
  REQUIRE(CompassDirection)
  REQUIRE(FieldInfo)
END_DECLARE_MODULE(SelflocSymbols2011)

class SelflocSymbols2011: public SelflocSymbols2011Base
{

public:
  SelflocSymbols2011()
  :
    robotPose(getRobotPose()),
    selfLocGoalModel(getSelfLocGoalModel()),
    playerInfo(getPlayerInfo()),
    gpsData(getGPSData()),
    motionStatus(getMotionStatus()),
    compassDirection(getCompassDirection()),
    fieldInfo(getFieldInfo()),

    angleOnField(Math::toDegrees(robotPose.rotation)),
    rel2fieldX_x(0.0),
    rel2fieldX_y(0.0),
    rel2fieldY_x(0.0),
    rel2fieldY_y(0.0)
  {
    theInstance = this;
  };
  virtual ~SelflocSymbols2011(){}
  
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

    Goal(const Vector2<double>& leftPost, const Vector2<double>& rightPost)
      : leftPost(leftPost),
        rightPost(rightPost)
    {
      center = (leftPost+rightPost)*0.5;
      angleToCenter = Math::toDegrees(center.angle());
      distanceToCenter = center.abs();
    }

    double angleToCenter;
    double distanceToCenter;
    Vector2<double> center;
    Vector2<double> leftPost;
    Vector2<double> rightPost;
  };

  Goal oppGoal;
  Goal ownGoal;

  static double getFieldToRelativeX();
  static double getFieldToRelativeY();
  Vector2<double> parameterVector;

  static SelflocSymbols2011* theInstance;

  // representations
  RobotPose const& robotPose;
  SelfLocGoalModel const& selfLocGoalModel;
  PlayerInfo const& playerInfo;
  GPSData const& gpsData;
  MotionStatus const& motionStatus;
  CompassDirection const& compassDirection;
  FieldInfo const& fieldInfo;

  // rotation of robotPose in degrees
  double angleOnField;

  static double getRel2fieldX();
  static double getRel2fieldY();
  double rel2fieldX_x;
  double rel2fieldX_y;
  double rel2fieldY_x;
  double rel2fieldY_y;


  Pose2D robotPosePlanned;
  double angleOnFieldPlanned;
};// End class SelflocSymbols2011


#endif // __SelflocSymbols2011_h_

