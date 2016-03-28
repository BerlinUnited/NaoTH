/**
* @file SituationPriorProvider.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class SituationPriorProvider
*/

#include "SituationPriorProvider.h"

SituationPriorProvider::SituationPriorProvider()
{
  // debug
  DEBUG_REQUEST_REGISTER("SPP:drawPriors", "drawPriors", false);  
  
  lastState = getPlayerInfo().gameData.gameState;
  currentState = getPlayerInfo().gameData.gameState;

  startedWalking = false;
}

SituationPriorProvider::~SituationPriorProvider()
{
}

void SituationPriorProvider::execute()
{
  reset();

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
  //Penalized in Set or Ready
  else if(getPlayerInfo().gameData.gameState == GameData::set && lastState == GameData::penalized)
  {
    getSituationPrior().currentPrior = getSituationPrior().penalizedInSet;
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

  DEBUG_REQUEST("SPP:drawPriors",
    drawPriors();
  );
}

void SituationPriorProvider::reset(){
  //Reset the booleans
  //Priors will no longer be active after the robot started walking
  if(getPlayerInfo().gameData.gameState == GameData::initial || getPlayerInfo().gameData.gameState == GameData::playing){
    startedWalking = false;
  }
}

void SituationPriorProvider::drawPriors(){
  if(getSituationPrior().currentPrior == getSituationPrior().none)
  {
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = getFieldInfo().fieldRect.min();
    const Vector2d& fieldMax = getFieldInfo().fieldRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  }
  else if(getSituationPrior().currentPrior == getSituationPrior().firstReady)
  {
     //Todo: make this Parameters again
    double startPositionsSigmaDistance = 500;
    double startPositionsSigmaAngle = 0.5;

    double offserY = 0;
    Vector2d startLeft(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yLength/2.0 - offserY);
    Vector2d endLeft(                               -500, getFieldInfo().yLength/2.0 - offserY);

    Vector2d startRight(startLeft.x, -startLeft.y);
    Vector2d endRight(endLeft.x, -endLeft.y);

    LineDensity leftStartingLine(startLeft, endLeft, -Math::pi_2, startPositionsSigmaDistance, startPositionsSigmaAngle);
    LineDensity rightStartingLine(startRight, endRight, Math::pi_2, startPositionsSigmaDistance, startPositionsSigmaAngle);

    //  for(size_t i = 0; i < sampleSet.size(); i++) {
    //    if(sampleSet[i].translation.y > 0) {
    //      sampleSet[i].likelihood *= leftStartingLine.update(sampleSet[i]);
    //    } else {
    //      sampleSet[i].likelihood *= rightStartingLine.update(sampleSet[i]);
    //    }
    //  }

    /*---- HACK BEGIN ----*/
    LineDensity startingLine;
    FIELD_DRAWING_CONTEXT;
    if(getPlayerInfo().gameData.playerNumber < 4) {
        startingLine = leftStartingLine;
        leftStartingLine.draw(getDebugDrawings());
    } else {
        startingLine = rightStartingLine;
        rightStartingLine.draw(getDebugDrawings());
    }

    /*---- HACK END ----*/

    
    
    

  }
  else if(getSituationPrior().currentPrior == getSituationPrior().penalizedInSet)
  {
    FIELD_DRAWING_CONTEXT;
    PEN("ff0000", 30);
    const Vector2d& fieldMin = getFieldInfo().ownHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().ownHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);

  }
  else if(getSituationPrior().currentPrior == getSituationPrior().goaliePenalizedInSet)
  {
    static const Geometry::Rect2d ownGoalBox(
    Vector2d(getFieldInfo().xPosOwnGroundline, getFieldInfo().yPosRightPenaltyArea) - Vector2d(200, 200), 
    Vector2d(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yPosLeftPenaltyArea) + Vector2d(200, 200));

    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = ownGoalBox.min();
    const Vector2d& fieldMax = ownGoalBox.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);

  }
  else if(getSituationPrior().currentPrior == getSituationPrior().set)
  {
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = getFieldInfo().ownHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().ownHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);

  }
  else if(getSituationPrior().currentPrior == getSituationPrior().playAfterPenalized)
  {
    //Todo: make this Parameters again
    double startPositionsSigmaDistance = 500;
    double startPositionsSigmaAngle = 0.5;

    double offserY = 0;
    Vector2d startLeft(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yLength/2.0 - offserY);
    Vector2d endLeft(                               -500, getFieldInfo().yLength/2.0 - offserY);

    Vector2d startRight(startLeft.x, -startLeft.y);
    Vector2d endRight(endLeft.x, -endLeft.y);

    LineDensity leftStartingLine(startLeft, endLeft, -Math::pi_2, startPositionsSigmaDistance, startPositionsSigmaAngle);
    LineDensity rightStartingLine(startRight, endRight, Math::pi_2, startPositionsSigmaDistance, startPositionsSigmaAngle);

    /*---- HACK BEGIN ----*/
    //Todo: should not be devided according to player number BUG
    LineDensity startingLine;
    if(getPlayerInfo().gameData.playerNumber < 4) {
        startingLine = leftStartingLine;
    } else {
        startingLine = rightStartingLine;
    }
    /*---- HACK END ----*/

    FIELD_DRAWING_CONTEXT;
    leftStartingLine.draw(getDebugDrawings());
    rightStartingLine.draw(getDebugDrawings());

  }
  else if(getSituationPrior().currentPrior == getSituationPrior().oppHalf)
  {
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = getFieldInfo().oppHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().oppHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  }
}

