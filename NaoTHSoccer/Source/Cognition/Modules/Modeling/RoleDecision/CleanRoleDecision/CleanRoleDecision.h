/**
* @file CleanRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef _CleanRoleDecision_h_
#define _CleanRoleDecision_h_

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
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
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
END_DECLARE_MODULE(CleanRoleDecision)

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
          && (getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime()) < parameters.maxBallLostTime + myStrikerBonusTime());
  }

  int myStrikerBonusTime() {
      return getRoleDecisionModel().firstStriker == getPlayerInfo().playerNumber ? parameters.strikerBonusTime:0;
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
      PARAMETER_REGISTER(strikerSelection) = 3;
      PARAMETER_REGISTER(strikerSelectionDiffThreshold) = 500; // ms
      PARAMETER_REGISTER(useSecondStriker) = true;
      PARAMETER_REGISTER(firstSecondStrikerBallDistance) = 500; // mm
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    bool useSecondStriker;
    int strikerBonusTime;
    int maxBallLostTime;
    int strikerSelection;
    int strikerSelectionDiffThreshold;
    int firstSecondStrikerBallDistance;
  } parameters;

private:
  /**
   * @brief Calculates, if the ball position of the given player are different from each other.
   *        Different in this context means, the ball positions are greater than "firstSecondStrikerDistance".
   * @param first the "first" player number
   * @param second the "second" player number
   * @return true, if the balls are further away, otherwise false
   */
  bool inline isSecondStrikerDifferentFromFirst(unsigned int first, unsigned int second);
};

#endif //__CleanRoleDecision_h_
