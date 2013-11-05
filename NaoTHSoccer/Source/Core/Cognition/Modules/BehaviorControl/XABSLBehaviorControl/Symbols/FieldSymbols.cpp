/**
 * @file FieldSymbols.cpp
 *
 * Implementation of class FieldSymbols
 *
 *@author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Benjamin</a>
 * Currently not used
 * Experimental feature
 *
 *Should be used soon
 */

#include "FieldSymbols.h"

void FieldSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("field.xPosOwnGroundline", &fieldInfo.xPosOwnGroundline);
  engine.registerDecimalInputSymbol("field.xPosOpponentGroundline", &fieldInfo.xPosOpponentGroundline);
  engine.registerDecimalInputSymbol("field.yPosLeftGoal", &fieldInfo.yPosLeftGoalpost);
  engine.registerDecimalInputSymbol("field.yPosRightGoal", &fieldInfo.yPosRightGoalpost );
  engine.registerDecimalInputSymbol("field.xPosOwnPenaltyArea", &fieldInfo.xPosOwnPenaltyArea );
  engine.registerDecimalInputSymbol("field.xPosOppPenaltyArea", &fieldInfo.xPosOpponentPenaltyArea );
  engine.registerDecimalInputSymbol("field.yPosLeftPenaltyArea", &fieldInfo.yPosLeftPenaltyArea );
  engine.registerDecimalInputSymbol("field.yPosRightPenaltyArea", &fieldInfo.yPosRightPenaltyArea );
  engine.registerDecimalInputSymbol("field.centerCircleRadius", &fieldInfo.centerCircleRadius );

   engine.registerDecimalInputSymbol("field.xPosFirstDefenseline", &xPosFirstDefenseline);
   engine.registerDecimalInputSymbol("field.xPosSecondDefenseline", &xPosSecondDefenseline);
   engine.registerDecimalInputSymbol("field.yPosLeftFlank", &yPosLeftFlank);
   engine.registerDecimalInputSymbol("field.yPosRightFlank", &yPosRightFlank);
   engine.registerDecimalInputSymbol("field.xPosFirstAttackline", &xPosFirstAttackline);
   engine.registerDecimalInputSymbol("field.xPosSecondAttackline", &xPosSecondAttackline);

   engine.registerDecimalInputSymbol("field.yPosLeftSideline", &fieldInfo.yPosLeftSideline);
   engine.registerDecimalInputSymbol("field.yPosRightSideline", &fieldInfo.yPosRightSideline);
}//end registerSymbols



FieldSymbols* FieldSymbols::theInstance = NULL;

void FieldSymbols::execute()
{
	 
}
double FieldSymbols::xPosFirstDefenseline(){
	return theInstance->getFieldInfo().xPosOwnGroundline/3; //immer minus
}
/**

    |---------------|---------------|
    |         |     |               |
    |---|     |    _|_          |---|
    |   |     |  /  |  \        |   |
    |   |   + | (   |   )   +   |   |
    |   |     |  \ _|_ /        |   |
    |---|     |     |           |---|
    |         |     |               |
    |---------------|---------------|

*/
double FieldSymbols::xPosSecondDefenseline(){
	return theInstance->getFieldInfo().xPosOwnGroundline*2/3; //immer minus
}
/**

    |---------------|---------------|
    |     |         |               |
    |---| |        _|_          |---|
    |   | |      /  |  \        |   |
    |   | | +   (   |   )   +   |   |
    |   | |      \ _|_ /        |   |
    |---| |         |           |---|
    |     |         |               |
    |---------------|---------------|

*/
double FieldSymbols::yPosLeftFlank(){
	return theInstance->getFieldInfo().yPosLeftSideline*6/10;
}
/**

    |---------------|---------------|
    |---------------|---------------|
    |---|          _|_          |---|
    |   |        /  |  \        |   |
    |   |   +   (   |   )   +   |   |
    |   |        \ _|_ /        |   |
    |---|           |           |---|
    |               |               |
    |---------------|---------------|

*/

double FieldSymbols::yPosRightFlank(){
	return theInstance->getFieldInfo().yPosRightSideline*6/10;
}
/**

    |---------------|---------------|
    |               |               |
    |---|          _|_          |---|
    |   |        /  |  \        |   |
    |   |   +   (   |   )   +   |   |
    |   |        \ _|_ /        |   |
    |---|           |           |---|
    |---------------|---------------|
    |---------------|---------------|

*/

double FieldSymbols::xPosFirstAttackline(){
	return -xPosFirstDefenseline(); //ist positive
}
double FieldSymbols::xPosSecondAttackline(){
	return -xPosSecondDefenseline(); //ist positive
}
