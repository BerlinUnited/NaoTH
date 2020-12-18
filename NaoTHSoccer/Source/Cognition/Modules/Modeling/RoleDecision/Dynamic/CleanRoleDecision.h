/**
* @file CleanRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef _CleanRoleDecision_h_
#define _CleanRoleDecision_h_

#include <functional>

#include <ModuleFramework/Module.h>

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/BallModel.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CleanRoleDecision)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(SoccerStrategy)
  REQUIRE(TeamMessage)
  REQUIRE(TeamMessagePlayersState)
  REQUIRE(BodyState)
  REQUIRE(BallModel)
  REQUIRE(TeamBallModel)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(CleanRoleDecision);

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class CleanRoleDecision : public CleanRoleDecisionBase
{
public: 

  CleanRoleDecision();
  virtual ~CleanRoleDecision();

  /** executes the module */
  virtual void execute();

  void computeStrikers();
  
protected:
  bool amIactive() {
      return getBodyState().fall_down_state == BodyState::upright
          && getPlayerInfo().robotState != PlayerInfo::penalized
          && getBallModel().valid
          && (getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime()) < params.maxBallLostTime + myStrikerBonusTime());
  }

  int myStrikerBonusTime() {
      return getRoleDecisionModel().firstStriker == getPlayerInfo().playerNumber ? params.strikerBonusTime:0;
  }

  /**
   * @brief Selects the first/second striker out of the possible striker based on their player number.
   *        The smallest player number gets the first striker role and second smallest the second striker role.
   *        If theres someone faster to the ball, this player dont "wantsToBeStriker".
   * @param possible_striker map of player number and their time to ball
   * @param ownTimeToBall the time to ball of this player
   */
  void strikerSelectionByNumber(std::map<unsigned int, unsigned int> &possible_striker, double &ownTimeToBall);

  /**
   * @brief Selects the first/second striker out of the possible striker based on their time to the ball.
   *        The fastest player to the ball gets the first striker role and the seceond fastest gets the second striker role.
   *        If theres someone faster to the ball, this player dont "wantsToBeStriker".
   * @param possible_striker map of player number and their time to ball
   * @param ownTimeToBall the time to ball of this player
   */
  void strikerSelectionByTime(std::map<unsigned int, unsigned int> &possible_striker, double &ownTimeToBall);

  /**
   * @brief Selects the first/second striker out of the possible striker based on their time to the ball.
   *        The fastest player to the ball gets the first striker role and the seceond fastest gets the second striker role.
   *        BUT, if the goalie is one of the possible striker, he is set to the first striker! And if someone is faster just
   *        for a small threshold (strikerSelectionDiffThreshold), the one with the smaller number is prefered.
   *        If theres someone faster to the ball, this player dont "wantsToBeStriker".
   * @param possible_striker map of player number and their time to ball
   * @param ownTimeToBall the time to ball of this player
   */
  void strikerSelectionByTimeExceptGoalie(std::map<unsigned int, unsigned int> &possible_striker, double &ownTimeToBall);

  /**
   * @brief Selects the first/second striker out of the possible striker based on their time to the ball.
   *        The fastest player to the ball gets the first striker role and the seceond fastest gets the second striker role, if the
   *        ball distance from the first and the "second" are greater than a given min distance.
   *        BUT, if the goalie is one of the possible striker, he is set to the first striker! And if someone is faster just
   *        for a small threshold (strikerSelectionDiffThreshold), the one with the smaller number is prefered.
   *        If there is someone faster to the ball, this player dont "wantsToBeStriker".
   * @param possible_striker map of player number and their time to ball
   * @param ownTimeToBall the time to ball of this player
   */
  void strikerSelectionByTimeExceptGoalieWithBallCompare(std::map<unsigned int, unsigned int> &possible_striker, double &ownTimeToBall);

  class Parameters: public ParameterList
  {
  public:
    Parameters(): ParameterList("CleanRoleDecision")
    {
      PARAMETER_REGISTER(strikerBonusTime) = 4000;
      PARAMETER_REGISTER(maxBallLostTime) = 1000;
      PARAMETER_REGISTER(strikerSelectionDiffThreshold) = 500; // ms
      PARAMETER_REGISTER(firstSecondStrikerMinBallDistance) = 500.0; // mm
      PARAMETER_REGISTER(useSecondStriker) = true;
      PARAMETER_REGISTER(strikerSelection, &Parameters::setStrikerSelectionFunction) = 3;
      PARAMETER_REGISTER(strikerBallRadiusFunction, &Parameters::setStrikerBallRadiusFunction) = 3;
      PARAMETER_REGISTER(firstSecondStrikerLinBallDistanceM) = 0.35;
      PARAMETER_REGISTER(firstSecondStrikerLinBallDistanceN) = -40.0;

      // load from the file after registering all parameters
      syncWithConfig();
    }

    bool useSecondStriker;
    int strikerBonusTime;
    int maxBallLostTime;
    int strikerSelectionDiffThreshold;

    int strikerSelection;
    void (CleanRoleDecision::*strikerSelectionFunction)(std::map<unsigned int, unsigned int>&, double&) = &CleanRoleDecision::strikerSelectionByNumber;

    int strikerBallRadiusFunction;
    double (CleanRoleDecision::*ballDifferenceRadius)(double) = &CleanRoleDecision::ballDifferenceRadiusConstant;

    double firstSecondStrikerMinBallDistance;
    double firstSecondStrikerLinBallDistanceM;
    double firstSecondStrikerLinBallDistanceN;

    /**
     * @brief Update function, if another radius function should be used
     * @param selection
     */
    void setStrikerBallRadiusFunction(int selection) {
        // select a radius function based on the given new parameter
        switch (selection) {
            // simply uses a firstSecondStrikerMinBallDistance as min radius between first and second striker's ball
            case 1: ballDifferenceRadius = &CleanRoleDecision::ballDifferenceRadiusConstant; break;
            // simply uses a linear 'function f(x) = x*m + n' based on the distance to the ball of the second striker
            case 2: ballDifferenceRadius = &CleanRoleDecision::ballDifferenceRadiusLinear; break;
            // uses the firstSecondStrikerMinBallDistance or a linear 'function f(x) = x*m + n' based on the distance to the ball of the second striker to determine the min radius between first and second striker's ball
            case 3: ballDifferenceRadius = &CleanRoleDecision::ballDifferenceRadiusConstantLinear; break;
            // simply uses a firstSecondStrikerMinBallDistance as min radius between first and second striker's ball
            default: ballDifferenceRadius = &CleanRoleDecision::ballDifferenceRadiusConstant;
        }
    }

    /**
     * @brief Update function, if another striker selection function should be used
     * @param selection
     */
    void setStrikerSelectionFunction(int selection) {
        // select a selection function based on the given new parameter
        switch (selection) {
            case 2:  strikerSelectionFunction = &CleanRoleDecision::strikerSelectionByTimeExceptGoalie; break;
            case 3:  strikerSelectionFunction = &CleanRoleDecision::strikerSelectionByTimeExceptGoalieWithBallCompare; break;
            case 1:
            default: strikerSelectionFunction = &CleanRoleDecision::strikerSelectionByNumber; break;
        }
    }
  } params;

private:
  /**
   * @brief Calculates, if the ball position of the given player are different from each other.
   *        Different in this context means, the ball positions are greater than "firstSecondStrikerDistance".
   * @param first the "first" player number
   * @param second the "second" player number
   * @return true, if the balls are further away, otherwise false
   */
  bool inline isSecondStrikerDifferentFromFirst(unsigned int first, unsigned int second);

  /**
   * @brief Sets the dynamic role of the player to the striker.
   * @param number  the player, which role should be set
   */
  void inline setStrikerRole(unsigned int number);

  /* Some radius functions to determine similiar balls. */
  inline double ballDifferenceRadiusConstant(double /*d*/) { return params.firstSecondStrikerMinBallDistance; }
  inline double ballDifferenceRadiusLinear(double d) { return d * params.firstSecondStrikerLinBallDistanceM + params.firstSecondStrikerLinBallDistanceN; }
  inline double ballDifferenceRadiusConstantLinear(double d) { return std::max(ballDifferenceRadiusConstant(d), ballDifferenceRadiusLinear(d)); }
};

#endif //__CleanRoleDecision_h_
