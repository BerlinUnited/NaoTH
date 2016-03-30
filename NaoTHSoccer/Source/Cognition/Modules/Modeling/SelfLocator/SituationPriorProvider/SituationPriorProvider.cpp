/**
* @file SituationPriorProvider.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class SituationPriorProvider
*/

#include "SituationPriorProvider.h"

SituationPriorProvider::SituationPriorProvider()
{  
  lastState = getPlayerInfo().gameData.gameState;
  currentState = getPlayerInfo().gameData.gameState;

  startedWalking = false;
  body_lift_up = false;
}

SituationPriorProvider::~SituationPriorProvider()
{
}

void SituationPriorProvider::execute()
{
  reset();
  //get BodyState
  body_lift_up = isLiftedUp();

  //Calculate lastGameState
  if(getPlayerInfo().gameData.gameState != currentState){
    lastState = currentState;
    currentState = getPlayerInfo().gameData.gameState;
  }

  //for Debug stuff
  if(getSituationStatus().oppHalf)
  {
    getSituationPrior().currentPrior = getSituationPrior().oppHalf;
  }
  //Init Positions
  else if(getPlayerInfo().gameData.gameState == GameData::ready && lastState == GameData::initial)
  {
    //Dont update afer the robot started walking
    if(getMotionStatus().currentMotion == motion::walk && startedWalking == false)
    {
      startedWalking = true;
      getSituationPrior().currentPrior = getSituationPrior().none;
    }
    if(startedWalking == false){
      getSituationPrior().currentPrior = getSituationPrior().firstReady;
    }
  }
  //Penalized in Set or Ready for Goalie
  else if(getPlayerInfo().gameData.playerNumber == 1 && getPlayerInfo().gameData.gameState == GameData::set && lastState == GameData::penalized){
    //The Goalie will be in the own Goal if manually placed in set
    getSituationPrior().currentPrior = getSituationPrior().goaliePenalizedInSet;
  }
  //Positioned in Set, e.g.: Penalized in Set or Ready or manual placement
  else if(
    getPlayerInfo().gameData.gameState == GameData::set
    && lastState == GameData::penalized
    //localise only of already standing
    || (wasLiftedUp == true && body_lift_up == false))
  {
    wasLiftedUp = false;
    getSituationPrior().currentPrior = getSituationPrior().positionedInSet;
  }
  //Penalized in Play
  else if(getPlayerInfo().gameData.gameState == GameData::playing && lastState == GameData::penalized)
  {
    //Dont update afer the robot started walking
    if(getMotionStatus().currentMotion == motion::walk && startedWalking == false)
    {
      startedWalking = true;
      getSituationPrior().currentPrior = getSituationPrior().none;
    }
    if(startedWalking == false){
      getSituationPrior().currentPrior = getSituationPrior().playAfterPenalized;
    }
  }
  //Set
  else if(getPlayerInfo().gameData.gameState == GameData::set)
  {
    getSituationPrior().currentPrior = getSituationPrior().set;
  }
  //Play, Finished
  else
  {
    getSituationPrior().currentPrior = getSituationPrior().none;
  }
}

void SituationPriorProvider::reset(){
  //Reset the booleans
  //Priors will no longer be active after the robot started walking
  if(getPlayerInfo().gameData.gameState == GameData::initial || getPlayerInfo().gameData.gameState == GameData::playing){
    startedWalking = false;
  }
}

bool SituationPriorProvider::isLiftedUp(){
  //this is copied from MonteCarloSelfLocator-execute
  bool body_lift_up =  getBodyState().fall_down_state == BodyState::upright && 
                      !getBodyState().standByLeftFoot && 
                      !getBodyState().standByRightFoot && // no foot is on the ground
                       getFrameInfo().getTimeSince(getBodyState().foot_state_time) > 500;//parameters.maxTimeForLiftUp;
  if(body_lift_up){
    wasLiftedUp = true;
  }

  return body_lift_up;
}