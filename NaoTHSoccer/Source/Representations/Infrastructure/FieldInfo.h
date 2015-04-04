/**
* @file FieldInfo.h
* 
* Declaration of class FieldInfo
*
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/ 

#ifndef _FieldInfo_h_
#define _FieldInfo_h_

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Geometry.h>
#include <Tools/ColorClasses.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/Printable.h>
#include "Tools/LinesTable.h"



class FieldInfo : public ParameterList, public naoth::Printable
{
private:
  void calculateValues();

  /** */
  void createLinesTable();

  /** */
  void calculateCrossings();

public:
  FieldInfo();

  void draw(DrawingCanvas2D& canvas) const;
  virtual void print(std::ostream& stream) const;

  // some tools
  inline bool insideCarpet(const Vector2d& p) {
    return carpetRect.inside(p);
  }

  //colors
  ColorClasses::Color ballColor;
//  const ColorClasses::Color ownGoalColor        = ColorClasses::yellow;
//  const ColorClasses::Color opponentGoalColor   = ColorClasses::skyblue;

  //////////////// basic values from configuration ////////////////
  double ballRadius;

  // size of the whole field (including the green area outside the lines) 
  double xFieldLength;
  double yFieldLength;

  // size of the field lines
  double xLength;
  double yLength;

  // penalty area
  double xPenaltyAreaLength;
  double yPenaltyAreaLength;

  double centerCircleRadius;
  double fieldLinesWidth;
  
  // goal
  double goalWidth;
  double goalHeight;
  double goalpostRadius;


  /////////////// pre-calculated values from basic values //////////////
  double xPosHalfWayLine;

  double xPosOwnGoal;
  double xPosOpponentGoal;
  double xPosOwnGroundline;
  double xPosOpponentGroundline;
  double xPosOpponentPenaltyArea;
  double xPosOwnPenaltyArea;
  
  double yPosLeftSideline;
  double yPosRightSideline;
  double yPosLeftPenaltyArea;
  double yPosRightPenaltyArea;
  double xPenaltyMarkDistance;

  double yPosRightGoalpost;
  double yPosLeftGoalpost;

  //Extra Stuff for the Simulator
  double xThrowInLineOwn;
  double xThrowInLineOpp;

  double yThrowInLineLeft;
  double yThrowInLineRight;

  Vector2d leftThrowInPointOwn;
  Vector2d rightThrowInPointOwn;

  Vector2d leftThrowInPointOpp;
  Vector2d rightThrowInPointOpp;

  //Should be done differently
  Math::LineSegment leftLineSegment;
  Math::LineSegment rightLineSegment;
  Math::LineSegment ownLineSegment;
  Math::LineSegment oppLineSegment;
  //End extra stuff

  Geometry::Rect2d fieldRect;
  Geometry::Rect2d carpetRect;
  Geometry::Rect2d ownHalfRect;
  Geometry::Rect2d oppHalfRect;

  enum LineCrossingsId
  {
    // L crossings
    opponentCornerLeft,
    opponentCornerRight,
    ownCornerLeft,
    ownCornerRight,

    opponentPenaltyCornerLeft,
    opponentPenaltyCornerRight,
    ownPenaltyCornerLeft,
    ownPenaltyCornerRight,

    // T crossings
    opponentGoalTCrossingLeft,
    opponentGoalTCrossingRight,
    ownGoalTCrossingLeft,
    ownGoalTCrossingRight,
    centerTCrossingLeft,
    centerTCrossingRight,

    // X crossings
    crossingCenterCircleLeft,
    crossingCenterCircleRight,

    numberLineCrossingsId
  };

  class Crossing
  {
  public:
    Crossing() : id(numberLineCrossingsId) {}
    LineCrossingsId id;
    //Type type; // TODO: L, T, X
    Vector2d position;
  };

  Crossing crossings[numberLineCrossingsId];

  /*
  // L crossings
  Vector2<double> opponentCornerLeft;
  Vector2<double> opponentCornerRight;
  Vector2<double> ownCornerLeft;
  Vector2<double> ownCornerRight;

  Vector2<double> opponentPenaltyCornerLeft;
  Vector2<double> opponentPenaltyCornerRight;
  Vector2<double> ownPenaltyCornerLeft;
  Vector2<double> ownPenaltyCornerRight;

  // T crossings
  Vector2<double> opponentGoalTCrossingLeft;
  Vector2<double> opponentGoalTCrossingRight;
  Vector2<double> ownGoalTCrossingLeft;
  Vector2<double> ownGoalTCrossingRight;
  Vector2<double> centerTCrossingLeft;
  Vector2<double> centerTCrossingRight;

  // X Crossings
  Vector2<double> crossingCenterCircleLeft;
  Vector2<double> crossingCenterCircleRight;
  */

  // goal post positions
  Vector2d opponentGoalPostLeft;
  Vector2d opponentGoalPostRight;
  Vector2d opponentGoalCenter;

  Vector2d ownGoalPostLeft;
  Vector2d ownGoalPostRight;
  Vector2d ownGoalCenter;

  /** */
  LinesTable fieldLinesTable;
};


#endif //__FieldInfo_h_
