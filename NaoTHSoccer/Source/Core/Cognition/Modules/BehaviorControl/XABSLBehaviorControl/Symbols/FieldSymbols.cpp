#include "FieldSymbols.h"

void FieldSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("field.xPosOwnGroundline", &fieldInfo.xPosOwnGroundline );
  engine.registerDecimalInputSymbol("field.xPosOpponentGroundline", &fieldInfo.xPosOpponentGroundline );
  engine.registerDecimalInputSymbol("field.yPosLeftGoal", &fieldInfo.yPosLeftGoalpost );
  engine.registerDecimalInputSymbol("field.yPosRightGoal", &fieldInfo.yPosRightGoalpost );
  engine.registerDecimalInputSymbol("field.xPosOwnPenaltyArea", &fieldInfo.xPosOwnPenaltyArea );
  engine.registerDecimalInputSymbol("field.xPosOppPenaltyArea", &fieldInfo.xPosOpponentPenaltyArea );
  engine.registerDecimalInputSymbol("field.yPosLeftPenaltyArea", &fieldInfo.yPosLeftPenaltyArea );
  engine.registerDecimalInputSymbol("field.yPosRightPenaltyArea", &fieldInfo.yPosRightPenaltyArea );
  engine.registerDecimalInputSymbol("field.centerCircleRadius", &fieldInfo.centerCircleRadius );
}//end registerSymbols



FieldSymbols* FieldSymbols::theInstance = NULL;

void FieldSymbols::execute()
{
}

