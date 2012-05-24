/**
* @file PotentialFieldProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class PotentialFieldProvider
*/

#ifndef _PotentialFieldProvider_h_
#define _PotentialFieldProvider_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"

#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/RawAttackDirection.h"
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
  
  PROVIDE(RawAttackDirection)
END_DECLARE_MODULE(PotentialFieldProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PotentialFieldProvider : public PotentialFieldProviderBase
{
public:

  PotentialFieldProvider();
  virtual ~PotentialFieldProvider(){}

  /** executes the module */
  void execute();


  /**
    * Calculate the potential field for a given point and a target point (e.g. the center of the goal)
    * @param point The relative point for which the vector should be calculated.
    * @param targetPoint Relative coordinates of the target point.
    * @param obstacles List of valid obstacles that need to be avoided.
    * @return A vector describing the attack direction to the target point as represented by the potential field.
    *         The vector is in relative coordinates (same coordinate system as point and targetPoint)
    */
  Vector2<double> calculatePotentialField(
        const Vector2<double>& point, const Vector2<double> &targetPoint,
        const std::list<Vector2<double> >& obstacles);

  /**
    * Calculate the target point between the goal posts to shoot at.
    * @param point The relative point for which the target point should be calculated.
    * @param oppGoalModel Relative coordinates of the opponent goal.
    * @return Target point.
    */
  Vector2<double> getGoalTarget(const Vector2<double>& point, const GoalModel::Goal& oppGoalModel);


private:

  /** @return the suggested attack direction in local coordinates */
  Vector2<double> calculateGoalPotentialField( const Vector2<double>& goal, const Vector2<double>& ball);
  Vector2<double> calculatePlayerPotentialField( const Vector2<double>& player, const Vector2<double>& ball);

  std::list<Vector2<double> > getValidObstacles();

};

#endif //__PotentialFieldProvider_h_
