/**
 * @file FieldInfo.cpp
 *
 * Implemention of class FieldInfo
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "FieldInfo.h"


FieldInfo::FieldInfo() : ParameterList("FieldInfo")
{
  ballColor = ColorClasses::orange;
  
  // default values as of SPL rules from 2012
  PARAMETER_REGISTER(ballRadius) = 32.5;

  PARAMETER_REGISTER(xLength) = 6000;
  PARAMETER_REGISTER(yLength) = 4000;

  PARAMETER_REGISTER(xFieldLength) = xLength + 2*700;
  PARAMETER_REGISTER(yFieldLength) = yLength + 2*700;

  PARAMETER_REGISTER(xPenaltyAreaLength) = 600;
  PARAMETER_REGISTER(yPenaltyAreaLength) = 2200;

  PARAMETER_REGISTER(centerCircleRadius) = 600;
  PARAMETER_REGISTER(fieldLinesWidth) = 50;

  PARAMETER_REGISTER(goalWidth) = 1400;
  PARAMETER_REGISTER(goalHeight) = 800;
  PARAMETER_REGISTER(goalpostRadius) = 50;
  PARAMETER_REGISTER(xPenaltyMarkDistance) = 1300;
  syncWithConfig();

  calculateValues();

  // calculate the field lines
  fieldLinesTable.create_closestPoinsTable(xFieldLength,yFieldLength);
  fieldLinesTable.create_closestCornerPoinsTable(xFieldLength,yFieldLength);
}

void FieldInfo::calculateValues()
{
  calculateCrossings();
  createLinesTable();
}//end calculateValues

void FieldInfo::calculateCrossings()
{
  xPosHalfWayLine = 0;

  xPosOpponentGroundline =   xLength / 2.0;
  xPosOwnGroundline =       -xPosOpponentGroundline;

  xPosOpponentGoal =         xPosOpponentGroundline;
  xPosOwnGoal =             -xPosOpponentGoal;

  xPosOpponentPenaltyArea =  xPosOpponentGroundline - xPenaltyAreaLength;
  xPosOwnPenaltyArea =      -xPosOpponentPenaltyArea;

  yPosLeftGoalpost =         goalWidth / 2.0;
  yPosRightGoalpost =       -yPosLeftGoalpost;

  yPosLeftPenaltyArea =      yPenaltyAreaLength / 2.0;
  yPosRightPenaltyArea =    -yPosLeftPenaltyArea;
  
  yPosLeftSideline =         yLength / 2.0; 
  yPosRightSideline =       -yPosLeftSideline;


  
  // L crossings
  crossings[opponentCornerLeft].position          = Vector2<double>(xPosOpponentGroundline, yPosLeftSideline);
  crossings[opponentCornerRight].position         = Vector2<double>(xPosOpponentGroundline, yPosRightSideline);
  crossings[ownCornerLeft].position               = Vector2<double>(xPosOwnGroundline, yPosLeftSideline);
  crossings[ownCornerRight].position              = Vector2<double>(xPosOwnGroundline, yPosRightSideline);

  crossings[opponentPenaltyCornerLeft].position   = Vector2<double>(xPosOpponentPenaltyArea, yPosLeftPenaltyArea);
  crossings[opponentPenaltyCornerRight].position  = Vector2<double>(xPosOpponentPenaltyArea, yPosRightPenaltyArea);
  crossings[ownPenaltyCornerLeft].position        = Vector2<double>(xPosOwnPenaltyArea, yPosLeftPenaltyArea);
  crossings[ownPenaltyCornerRight].position       = Vector2<double>(xPosOwnPenaltyArea, yPosRightPenaltyArea);

  // T crossings
  crossings[opponentGoalTCrossingLeft].position   = Vector2<double>(xPosOpponentGroundline, yPosLeftPenaltyArea);
  crossings[opponentGoalTCrossingRight].position  = Vector2<double>(xPosOpponentGroundline, yPosRightPenaltyArea);
  crossings[ownGoalTCrossingLeft].position        = Vector2<double>(xPosOwnGroundline, yPosLeftPenaltyArea);
  crossings[ownGoalTCrossingRight].position       = Vector2<double>(xPosOwnGroundline, yPosRightPenaltyArea);
  crossings[centerTCrossingLeft].position         = Vector2<double>(xPosHalfWayLine, yPosLeftSideline);
  crossings[centerTCrossingRight].position        = Vector2<double>(xPosHalfWayLine, yPosRightSideline);

  // X Crossings
  crossings[crossingCenterCircleLeft].position    = Vector2<double>(xPosHalfWayLine, -centerCircleRadius);
  crossings[crossingCenterCircleRight].position   = Vector2<double>(xPosHalfWayLine, centerCircleRadius);


  // goal post positions
  opponentGoalPostLeft  = Vector2<double>(xPosOpponentGoal, yPosLeftGoalpost);
  opponentGoalPostRight = Vector2<double>(xPosOpponentGoal, yPosRightGoalpost);
  opponentGoalCenter    = Vector2<double>(xPosOpponentGoal, 0.0);

  ownGoalPostLeft   = Vector2<double>(xPosOwnGoal, yPosLeftGoalpost);
  ownGoalPostRight  = Vector2<double>(xPosOwnGoal, yPosRightGoalpost);
  ownGoalCenter     = Vector2<double>(xPosOwnGoal, 0.0);
}//end calculateCrossings


/**

    |---------------|---------------|
    |               |               |
    |---|          _|_          |---|
    |   |        /  |  \        |   |
    |   |   +   (   |   )   +   |   |
    |   |        \ _|_ /        |   |
    |---|           |           |---|
    |               |               |
    |---------------|---------------|

*/

void FieldInfo::createLinesTable()
{
  fieldLinesTable = LinesTable();
  // HACK: remove it
  fieldLinesTable.circle_radius = centerCircleRadius;
  fieldLinesTable.penalty_area_width = xPosOpponentGoal - xPosOpponentPenaltyArea;


  // 0 - right side line - sideLineRight
  fieldLinesTable.addLine(
    crossings[ownCornerRight].position,
    crossings[opponentCornerRight].position
    );

  // 1 - left side line - sideLineLeft
  fieldLinesTable.addLine(
    crossings[ownCornerLeft].position,
    crossings[opponentCornerLeft].position
    );

  // 2 - own goal line - ownGoalLine
  fieldLinesTable.addLine(
    crossings[ownCornerRight].position,
    crossings[ownCornerLeft].position
    );

  // 3 - opponent goal line - opponentGoalLine
  fieldLinesTable.addLine(
    crossings[opponentCornerRight].position,
    crossings[opponentCornerLeft].position
    );

  // 4 - center line - centerLine
  fieldLinesTable.addLine(
    crossings[centerTCrossingRight].position,
    crossings[centerTCrossingLeft].position
    );

  // 5 - own penalty line - ownPenaltyLine
  fieldLinesTable.addLine(
    crossings[ownPenaltyCornerRight].position,
    crossings[ownPenaltyCornerLeft].position
    );

  // 6 - own penalty right line - ownPenaltyLineRight
  fieldLinesTable.addLine(
    crossings[ownGoalTCrossingRight].position,
    crossings[ownPenaltyCornerRight].position
    );

  // 7 - own penalty left line - ownPenaltyLineLeft
  fieldLinesTable.addLine(
    crossings[ownGoalTCrossingLeft].position,
    crossings[ownPenaltyCornerLeft].position
    );

  // 8 - opponent penalty line - opponentPenaltyLine
  fieldLinesTable.addLine(
    crossings[opponentPenaltyCornerRight].position,
    crossings[opponentPenaltyCornerLeft].position
    );

  // 9 - opponent penalty right line - opponentPenaltyLineRight
  fieldLinesTable.addLine(
    crossings[opponentPenaltyCornerRight].position,
    crossings[opponentGoalTCrossingRight].position
    );

  // 10 - opponent penalty left line - opponentPenaltyLineLeft
  fieldLinesTable.addLine(
    crossings[opponentPenaltyCornerLeft].position,
    crossings[opponentGoalTCrossingLeft].position
    );

  // center circle approximated by sequence of lines
  double numberOfSegments = 12;
  double angleStep = Math::pi2 / numberOfSegments;

  // the radius is choosen in a way the centers of line segments are exactly on the circle
  Vector2<double> x0(centerCircleRadius / cos(angleStep*0.5), 0.0);
  x0.rotate(-angleStep * 0.5); // the middle line should be intersected by only one circle line
  Vector2<double> x1(x0);

  for (int i = 0; i < numberOfSegments; i++)
  {
    x0.rotate(angleStep);
    fieldLinesTable.addLine(x0, x1);
    x1 = x0;
  }//end for

  fieldLinesTable.findIntersections();
}//end createLinesTable

void FieldInfo::print(std::ostream& stream) const
{
  stream << "ballColor = "<< ColorClasses::getColorName( ballColor )<<"\n";

  stream << "//////////////// basic values from configuration ////////////////\n";
  stream << "ballRadius = "<< ballRadius <<"\n";

  stream << "size of the whole field (including the green area outside the lines): \n";
  stream << "xFieldLength = "<< xFieldLength <<"\n";
  stream << "yFieldLength = "<< yFieldLength <<"\n";

  stream << "size of the field lines:\n";
  stream << "xLength = "<< xLength <<"\n";
  stream << "yLength = "<< yLength <<"\n";

  stream << "xPosOpponentPenaltyArea = "<< xPosOpponentPenaltyArea <<"\n";
  stream << "yPosLeftPenaltyArea = "<< yPosLeftPenaltyArea <<"\n";

  stream << "centerCircleRadius = "<< centerCircleRadius <<"\n";
  stream << "fieldLinesWidth = "<< fieldLinesWidth <<"\n";
  stream << "goalWidth = "<< goalWidth <<"\n";
  stream << "goalHeight = "<< goalHeight <<"\n";
  stream << "goalpostRadius = "<< goalpostRadius <<"\n";

  stream << "/////////////// pre-calculated values from basic values //////////////\n";
  stream << "xPosHalfWayLine = "<< xPosHalfWayLine <<"\n";

  stream << "xPosOwnGoal = "<< xPosOwnGoal <<"\n";
  stream << "xPosOpponentGoal = "<< xPosOpponentGoal <<"\n";
  stream << "xPosOwnGroundline = "<< xPosOwnGroundline <<"\n";
  stream << "xPosOpponentGroundline = "<< xPosOpponentGroundline <<"\n";
  stream << "xPosOwnPenaltyArea = "<< xPosOwnPenaltyArea <<"\n";

  stream << "yPosLeftSideline = "<< yPosLeftSideline <<"\n";
  stream << "yPosRightSideline = "<< yPosRightSideline <<"\n";
  stream << "yPosRightPenaltyArea = "<< yPosRightPenaltyArea <<"\n";

  stream << "yPosRightGoalpost = "<< yPosRightGoalpost <<"\n";
  stream << "yPosLeftGoalpost = "<< yPosLeftGoalpost <<"\n";
}
