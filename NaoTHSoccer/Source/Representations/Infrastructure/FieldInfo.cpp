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
  PARAMETER_REGISTER(ballRadius) = 50.0;

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
  PARAMETER_REGISTER(goalDepth) = 500;
  PARAMETER_REGISTER(goalpostRadius) = 50;
  PARAMETER_REGISTER(xPenaltyMarkDistance) = 1300;

  PARAMETER_REGISTER(goalBoxAsLines) = false;

  PARAMETER_REGISTER(enable_arcs_at_penalty_box) = false;
  PARAMETER_REGISTER(center_x_in_own_half) = - xLength / 2.0 + xPenaltyAreaLength; ;
  PARAMETER_REGISTER(radius) = yPenaltyAreaLength / 4;
  PARAMETER_REGISTER(start_angle) = -Math::pi_2;
  PARAMETER_REGISTER(target_angle) = Math::pi_2;

  syncWithConfig();

  calculateValues();

  // calculate the field lines
  fieldLinesTable.create_closestPointsTable(xFieldLength,yFieldLength);
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

  //Extra stuff for the Simulator
  xThrowInLineOwn = xPosOwnGroundline+1000;
  xThrowInLineOpp = -xThrowInLineOwn;

  yThrowInLineLeft = yPosLeftSideline-400;
  yThrowInLineRight = -yThrowInLineLeft;
  //Calculate Points
  leftThrowInPointOwn = Vector2d(xThrowInLineOwn,yThrowInLineLeft);
  leftThrowInPointOpp = Vector2d(xThrowInLineOpp,yThrowInLineLeft);

  rightThrowInPointOwn = Vector2d(xThrowInLineOwn,yThrowInLineRight);  
  rightThrowInPointOpp = Vector2d(xThrowInLineOpp,yThrowInLineRight);

  // L crossings
  crossings[opponentCornerLeft].position          = Vector2d(xPosOpponentGroundline, yPosLeftSideline);
  crossings[opponentCornerRight].position         = Vector2d(xPosOpponentGroundline, yPosRightSideline);
  crossings[ownCornerLeft].position               = Vector2d(xPosOwnGroundline, yPosLeftSideline);
  crossings[ownCornerRight].position              = Vector2d(xPosOwnGroundline, yPosRightSideline);

  crossings[opponentPenaltyCornerLeft].position   = Vector2d(xPosOpponentPenaltyArea, yPosLeftPenaltyArea);
  crossings[opponentPenaltyCornerRight].position  = Vector2d(xPosOpponentPenaltyArea, yPosRightPenaltyArea);
  crossings[ownPenaltyCornerLeft].position        = Vector2d(xPosOwnPenaltyArea, yPosLeftPenaltyArea);
  crossings[ownPenaltyCornerRight].position       = Vector2d(xPosOwnPenaltyArea, yPosRightPenaltyArea);

  // T crossings
  crossings[opponentGoalTCrossingLeft].position   = Vector2d(xPosOpponentGroundline, yPosLeftPenaltyArea);
  crossings[opponentGoalTCrossingRight].position  = Vector2d(xPosOpponentGroundline, yPosRightPenaltyArea);
  crossings[ownGoalTCrossingLeft].position        = Vector2d(xPosOwnGroundline, yPosLeftPenaltyArea);
  crossings[ownGoalTCrossingRight].position       = Vector2d(xPosOwnGroundline, yPosRightPenaltyArea);
  crossings[centerTCrossingLeft].position         = Vector2d(xPosHalfWayLine, yPosLeftSideline);
  crossings[centerTCrossingRight].position        = Vector2d(xPosHalfWayLine, yPosRightSideline);

  // X Crossings
  crossings[crossingCenterCircleLeft].position    = Vector2d(xPosHalfWayLine, -centerCircleRadius);
  crossings[crossingCenterCircleRight].position   = Vector2d(xPosHalfWayLine, centerCircleRadius);


  // goal post positions
  opponentGoalPostLeft  = Vector2d(xPosOpponentGoal+25, yPosLeftGoalpost);
  opponentGoalPostRight = Vector2d(xPosOpponentGoal+25, yPosRightGoalpost);
  opponentGoalCenter    = Vector2d(xPosOpponentGoal+25, 0.0);

  ownGoalPostLeft   = Vector2d(xPosOwnGoal-25, yPosLeftGoalpost);
  ownGoalPostRight  = Vector2d(xPosOwnGoal-25, yPosRightGoalpost);
  ownGoalCenter     = Vector2d(xPosOwnGoal-25, 0.0);

  carpetRect = Geometry::Rect2d(Vector2d(-xFieldLength*0.5, -yFieldLength*0.5), Vector2d(xFieldLength*0.5, yFieldLength*0.5));

  // HACK: assymmetric field
  bdrCarpetRect = Geometry::Rect2d(Vector2d(-xFieldLength*0.5, -yFieldLength*0.5), Vector2d(xFieldLength*0.5, yFieldLength*0.5 + 60));

  ownHalfRect = Geometry::Rect2d(Vector2d(-xLength*0.5, -yLength*0.5), Vector2d(0, yLength*0.5));
  fieldRect = Geometry::Rect2d(Vector2d(-xLength*0.5, -yLength*0.5), Vector2d(xLength*0.5, yLength*0.5));
  oppHalfRect = Geometry::Rect2d(Vector2d(0, -yLength*0.5), Vector2d(xLength*0.5, yLength*0.5));


  ownGoalBackLeft = Vector2d(ownGoalPostLeft.x - goalDepth, ownGoalPostLeft.y);
  ownGoalBackRight = Vector2d(ownGoalPostRight.x - goalDepth, ownGoalPostRight.y);  
  ownGoalRect = Geometry::Rect2d(ownGoalBackRight, ownGoalPostLeft); 

  oppGoalBackLeft = Vector2d(opponentGoalPostLeft.x + goalDepth, opponentGoalPostLeft.y);
  oppGoalBackRight = Vector2d(opponentGoalPostRight.x + goalDepth, opponentGoalPostRight.y);
  oppGoalRect = Geometry::Rect2d(oppGoalBackRight, opponentGoalPostLeft);



  ////Should be done differently
  // 0 - right side line - sideLineRight
  rightLineSegment = Math::LineSegment(
    crossings[ownCornerRight].position,
    crossings[opponentCornerRight].position
    );
  
  // 1 - left side line - sideLineLeft
  leftLineSegment = Math::LineSegment(
    crossings[ownCornerLeft].position,
    crossings[opponentCornerLeft].position
    );

  // 2 - own goal line - ownGoalLine
  ownLineSegment = Math::LineSegment(
    crossings[ownCornerRight].position,
    crossings[ownCornerLeft].position
    );

  // 3 - opponent goal line - opponentGoalLine
  oppLineSegment = Math::LineSegment(
    crossings[opponentCornerRight].position,
    crossings[opponentCornerLeft].position
    );
  ////
  //Extra Stuff end
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

  //

  // HACK: RC14, RC15 white goal box is recognized as lines (opp goal)
  if(goalBoxAsLines)
  {
    fieldLinesTable.addLine(
      Vector2d(opponentGoalPostLeft.x-25,opponentGoalPostLeft.y),
      Vector2d(opponentGoalPostLeft.x + goalDepth, opponentGoalPostLeft.y)
      );
    fieldLinesTable.addLine(
      Vector2d(opponentGoalPostRight.x-25,opponentGoalPostRight.y),
      Vector2d(opponentGoalPostRight.x + goalDepth, opponentGoalPostRight.y)
      );
    fieldLinesTable.addLine(
		oppGoalBackLeft,
		oppGoalBackRight
      );

    fieldLinesTable.addLine(
      Vector2d(ownGoalPostLeft.x+25,ownGoalPostLeft.y),
      Vector2d(ownGoalPostLeft.x - goalDepth, ownGoalPostLeft.y)
      );
    fieldLinesTable.addLine(
      Vector2d(ownGoalPostRight.x+25,ownGoalPostRight.y),
      Vector2d(ownGoalPostRight.x - goalDepth, ownGoalPostRight.y)
      );
    fieldLinesTable.addLine(
      ownGoalBackLeft,
      ownGoalBackRight
      );
  }

  // center circle approximated by sequence of lines
  double numberOfSegments = 12;
  double angleStep = Math::pi2 / numberOfSegments;

  // the radius is choosen in a way the centers of line segments are exactly on the circle
  Vector2d x0(centerCircleRadius / cos(angleStep*0.5), 0.0);
  x0.rotate(-angleStep * 0.5); // the middle line should be intersected by only one circle line
  Vector2d x1(x0);

  for (int i = 0; i < numberOfSegments; i++)
  {
    x0.rotate(angleStep);
    fieldLinesTable.addLine(x0, x1);
    x1 = x0;
  }//end for

  if(enable_arcs_at_penalty_box){
      // the arcs are approximated by line segments
      // starting from the start_angle going anti-clock-wise to the target angle
      Vector2d center(center_x_in_own_half,0);
      angleStep = (target_angle - start_angle)/numberOfSegments;

      x0 = Vector2d(radius,0);
      x0.rotate(start_angle);
      x1 = x0;
      Vector2d start, end;

      for (int i = 0; i < numberOfSegments; i++)
      {
        x0.rotate(angleStep);
        start = center + x1;
        end   = center + x0;
        fieldLinesTable.addLine(start, end);
        // mirror for opp penalty box
        start.x = -start.x;
        end.x   = -end.x;
        fieldLinesTable.addLine(start, end);
        x1 = x0;
      }//end for
  }

  fieldLinesTable.findIntersections();
}//end createLinesTable

void FieldInfo::draw(DrawingCanvas2D& canvas) const
{
  fieldLinesTable.draw(canvas);

  // draw throw in lines
  canvas.pen("000000", 1);
  canvas.drawLine(leftThrowInPointOwn.x,leftThrowInPointOwn.y,leftThrowInPointOpp.x,leftThrowInPointOpp.y);
  canvas.drawLine(rightThrowInPointOwn.x,rightThrowInPointOwn.y,rightThrowInPointOpp.x,rightThrowInPointOpp.y);

  // draw goal posts
  canvas.pen("0000FF", 20);

  // Opponent Goal
  canvas.drawCircle(
             opponentGoalPostLeft.x,
             opponentGoalPostLeft.y,
             goalpostRadius - 10);
  canvas.drawCircle(
             opponentGoalPostRight.x,
             opponentGoalPostRight.y,
             goalpostRadius - 10);

  // Own Goal
  canvas.drawCircle(
             ownGoalPostLeft.x,
             ownGoalPostLeft.y,
             goalpostRadius - 10);
  canvas.drawCircle(
             ownGoalPostRight.x,
             ownGoalPostRight.y,
             goalpostRadius - 10);
}

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

  stream << "enable_arcs_at_penalty_box = " << enable_arcs_at_penalty_box << "\n";
  stream << "center_x_in_own_half = "       << center_x_in_own_half << "\n";
  stream << "radius = "                     << radius << "\n";
  stream << "start_angle = "                << start_angle << "\n";
  stream << "target_angle = "               << target_angle << "\n";

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
