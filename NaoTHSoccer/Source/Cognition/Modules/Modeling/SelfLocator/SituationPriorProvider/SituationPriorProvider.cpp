/**
* @file SituationPriorProvider.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class SituationPriorProvider
*/

#include "SituationPriorProvider.h"

// tools
#include "Tools/Math/Probabilistics.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "../MonteCarloSelfLocator/Tools.h"

using namespace std;
using namespace mcsl;

SituationPriorProvider::SituationPriorProvider()
{
  // debug
  //DEBUG_REQUEST_REGISTER("SPP:updateByOwnHalf", "updateByOwnHalf", false);
  //DEBUG_REQUEST_REGISTER("SPP:updateByOppHalf", "updateByOppHalf", false);
  //DEBUG_REQUEST_REGISTER("SPP:updateByOwnHalfLookingForward", "updateByOwnHalfLookingForward", false);
  //DEBUG_REQUEST_REGISTER("SPP:updateByGoalBox", "updateByGoalBox", false);
  //DEBUG_REQUEST_REGISTER("SPP:updateByStartPositions", "updateByStartPositions", false);
  //DEBUG_REQUEST_REGISTER("SPP:updateAfterPenalize", "updateAfterPenalize", false);  
  
  lastState = getPlayerInfo().gameData.gameState;
  currentState = getPlayerInfo().gameData.gameState;

  startedWalking = false;
  startedWalking2 = false;
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
    updateByOppHalf();
  }

  //Init Positions
  else if(getPlayerInfo().gameData.gameState == GameData::ready && lastState == GameData::initial)
  {
    //Dont update afer the robot started walking
    if(getMotionStatus().currentMotion == motion::walk && startedWalking == false)
    {
      startedWalking = true;
    }
    if(startedWalking == false){
      updateByStartPositions();
    }
  }
  //Penalized in Set or Ready
  else if(getPlayerInfo().gameData.gameState == GameData::set && lastState == GameData::penalized)
  {
      updateByOwnHalfLookingForward();  
  }
  //Penalized in Set or Ready for Goalie
  else if(getPlayerInfo().gameData.playerNumber == 1 && getPlayerInfo().gameData.gameState == GameData::set && lastState == GameData::penalized){
    //The Goalie will be in the own Goal if manually placed in set
    updateByGoalBox();
  }
  //Penalized in Play
  else if(getPlayerInfo().gameData.gameState == GameData::penalized)
  {
    //Dont update afer the robot started walking
    if(getMotionStatus().currentMotion == motion::walk && startedWalking2 == false)
    {
      startedWalking2 = true;
    }
    if(startedWalking2 == false){
      updateAfterPenalize();
    }
  }
  //Init, Set, Play, Finished
  else
  {
    updateByOwnHalf();
  }
}

void SituationPriorProvider::updateAfterPenalize(){
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

  //for(size_t i = 0; i < sampleSet.size(); i++) {
  //    sampleSet[i].likelihood *= startingLine.update(sampleSet[i]);
  //}

  //DEBUG_REQUEST("SPP:updateAfterPenalize",
    FIELD_DRAWING_CONTEXT;
    leftStartingLine.draw(getDebugDrawings());
    rightStartingLine.draw(getDebugDrawings());
  //);
}

void SituationPriorProvider::updateByOwnHalfLookingForward()
{
  //DEBUG_REQUEST("SPP:updateByOwnHalfLookingForward",
    FIELD_DRAWING_CONTEXT;
    PEN("ff0000", 30);
    const Vector2d& fieldMin = getFieldInfo().ownHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().ownHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  //);
}

void SituationPriorProvider::updateByOwnHalf()
{

  //DEBUG_REQUEST("SPP:updateByOwnHalf",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = getFieldInfo().ownHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().ownHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  //);
}

void SituationPriorProvider::updateByOppHalf()
{

  //DEBUG_REQUEST("SPP:updateByOppHalf",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = getFieldInfo().oppHalfRect.min();
    const Vector2d& fieldMax = getFieldInfo().oppHalfRect.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  //);
}

void SituationPriorProvider::updateByGoalBox()
{
  static const Geometry::Rect2d ownGoalBox(
    Vector2d(getFieldInfo().xPosOwnGroundline, getFieldInfo().yPosRightPenaltyArea) - Vector2d(200, 200), 
    Vector2d(getFieldInfo().xPosOwnPenaltyArea, getFieldInfo().yPosLeftPenaltyArea) + Vector2d(200, 200));

  //DEBUG_REQUEST("SPP:updateByGoalBox",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 30);
    const Vector2d& fieldMin = ownGoalBox.min();
    const Vector2d& fieldMax = ownGoalBox.max();
    BOX(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMin.y, fieldMax.x, fieldMax.y);
    LINE(fieldMin.x, fieldMax.y, fieldMax.x, fieldMin.y);
  //);
}

void SituationPriorProvider::updateByStartPositions()
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
  if(getPlayerInfo().gameData.playerNumber < 4) {
      startingLine = leftStartingLine;
  } else {
      startingLine = rightStartingLine;
  }

  //for(size_t i = 0; i < sampleSet.size(); i++) {
  //    sampleSet[i].likelihood *= startingLine.update(sampleSet[i]);
  //}
  /*---- HACK END ----*/

  //DEBUG_REQUEST("SPP:updateByStartPositions",
    FIELD_DRAWING_CONTEXT;
    leftStartingLine.draw(getDebugDrawings());
    rightStartingLine.draw(getDebugDrawings());
  //);
}

void SituationPriorProvider::reset(){
  //Reset the booleans
  if(getPlayerInfo().gameData.gameState == GameData::initial){
    startedWalking = false;
  }
  if(getPlayerInfo().gameData.gameState == GameData::playing){
    startedWalking2 = false;
  }
}