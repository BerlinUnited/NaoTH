/**
* @file SituationPriorProvider.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Stella Alice Schlotter</a>
* Implementation of class SituationPriorProvider
*/

#include "SituationPriorProvider.h"

SituationPriorProvider::SituationPriorProvider()
{ 
  lastRobotState          = getPlayerInfo().robotState;
  currentRobotState       = getPlayerInfo().robotState;
  gameStateWhenPenalized  = GameData::unknown_game_state;
  reasonForLastPenalized  = GameData::penalty_none;

  walked_since_state_change = false;
}

void SituationPriorProvider::execute()
{
  // reset prior
  getSituationPrior().currentPrior = SituationPrior::none;

  // robot state changed
  if(getPlayerInfo().robotState != currentRobotState)
  {
    lastRobotState    = currentRobotState;
    currentRobotState = getPlayerInfo().robotState;

    walked_since_state_change = false;

    // remember the reason for the current penalty if the robot was penalized 
    // and the global game state when the penalry was isseud
    if(getPlayerInfo().robotState == PlayerInfo::penalized) {
      gameStateWhenPenalized = getGameData().gameState;

      if(getGameData().valid) {
        reasonForLastPenalized = getGameData().getOwnRobotInfo(getPlayerInfo().playerNumber).penalty;
      }
    }
  }

  if(getMotionStatus().currentMotion == motion::walk) {
    walked_since_state_change = true;
  }
  
  // TODO: for the future - track whether the robot was manually moved
  /*
  // in those states the robot does not walk and can be moved manually
  if( currentRobotState == PlayerInfo::set || 
      currentRobotState == PlayerInfo::initial || 
      currentRobotState == PlayerInfo::penalized ||
      currentRobotState == PlayerInfo::standby)
  {
    ...
  }

  if(getBodyState().isLiftedUp) {
    ...
  }
  */

  // all static priors are not valid when the robot walks
  if(walked_since_state_change) {
    getSituationPrior().currentPrior = SituationPrior::none;
    return;
  }

  // initial positions at the first ready
  else if(currentRobotState == PlayerInfo::ready && (lastRobotState == PlayerInfo::initial || lastRobotState == PlayerInfo::standby))
  {
    getSituationPrior().currentPrior = SituationPrior::firstReady;
  }
  // treat positioning after penalized
  else if( lastRobotState == PlayerInfo::penalized )
  {
    // robot was penalized in set
    if(reasonForLastPenalized == GameData::motion_in_set)
    {
      if(getPlayerInfo().playerNumber == 1) {
        // The Goalie will be in the own goal if manually placed in set
        getSituationPrior().currentPrior = SituationPrior::goaliePenalizedInSet;
      } else {
        getSituationPrior().currentPrior = SituationPrior::positionedInSet;
      }
    }
    // robot was penalized during the play state and will be placed at a sideline
    else
    {
      getSituationPrior().currentPrior = SituationPrior::playAfterPenalized;
    }
  }
  // Set
  else if(currentRobotState == PlayerInfo::set)
  {
    getSituationPrior().currentPrior = SituationPrior::set;
  }
}