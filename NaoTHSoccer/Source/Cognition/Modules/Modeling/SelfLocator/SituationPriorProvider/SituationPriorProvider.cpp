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

  lastGameControllerState = getPlayerInfo().gameData.gameControllerState;
  currentGameControllerState = getPlayerInfo().gameData.gameControllerState;

  walked_after_penalized_or_init = false;
  wasLiftedUp = false;
}

SituationPriorProvider::~SituationPriorProvider()
{
}

void SituationPriorProvider::execute()
{
  //reset prior
  //getSituationPrior().currentPrior = SituationPrior::none;

  //Calculate lastGameState
  if(getPlayerInfo().gameData.gameState != currentState){
    lastState = currentState;
    currentState = getPlayerInfo().gameData.gameState;
  }
  //Calculate lastGameControllerState
  if(getPlayerInfo().gameData.gameControllerState != currentGameControllerState){
    lastGameControllerState = currentGameControllerState;
    currentGameControllerState = getPlayerInfo().gameData.gameControllerState;
  }
  if(currentGameControllerState == GameData::penalized){
    stateBeforePenalized = lastGameControllerState;
  }


  // calculate walked_after_penalized_or_init
  if(currentState == GameData::set || currentState == GameData::initial || currentState == GameData::penalized){
    walked_after_penalized_or_init = false;
  } 
  if(getMotionStatus().currentMotion == motion::walk){
    walked_after_penalized_or_init = true;
    wasLiftedUp = false;
  }
  
  //get BodyState
  if(getBodyState().isLiftedUp){
    wasLiftedUp = true;
  }

  //for penalty kicker
  if(getSituationStatus().oppHalf)
  {
    getSituationPrior().currentPrior = SituationPrior::oppHalf;
  }
  //Init Positions and the robot didn't start walking
  else if(currentState == GameData::ready && lastState == GameData::initial && !walked_after_penalized_or_init)
  {
    getSituationPrior().currentPrior = SituationPrior::firstReady;
  }
  //Positioned in Set, e.g.: Penalized in Set or Ready or manual placement
  else if( (currentState == GameData::set
          && lastState == GameData::penalized)
          || (currentState == GameData::set && wasLiftedUp) )
  {
    //Penalized in Set or Ready for Goalie
    if(getPlayerInfo().gameData.playerNumber == 1){
      //The Goalie will be in the own Goal if manually placed in set
      getSituationPrior().currentPrior = SituationPrior::goaliePenalizedInSet;
    }
    
    getSituationPrior().currentPrior = SituationPrior::positionedInSet;
  }
  else if(stateBeforePenalized == GameData::set && lastGameControllerState == GameData::penalized && currentGameControllerState == GameData::set){
          // && wasLiftedUp
    //should treat Motion in Set Problem
    getSituationPrior().currentPrior = SituationPrior::set;
  }
  else if(stateBeforePenalized == GameData::playing && lastGameControllerState == GameData::penalized && currentGameControllerState == GameData::ready)  {
    getSituationPrior().currentPrior = SituationPrior::playAfterPenalized;
  }
  //Penalized in Play
  else if(currentState == GameData::playing && lastState == GameData::penalized && !walked_after_penalized_or_init)
  {
      getSituationPrior().currentPrior = SituationPrior::playAfterPenalized;
  }
  //Set
  else if(currentState == GameData::set)
  {
    getSituationPrior().currentPrior = SituationPrior::set;
  }
  //Play, Finished
  else
  {
    getSituationPrior().currentPrior = SituationPrior::none;
  }
}