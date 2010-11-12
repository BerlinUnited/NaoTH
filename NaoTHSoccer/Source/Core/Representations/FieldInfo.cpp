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
  
  PARAMETER_REGISTER(ballRadius);

  PARAMETER_REGISTER(xFieldLength);
  PARAMETER_REGISTER(yFieldLength);
  PARAMETER_REGISTER(xLength);
  PARAMETER_REGISTER(yLength);

  PARAMETER_REGISTER(xPosOpponentFieldBorder);
  PARAMETER_REGISTER(xPosOpponentGoal);
  PARAMETER_REGISTER(xPosOpponentGoalpost);
  PARAMETER_REGISTER(xPosOpponentGoalpostCenter);
  PARAMETER_REGISTER(xPosOpponentGroundline);
  PARAMETER_REGISTER(xPosOpponentSideCorner);
  PARAMETER_REGISTER(xPosOpponentPenaltyArea);

  PARAMETER_REGISTER(yPosLeftFieldBorder);
  PARAMETER_REGISTER(yPosLeftSideline);
  PARAMETER_REGISTER(yPosLeftGroundline);
  PARAMETER_REGISTER(yPosLeftPenaltyArea);
  PARAMETER_REGISTER(yPosLeftGoal);
  PARAMETER_REGISTER(yPosLeftGoalpostCenter);

  PARAMETER_REGISTER(centerCircleRadius);

  PARAMETER_REGISTER(fieldLinesWidth);
  PARAMETER_REGISTER(goalWidth);
  PARAMETER_REGISTER(goalHeight);
  PARAMETER_REGISTER(goalpostRadius);

  initParams();

  calculateValues();
}

void FieldInfo::calculateValues()
{
  xPosHalfWayLine = 0;
  xPosOwnPenaltyArea = -xPosOpponentPenaltyArea;
  xPosOwnSideCorner = -xPosOpponentSideCorner;
  xPosOwnGroundline = -xPosOpponentGroundline;
  xPosOwnGoalpostCenter = -xPosOpponentGoalpostCenter;
  xPosOwnGoalpost = -xPosOpponentGoalpost;
  xPosOwnGoal = -xPosOpponentGoal;
  xPosOwnFieldBorder = -xPosOpponentFieldBorder;

  yPosCenterGoal = 0;
  yPosRightGoalpostCenter = -yPosLeftGoalpostCenter;
  yPosRightGoal = -yPosLeftGoal;
  yPosRightPenaltyArea = -yPosLeftPenaltyArea;
  yPosRightGroundline = -yPosLeftGroundline;
  yPosRightSideline = -yPosLeftSideline;
  yPosRightFieldBorder = -yPosLeftFieldBorder;

  //Corner crossings
  opponentCornerLeft = Vector2<double>(xPosOpponentGroundline, yPosLeftGroundline);
  opponentCornerRight = Vector2<double>(xPosOpponentGroundline, yPosRightGroundline);
  ownCornerLeft = Vector2<double>(xPosOwnGroundline, yPosLeftGroundline);
  ownCornerRight = Vector2<double>(xPosOwnGroundline, yPosRightGroundline);

  opponentPenaltyCornerLeft = Vector2<double>(xPosOpponentPenaltyArea, yPosLeftPenaltyArea);
  opponentPenaltyCornerRight = Vector2<double>(xPosOpponentPenaltyArea, yPosRightPenaltyArea);
  ownPenaltyCornerLeft = Vector2<double>(xPosOwnPenaltyArea, yPosLeftPenaltyArea);
  ownPenaltyCornerRight = Vector2<double>(xPosOwnPenaltyArea, yPosRightPenaltyArea);

  //T crossings
  opponentGoalTCrossingLeft = Vector2<double>(xPosOpponentGroundline, yPosLeftPenaltyArea);
  opponentGoalTCrossingRight = Vector2<double>(xPosOpponentGroundline, yPosRightPenaltyArea);
  ownGoalTCrossingLeft = Vector2<double>(xPosOwnGroundline, yPosLeftPenaltyArea);
  ownGoalTCrossingRight = Vector2<double>(xPosOwnGroundline, yPosRightPenaltyArea);
  centerTCrossingLeft = Vector2<double>(xPosHalfWayLine, yPosLeftGroundline);
  centerTCrossingRight = Vector2<double>(xPosHalfWayLine, yPosRightGroundline);

  //Crossing
  crossingCenterCircleLeft = Vector2<double>(xPosHalfWayLine, -centerCircleRadius);
  crossingCenterCircleRight = Vector2<double>(xPosHalfWayLine, centerCircleRadius);

  // goal post positions
  opponentGoalPostLeft = Vector2<double>(xPosOpponentGoal, yPosLeftGoal);
  opponentGoalPostRight = Vector2<double>(xPosOpponentGoal, yPosRightGoal);
  opponentGoalCenter = Vector2<double>(xPosOpponentGoal, yPosCenterGoal);

  ownGoalPostLeft = Vector2<double>(xPosOwnGoal, yPosRightGoal);
  ownGoalPostRight = Vector2<double>(xPosOwnGoal, yPosLeftGoal);
  ownGoalCenter = Vector2<double>(xPosOwnGoal, yPosCenterGoal);

  createLinesTable();
}

void FieldInfo::createLinesTable()
{
  fieldLinesTable = LinesTable();

  // bottom side line
  fieldLinesTable.push(
    Vector2<double>(xPosOwnGroundline, yPosRightGroundline),
    Vector2<double>(xPosOpponentGroundline, yPosRightGroundline)
    );

  // top side line
  fieldLinesTable.push(
    Vector2<double>(xPosOwnGroundline, yPosLeftGroundline),
    Vector2<double>(xPosOpponentGroundline, yPosLeftGroundline)
    );

  // right side line
  fieldLinesTable.push(
    Vector2<double>(xPosOwnGroundline, yPosRightGroundline),
    Vector2<double>(xPosOwnGroundline, yPosLeftGroundline)
    );

  // right side line
  fieldLinesTable.push(
    Vector2<double>(xPosOpponentGroundline, yPosRightGroundline),
    Vector2<double>(xPosOpponentGroundline, yPosLeftGroundline)
    );

  // center line
  fieldLinesTable.push(
    Vector2<double>(xPosHalfWayLine, yPosRightGroundline),
    Vector2<double>(xPosHalfWayLine, yPosLeftGroundline)
    );

  // blue front goal line
  fieldLinesTable.push(
    Vector2<double>(xPosOwnPenaltyArea, yPosRightPenaltyArea),
    Vector2<double>(xPosOwnPenaltyArea, yPosLeftPenaltyArea)
    );

  // blue bottom goal line
  fieldLinesTable.push(
    Vector2<double>(xPosOwnGroundline, yPosRightPenaltyArea),
    Vector2<double>(xPosOwnPenaltyArea, yPosRightPenaltyArea)
    );

  // blue top goal line
  fieldLinesTable.push(
    Vector2<double>(xPosOwnGroundline, yPosLeftPenaltyArea),
    Vector2<double>(xPosOwnPenaltyArea, yPosLeftPenaltyArea)
    );

  // yellow front goal line
  fieldLinesTable.push(
    Vector2<double>(xPosOpponentPenaltyArea, yPosRightPenaltyArea),
    Vector2<double>(xPosOpponentPenaltyArea, yPosLeftPenaltyArea)
    );

  // yellow bottom goal line
  fieldLinesTable.push(
    Vector2<double>(xPosOpponentPenaltyArea, yPosRightPenaltyArea),
    Vector2<double>(xPosOpponentGroundline, yPosRightPenaltyArea)
    );

  // yellow top goal line
  fieldLinesTable.push(
    Vector2<double>(xPosOpponentPenaltyArea, yPosLeftPenaltyArea),
    Vector2<double>(xPosOpponentGroundline, yPosLeftPenaltyArea)
    );

  // center circle approximated by sequence of lines
  double numberOfSegments = 12;
  double angleStep = Math::pi2 / numberOfSegments;
  Vector2<double> x0(centerCircleRadius, 0.0);
  x0.rotate(-angleStep * 0.5); // the middle line should be intersected by only one circle line
  Vector2<double> x1(x0);

  for (int i = 0; i < numberOfSegments; i++)
  {
    x0.rotate(angleStep);
    fieldLinesTable.push(x0, x1);
    x1 = x0;
  }//end for

  fieldLinesTable.findIntersections();
}//end createLinesTable
