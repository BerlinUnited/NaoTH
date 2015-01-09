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
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>

// Debug
#include "Tools/Debug/DebugModify.h"
#include <Tools/Debug/DebugRequest.h>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PotentialFieldProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugModify)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)  
  REQUIRE(PlayerInfo)

  REQUIRE(RobotPose)
  REQUIRE(BallModel)
  REQUIRE(SelfLocGoalModel)
//  REQUIRE(LocalGoalModel)
  REQUIRE(CompassDirection)
  REQUIRE(PlayersModel)
  REQUIRE(MotionStatus)
  
  PROVIDE(RawAttackDirection)
END_DECLARE_MODULE(PotentialFieldProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class PotentialFieldProvider : public PotentialFieldProviderBase
{
public:

  PotentialFieldProvider();
  virtual ~PotentialFieldProvider();

  /** executes the module */
  void execute();

  /** parameters for the module */
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("PotentialFieldParameters")
    {
      PARAMETER_REGISTER(goal_attractor_strength) = -0.001;
      PARAMETER_REGISTER(goal_post_offset) = 200.0;
      PARAMETER_REGISTER(goal_line_offset_front) = 100.0;
      PARAMETER_REGISTER(goal_line_offset_back) = 100.0;

      PARAMETER_REGISTER(player_repeller_radius) = 2000;
      PARAMETER_REGISTER(player_repeller_strenth) = 1500;

      syncWithConfig();
    }

    // 
    double goal_attractor_strength;
    double goal_post_offset;
    double goal_line_offset_front;
    double goal_line_offset_back;

    //
    double player_repeller_radius;
    double player_repeller_strenth;
    
  } theParameters;


private:

  /**
    * Calculate the target point between the goal posts to shoot at.
    * @param point The relative point for which the target point should be calculated.
    * @param oppGoalModel Relative coordinates of the opponent goal.
    * @return Target point.
    */
  Vector2d getGoalTarget(const Vector2d& point, const GoalModel::Goal& oppGoalModel) const;

  /**
    */
  Vector2d getGoalTargetOld(const Vector2d& point, const GoalModel::Goal& oppGoalModel) const;

  /** 
    * Calculate the list of valide obstacles (opponents, teammates)
    */
  std::list<Vector2d > getValidObstacles() const;


  // TODO: following methods should be made more universal and moved to a toolbox 

  /**
    * Calculate the potential field at a given point for a target point (e.g. the center of the goal)
    * acting as attractor and a list of abstacles acting as repellers
    * @param point The relative point for which the vector should be calculated.
    * @param targetPoint Relative coordinates of the target point.
    * @param obstacles List of valid obstacles that need to be avoided.
    * @param a 2D transformation applied to the field
    * @return A vector describing the attack direction to the target point as represented by the potential field.
    *         The vector is in relative coordinates (same coordinate system as point and targetPoint)
    */
  Vector2d calculatePotentialField(
        const Vector2d& point, 
        const Vector2d& targetPoint,
        const std::list<Vector2d >& obstacles) const;

  /**
    * Global field pointing to the point p with axponential strenth depending on the distance.
    * @param x point at which to evaluate the vector field
    * @param p position of the attractor
    * @return potential vector at the point x 
    */
  Vector2d globalExponentialAttractor( const Vector2d& p, const Vector2d& x ) const;

  /**
    * A repeller produces a round fild around a point which 'pushes' away.
    * The influence of the repeller is limited to the radius d.
    * The sloping parameter 0 < a < d defines the strenth of the field 
    * depending on the distance to the repeller.
    * @param x point at which to evaluate the vector field
    * @param p position of the repeller
    * @return potential vector at the point x 
    */
  Vector2d compactExponentialRepeller( const Vector2d& p, const Vector2d& x ) const;

};

#endif //_PotentialFieldProvider_h_
