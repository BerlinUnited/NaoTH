/**
* @file FieldInfo.h
* 
* Declaration of class FieldInfo
*
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/ 

#ifndef __FieldInfo_h_
#define __FieldInfo_h_

#include "Tools/Math/Vector2.h"
#include "Core/Tools/LinesTable.h"
#include "Core/Tools/DataStructures/ParameterList.h"
#include "Core/Tools/ColorClasses.h"

class FieldInfo : ParameterList
{
private:
  void calculateValues();

  void createLinesTable();

public:
  FieldInfo();

  //colors
  ColorClasses::Color ballColor;
//  const ColorClasses::Color ownGoalColor        = ColorClasses::yellow;
//  const ColorClasses::Color opponentGoalColor   = ColorClasses::skyblue;

  //////////////// basic values from configuration ////////////////
  double ballRadius;

  // size of the whole field 
  double xFieldLength;
  double yFieldLength;

  // size of the field lines
  double xLength;
  double yLength;

  double xPosOpponentFieldBorder;
  double xPosOpponentGoal;
  double xPosOpponentGoalpost;
  double xPosOpponentGoalpostCenter;
  double xPosOpponentGroundline;
  double xPosOpponentSideCorner;
  double xPosOpponentPenaltyArea;

  double yPosLeftFieldBorder;
  double yPosLeftSideline;
  double yPosLeftGroundline;
  double yPosLeftPenaltyArea;
  double yPosLeftGoal;
  double yPosLeftGoalpostCenter;

  double centerCircleRadius;

  double fieldLinesWidth;
  double goalWidth;
  double goalHeight;
  double goalpostRadius;

  /////////////// pre-calculated values from basic values //////////////
  double xPosHalfWayLine;
  double xPosOwnPenaltyArea;
  double xPosOwnSideCorner;
  double xPosOwnGroundline;
  double xPosOwnGoalpostCenter;
  double xPosOwnGoalpost;
  double xPosOwnGoal;
  double xPosOwnFieldBorder;

  double yPosCenterGoal;
  double yPosRightGoalpostCenter;
  double yPosRightGoal;
  double yPosRightPenaltyArea;
  double yPosRightGroundline;
  double yPosRightSideline;
  double yPosRightFieldBorder;

  //Corner crossings
  Vector2<double> opponentCornerLeft;
  Vector2<double> opponentCornerRight;
  Vector2<double> ownCornerLeft;
  Vector2<double> ownCornerRight;

  Vector2<double> opponentPenaltyCornerLeft;
  Vector2<double> opponentPenaltyCornerRight;
  Vector2<double> ownPenaltyCornerLeft;
  Vector2<double> ownPenaltyCornerRight;

  //T crossings
  Vector2<double> opponentGoalTCrossingLeft;
  Vector2<double> opponentGoalTCrossingRight;
  Vector2<double> ownGoalTCrossingLeft;
  Vector2<double> ownGoalTCrossingRight;
  Vector2<double> centerTCrossingLeft;
  Vector2<double> centerTCrossingRight;

  //Crossing
  Vector2<double> crossingCenterCircleLeft;
  Vector2<double> crossingCenterCircleRight;

  // goal post positions
  Vector2<double> opponentGoalPostLeft;
  Vector2<double> opponentGoalPostRight;
  Vector2<double> opponentGoalCenter;

  Vector2<double> ownGoalPostLeft;
  Vector2<double> ownGoalPostRight;
  Vector2<double> ownGoalCenter;

  LinesTable fieldLinesTable;
};

#endif //__FieldInfo_h_
