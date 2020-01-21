/**
* @file SituationPriorProvider.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class SituationPriorProvider
*/

#include "SituationPriorProvider.h"

SituationPriorProvider::SituationPriorProvider()
{ 
  lastRobotState = getPlayerInfo().robotState;
  currentRobotState = getPlayerInfo().robotState;
  gameStateWhenPenalized = GameData::unknown_game_state;
  reasonForLastPenalized = GameData::penalty_none;

  walked_after_penalized_or_init = false;
  wasLiftedUp = false;
}

SituationPriorProvider::~SituationPriorProvider()
{
}

void SituationPriorProvider::execute()
{
  //reset prior
  getSituationPrior().currentPrior = SituationPrior::none;

  // robot state changed
  if(getPlayerInfo().robotState != currentRobotState) {
    lastRobotState = currentRobotState;
    currentRobotState = getPlayerInfo().robotState;

    if(getPlayerInfo().robotState == PlayerInfo::penalized) {
      gameStateWhenPenalized = getGameData().gameState;

      if(getGameData().valid) {
        reasonForLastPenalized = getGameData().getOwnRobotInfo(getPlayerInfo().playerNumber).penalty;
      }
    }
  }

  // calculate walked_after_penalized_or_init
  if( currentRobotState == PlayerInfo::set || 
      currentRobotState == PlayerInfo::initial || 
      currentRobotState == PlayerInfo::penalized) 
  {
    walked_after_penalized_or_init = false;
  }
  if(getMotionStatus().currentMotion == motion::walk) {
    walked_after_penalized_or_init = true;
    wasLiftedUp = false;
  }
  
  //get BodyState
  if(getBodyState().isLiftedUp){
    wasLiftedUp = true;
  }

  //Init Positions and the robot didn't start walking
  else if(currentRobotState == PlayerInfo::ready && lastRobotState == PlayerInfo::initial && !walked_after_penalized_or_init)
  {
    getSituationPrior().currentPrior = SituationPrior::firstReady;
  }
  // treat positioning after penalized
  else if( lastRobotState == PlayerInfo::penalized && !walked_after_penalized_or_init )
  {
    // robot was penalized in set
    //if(gameStateWhenPenalized == GameData::set) 
    if(reasonForLastPenalized == GameData::illegal_motion_in_set)
    {
      if(getPlayerInfo().playerNumber == 1) {
        //The Goalie will be in the own Goal if manually placed in set
        getSituationPrior().currentPrior = SituationPrior::goaliePenalizedInSet;
      } else {
        getSituationPrior().currentPrior = SituationPrior::positionedInSet;
      }
    }
    // robot was penalized in game
    else //if (gameStateWhenPenalized == GameData::playing)
    {
      getSituationPrior().currentPrior = SituationPrior::playAfterPenalized;
    }
  }
  //Set
  else if(currentRobotState == PlayerInfo::set)
  {
    getSituationPrior().currentPrior = SituationPrior::set;
  }
}