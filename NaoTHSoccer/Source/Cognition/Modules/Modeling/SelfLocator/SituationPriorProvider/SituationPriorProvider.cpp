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
  walked_after_penalized_or_init = false;
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
  //std::cout << "CurrentState: " << currentState << std::endl;
  //std::cout << "LastState: " << lastState << std::endl;

  // calculate walked_after_penalized_or_init
  if(currentState == GameData::set || currentState == GameData::initial || currentState == GameData::penalized){
    walked_after_penalized_or_init = false;
  } 
  if(getMotionStatus().currentMotion == motion::walk){
    walked_after_penalized_or_init = true;
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
          || wasLiftedUp)
  {
    //Penalized in Set or Ready for Goalie
    if(getPlayerInfo().gameData.playerNumber == 1){
      //The Goalie will be in the own Goal if manually placed in set
      getSituationPrior().currentPrior = SituationPrior::goaliePenalizedInSet;
    }
    wasLiftedUp = false;
    getSituationPrior().currentPrior = SituationPrior::positionedInSet;
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