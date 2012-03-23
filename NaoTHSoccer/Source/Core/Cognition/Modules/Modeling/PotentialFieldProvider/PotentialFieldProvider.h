/**
* @file PotentialFieldProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class PotentialFieldProvider
*/

#ifndef __PotentialFieldProvider_h_
#define __PotentialFieldProvider_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"

#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/CompassDirection.h"
#include "Representations/Motion/MotionStatus.h"

// Tools


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PotentialFieldProvider)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  


  REQUIRE(PlayerInfo)
  REQUIRE(BallModel)
  REQUIRE(SelfLocGoalModel)
  REQUIRE(LocalGoalModel)
  REQUIRE(PlayersModel)
  REQUIRE(RobotPose)
  REQUIRE(MotionStatus)
  REQUIRE(CompassDirection)
  
  PROVIDE(SoccerStrategy)
END_DECLARE_MODULE(PotentialFieldProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PotentialFieldProvider : public PotentialFieldProviderBase
{
public:

  PotentialFieldProvider();
  ~PotentialFieldProvider(){}

  /** executes the module */
  void execute();

private:

  /** @return the suggested attack direction in local coordinates */
  Vector2<double> calculateGoalPotentialField( const Vector2<double>& goal, const Vector2<double>& ball);

  /** @param ball position in local coordinates*/
  Vector2<double> calculatePotentialField(const Vector2<double>& point);

  Vector2<double> calculatePlayerPotentialField( const Vector2<double>& player, const Vector2<double>& ball);

  Vector2<double> getGoal(const Vector2<double>& ball, const GoalModel::Goal& oppGoalModel);
  Vector2<double> getGoalTarget(const Vector2<double>& point, const GoalModel::Goal& oppGoalModel);


  Vector2<double> approachBall(const Vector2<double>& point, const Vector2<double>& target);
  Vector2<double> calculateBallPotentialField(const Vector2<double>& point, const Vector2<double>& ball, const Vector2<double>& dir);
};

#endif //__PotentialFieldProvider_h_
