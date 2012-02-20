#include "StrategySymbols.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Geometry.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/GoalModel.h"
#include "Representations/Modeling/PlayerInfo.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Representations/Motion/MotionStatus.h"

void StrategySymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("attention.mi_point.x", &attentionModel.mostInterestingPoint.x );
  engine.registerDecimalInputSymbol("attention.mi_point.y", &attentionModel.mostInterestingPoint.y );

  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.number", &getOppClosestToBallNumber);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.x", &playersModel.oppClosestToBall.pose.translation.x);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.y", &playersModel.oppClosestToBall.pose.translation.y);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.rotation", &playersModel.oppClosestToBall.pose.rotation);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.time_since_last_seen", &getOppClosestToBallTimeSinceLastSeen);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.distance_to_ball", &getOppClosestToBallDistanceToBall);

  engine.registerBooleanInputSymbol("situation.opponent_at_the_ball", &getOpponentAtTheBall);

  engine.registerDecimalInputSymbol("players.own_closest_to_ball.number", &getOwnClosestToBallNumber);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.x", &playersModel.ownClosestToBall.pose.translation.x);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.y", &playersModel.ownClosestToBall.pose.translation.y);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.rotation", &playersModel.ownClosestToBall.pose.rotation);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.time_since_last_seen", &getOwnClosestToBallTimeSinceLastSeen);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.distance_to_ball", &getOwnClosestToBallDistanceToBall);


  engine.registerDecimalInputSymbol("attack.direction", &calculateAttackDirection);

  engine.registerDecimalInputSymbol("defense.pose.translation.x", &defensePoseX);
  engine.registerDecimalInputSymbol("defense.pose.translation.y", &defensePoseY);
  engine.registerDecimalInputSymbol("defense.pose.rotation", &defensePoseA);

  //symbols for set pieces
  engine.registerDecimalInputSymbol("setpiece.pose.x", &setpiecePosition.x);
  engine.registerDecimalInputSymbol("setpiece.pose.y", &setpiecePosition.y);
  engine.registerBooleanInputSymbol("setpiece.participation", &setpieceParticipation);

  engine.registerBooleanInputSymbol("attack.approaching_with_right_foot", &getApproachingWithRightFoot );




  engine.registerDecimalInputSymbol("circlex", &circleX);
  engine.registerDecimalInputSymbolDecimalParameter("circlex", "circlex.directionx", &cycle.in_direction.x);
  engine.registerDecimalInputSymbolDecimalParameter("circlex", "circlex.directiony", &cycle.in_direction.y);
  engine.registerDecimalInputSymbolDecimalParameter("circlex", "circlex.x", &cycle.target.x);
  engine.registerDecimalInputSymbolDecimalParameter("circlex", "circlex.y", &cycle.target.y);
  engine.registerDecimalInputSymbolDecimalParameter("circlex", "circlex.robot.x", &cycle.robot_pose.x);
  engine.registerDecimalInputSymbolDecimalParameter("circlex", "circlex.robot.y", &cycle.robot_pose.y);

  engine.registerDecimalInputSymbol("circley", &circleY);
  engine.registerDecimalInputSymbolDecimalParameter("circley", "circley.directionx", &cycle.in_direction.x);
  engine.registerDecimalInputSymbolDecimalParameter("circley", "circley.directiony", &cycle.in_direction.y);
  engine.registerDecimalInputSymbolDecimalParameter("circley", "circley.x", &cycle.target.x);
  engine.registerDecimalInputSymbolDecimalParameter("circley", "circley.y", &cycle.target.y);
  engine.registerDecimalInputSymbolDecimalParameter("circley", "circley.robot.x", &cycle.robot_pose.x);
  engine.registerDecimalInputSymbolDecimalParameter("circley", "circley.robot.y", &cycle.robot_pose.y);

  engine.registerDecimalInputSymbol("goalie.guardline.x", &goalieGuardPositionX);
  engine.registerDecimalInputSymbol("goalie.guardline.y", &goalieGuardPositionY);
  engine.registerDecimalInputSymbol("penalty_goalie.pos.x", &penaltyGoalieGuardPositionX);
  engine.registerDecimalInputSymbol("penalty_goalie.pos.y", &penaltyGoalieGuardPositionY);

  engine.registerDecimalInputSymbol("soccer_strategy.formation.x", &soccerStrategy.formation.x);
  engine.registerDecimalInputSymbol("soccer_strategy.formation.y", &soccerStrategy.formation.y);

  //Ausgabe in RobotControl
  DEBUG_REQUEST_REGISTER("roundWalk:draw_circle", "Roter Kreis", false);
 
  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_attack_direction","draw the attack direction", false);

  //testArrangeRobots();
}//end registerSymbols


StrategySymbols* StrategySymbols::theInstance = NULL;


void StrategySymbols::execute()
{
  DEBUG_REQUEST("XABSL:StrategySymbols:draw_attack_direction",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 50);
    const Vector2<double>& ball = getBallModel().position;
    Vector2<double> p(200,0);
    p.rotate( Math::fromDegrees(calculateAttackDirection()));
    ARROW(ball.x, ball.y, (ball.x + p.x), (ball.y + p.y));
  );

}//end execute



Vector2<double> StrategySymbols::calculateGoalieGuardPosition()
{
  Vector2<double> ballPos = robotPose*ballModel.futurePosition[5];

  double groundLineDistance = 500.0;
  MODIFY("StrategySymbols:groundLineDistance", groundLineDistance);
  // apply clipping
  double x = fieldInfo.xPosOwnGroundline+groundLineDistance;
  double y = Math::clamp(ballPos.y, fieldInfo.yPosRightGoalpost+400, fieldInfo.yPosLeftGoalpost-400);


  Vector2<double> result;
  result.x = x;
  result.y = y;
  
  return result;
}

Vector2<double> StrategySymbols::calculatePenaltyGoalieGuardPosition()
{

  const Vector2<double> goalCenter(fieldInfo.xPosOwnGroundline, 0);
  const Vector2<double>& ballPos = robotPose*ballModel.futurePosition[5];

  Math::LineSegment goal2BallLine(goalCenter, ballPos);

  Vector2<double> p1(0,fieldInfo.yPosLeftGoalpost);
  Vector2<double> p2(fieldInfo.xPosOwnPenaltyArea-250, fieldInfo.yPosLeftGoalpost+250);
  Vector2<double> p3(fieldInfo.xPosOwnPenaltyArea-250, fieldInfo.yPosRightGoalpost-250);
  Vector2<double> p4(0,fieldInfo.yPosRightGoalpost);

  Math::LineSegment l1(p1, p2);
  Math::LineSegment l2(p2, p3);
  Math::LineSegment l3(p4, p3);

  Math::LineSegment lines[3];
  lines[0] = l1;
  lines[1] = l2;
  lines[2] = l3;

  std::vector<Math::Intersection> intersections;
  for(int i=0; i < 3; i++)
  {
    Math::Intersection inter(goal2BallLine,lines[i]);
    intersections.push_back(inter);
  }

  double distances[3];
  for(int i=0; i < 3; i++)
  {
    distances[i] = (intersections[i].pos - goalCenter).abs();
  }

  int minIndex = -1;
  double currentMin = std::numeric_limits<double>::max();
  for(int i=0; i < 3; i++)
  {
    if(intersections[i].type != Math::Intersection::none && (minIndex == -1 && distances[i] < currentMin))
    {
      minIndex = i;
      currentMin = distances[i];
    }
  }

  if(minIndex >= 0)
  {
    return intersections[minIndex].pos;
  }
  else
  {
    // fallback, goto middle

    Vector2<double> result(fieldInfo.xPosOwnPenaltyArea-250,0.0);
    return result;
  }
  

}

double StrategySymbols::goalieGuardPositionX()
{
  return theInstance->calculateGoalieGuardPosition().x;
}

double StrategySymbols::goalieGuardPositionY()
{
  return theInstance->calculateGoalieGuardPosition().y;
}

double StrategySymbols::penaltyGoalieGuardPositionX()
{
  return theInstance->calculatePenaltyGoalieGuardPosition().x;
}

double StrategySymbols::penaltyGoalieGuardPositionY()
{
  return theInstance->calculatePenaltyGoalieGuardPosition().y;
}

bool StrategySymbols::getApproachingWithRightFoot()
{
  // get the vector to the center of the opponent goal
  Vector2<double> oppGoal = theInstance->goalModel.getOppGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter();
  
  Vector2<double> ballPose = theInstance->ballModel.position;

  // normal vector to the RIGHT side
  oppGoal.rotateRight();

  // true = right
  // false = left
  return oppGoal*ballPose > 0;
}//end getApproachingFoot

double StrategySymbols::getOppClosestToBallNumber()
{
  return theInstance->playersModel.oppClosestToBall.number;
}

double StrategySymbols::getOppClosestToBallTimeSinceLastSeen()
{
  return theInstance->frameInfo.getTimeSince(
    theInstance->playersModel.oppClosestToBall.frameInfoWhenWasSeen.getTime());
}

double StrategySymbols::getOppClosestToBallDistanceToBall()
{
  return (theInstance->ballModel.position - theInstance->playersModel.oppClosestToBall.pose.translation).abs();
}

bool StrategySymbols::getOpponentAtTheBall()
{
  if(theInstance->playersModel.oppClosestToBall.frameInfoWhenWasSeen.getFrameNumber() != theInstance->frameInfo.getFrameNumber())
    return false;

  // the ball model in the local cooordinates of the opponent
  Vector2<double> opponentBall = theInstance->playersModel.oppClosestToBall.pose / theInstance->ballModel.position;
  /*
  FIELD_DRAWING_CONTEXT;
  PEN("AA9900",20);
  CIRCLE(opponentBall.x, opponentBall.y, theInstance->fieldInfo.ballRadius);
  */
  return opponentBall.abs() < 300 && fabs(opponentBall.angle()) < Math::fromDegrees(30);
}

double StrategySymbols::getOwnClosestToBallNumber()
{
  return theInstance->playersModel.ownClosestToBall.number;
}

double StrategySymbols::getOwnClosestToBallTimeSinceLastSeen()
{
  return theInstance->frameInfo.getTimeSince(
    theInstance->playersModel.ownClosestToBall.frameInfoWhenWasSeen.getTime());
}

double StrategySymbols::getOwnClosestToBallDistanceToBall()
{
  return (theInstance->ballModel.position - theInstance->playersModel.ownClosestToBall.pose.translation).abs();
}

Pose2D StrategySymbols::calculateDefensePose()
{
  Pose2D defPose;
  Vector2<double> ownGoal = goalModel.getOwnGoal(theInstance->compassDirection, theInstance->fieldInfo).calculateCenter();

  Pose2D g(ownGoal.angle(), ownGoal.x, ownGoal.y);
  g -= motionStatus.plannedMotion.hip;
  ownGoal = g.translation;
//  double d = ownGoal.abs();
//  Vector2<double> p = ballModel.position - ownGoal;
//  if ( p.abs() > d )
//  {
//   p = p.normalize(d) + ownGoal;
//   double k = 0.2;
//   defPose.translation = p * (1-k) + ballModel.position*k;
//   defPose.rotation = ballModel.position.angle();
//  }
//  else
//  {
//    defPose.translation = (ballModel.position+ownGoal)*0.5;
//    defPose.rotation = defPose.translation.angle();
//  }

  Vector2<double> pM = ballModel.positionPreview * 0.5;
  Vector2<double> nM = pM;
  nM.rotate(Math::pi_2);
  Math::Line lMI = Math::Line(pM, nM);
  Math::Line lBI = Math::Line(ownGoal, ballModel.positionPreview - ownGoal);
  double i = lBI.intersection(lMI);
  Vector2<double> pI = lBI.point(i);
  if ( i > 0 && (pI-ownGoal).abs2() < (ballModel.positionPreview-ownGoal).abs2() )
  {
    defPose.translation = pI;
    defPose.rotation = ballModel.positionPreview.angle();
  }
  else
  {
    defPose.translation = (ballModel.positionPreview+ownGoal)*0.5;
    defPose.rotation = defPose.translation.angle();
  }
  return defPose;
}

double StrategySymbols::defensePoseX()
{
  return theInstance->calculateDefensePose().translation.x;
}

double StrategySymbols::defensePoseY()
{
  return theInstance->calculateDefensePose().translation.y;
}

double StrategySymbols::defensePoseA()
{
    return Math::toDegrees(theInstance->calculateDefensePose().rotation);
}

double StrategySymbols::setpiecePoseX()
{
  return theInstance->setpiecePosition.x;
}

double StrategySymbols::setpiecePoseY()
{
  return theInstance->setpiecePosition.y;
}


//Calculation for set pieces
bool StrategySymbols::calculateSetPiecePose()
{
  goalKickPlayer = 0;

  // check who is alive
  std::vector<Vector2<double> > playersPoints;
  std::vector<unsigned int> playersPointsNumbers;
  vector<PlayersModel::Player>::const_iterator iter = theInstance->playersModel.teammates.begin();
  for (; iter != theInstance->playersModel.teammates.end(); ++iter)
  {
    if (theInstance->frameInfo.getTimeSince(iter->frameInfoWhenWasSeen.getTime()) < 1000)
    {
      // TODO: consider direction?
      playersPoints.push_back(iter->globalPose.translation);
      playersPointsNumbers.push_back(iter->number);
    }//end if
  }//end for



  //switch positions
  if (playerInfo.gameData.playMode == GameData::goal_kick_opp) 
  {
    std::vector<Vector2<double> > setPiecePoints;
    //input of positions
    setPiecePoints.push_back(Vector2<double>(fieldInfo.xPosOpponentPenaltyArea-2700,  fieldInfo.yPosLeftPenaltyArea+2550));
    setPiecePoints.push_back(Vector2<double>(fieldInfo.xPosOpponentPenaltyArea-2700, fieldInfo.yPosLeftPenaltyArea-2550));
    setPiecePoints.push_back(Vector2<double>(fieldInfo.xPosOpponentPenaltyArea-200,     0.0));

    // init
    std::vector<int> placesToRobots(3);
    // TODO: make it general
    if(playersPoints.size() < setPiecePoints.size()) 
      return false;

    for(unsigned int i = 0; i < placesToRobots.size(); i++)
    {
      placesToRobots[i] = i;
    }//end for

    arrangeRobots(playersPoints, setPiecePoints, placesToRobots);


    for(unsigned int i = 0; i < placesToRobots.size(); i++)
    {
      if(playersPointsNumbers[placesToRobots[i]] == theInstance->playerInfo.gameData.playerNumber)
      {
        theInstance->setpiecePosition = setPiecePoints[i];
        return true;
      }
    }//end for

  } 
  else if (playerInfo.gameData.playMode == GameData::goal_kick_own) 
  {
    // do nothing
  } 
  else if (playerInfo.gameData.playMode == GameData::kick_off_opp) 
  {
    // do nothing
  } 
  else if (  playerInfo.gameData.playMode == GameData::free_kick_opp
          || playerInfo.gameData.playMode == GameData::kick_in_opp
          || playerInfo.gameData.playMode == GameData::corner_kick_opp) 
  {
    // TODO?
  } 
  else if (  playerInfo.gameData.playMode == GameData::free_kick_own
          || playerInfo.gameData.playMode == GameData::kick_in_own
          || playerInfo.gameData.playMode == GameData::corner_kick_own) 
  {
    // do nothing special
    return false;
  }

  return false;
}//end calculateSetPiecePose


void StrategySymbols::testArrangeRobots()
{
  std::vector<Vector2<double> > setPiecePoints;
  //input of positions
  setPiecePoints.push_back(Vector2<double>(-1700.0, -1700.0));
  setPiecePoints.push_back(Vector2<double>(-2400.0,     0.0));
  setPiecePoints.push_back(Vector2<double>(-1700.0,  1700.0));


  std::vector<int> placesToRobots(3);

  // init
  for(unsigned int i = 0; i < placesToRobots.size(); i++)
  {
    placesToRobots[i] = i;
  }//end for

  std::vector<Vector2<double> > playersPoints;
  playersPoints.push_back(Vector2<double>(2400.0,      0.0));
  playersPoints.push_back(Vector2<double>(3400.0,    500.0));
  playersPoints.push_back(Vector2<double>(2800.0,   -700.0));
  playersPoints.push_back(Vector2<double>(-2500.0,     0.0));
  playersPoints.push_back(Vector2<double>(-1600.0,  1600.0));
  playersPoints.push_back(Vector2<double>(-1600.0, -1700.0));
  

  arrangeRobots(playersPoints, setPiecePoints, placesToRobots);

  std::vector<int> expectedPlacesToRobots;
  expectedPlacesToRobots.push_back(0);
  expectedPlacesToRobots.push_back(1);
  expectedPlacesToRobots.push_back(2);

  for(unsigned int i = 0; i < placesToRobots.size(); i++)
  {
    if(placesToRobots[i] != expectedPlacesToRobots[1])
    {
      cout << "Assignement Wrong" << endl;
    }
  }//end for
}//end testArrangeRobots


void StrategySymbols::arrangeRobots(
  const std::vector<Vector2<double> >& robots, 
  const std::vector<Vector2<double> >& places, 
  std::vector<int>& placesToRobots )
{
  size_t numberOfRobots = robots.size();
  size_t numberOfPlaces = places.size();

  double fakeDistance = 0;

  // inverse assignement
  std::vector<int> robotsToPlaces(numberOfRobots,-1); // init with invalid places -1

  ASSERT(numberOfPlaces == placesToRobots.size());
  ASSERT(numberOfPlaces <= numberOfRobots);
  
  // TODO: treat the case numberOfPlaces > numberOfRobots
  //initial assignment
  for(size_t i = 0; i < numberOfPlaces; i++)
  {
    //int robotIndex = (int)(Math::random()*(numberOfRobots-1)+0.5);
    robotsToPlaces[placesToRobots[i]] = i;

    // place i <= robotIndex
    //placesToRobots[i] = i;
  }//end for


  int numberOfIterations = 50;//20;
  for(int i = 0; i < numberOfIterations; i++)
  {
    // select place
    int placeIndexOne = (int)(Math::random()*(numberOfPlaces-1)+0.5);
    // take the robot assighned to this place
    int robotIndexOne = placesToRobots[placeIndexOne];

    // select another robot
    int robotIndexTwo = (int)(Math::random()*(numberOfRobots-1)+0.5);
    // take the place of this robot (!!!could be -1)
    int placeIndexTwo = robotsToPlaces[robotIndexTwo];

    if(robotIndexOne == robotIndexTwo) 
      continue;

    // current distance
    double distOne = (robots[robotIndexOne] - places[placeIndexOne]).abs();
    double distTwo = fakeDistance;
    if(placeIndexTwo != -1)
      distTwo = (robots[robotIndexTwo] - places[placeIndexTwo]).abs();
    double currentDistance = distOne + distTwo;
  
    // alternative distance
    double alternativeDistOne = fakeDistance;
    if(placeIndexTwo != -1)
      alternativeDistOne = (robots[robotIndexOne] - places[placeIndexTwo]).abs();

    double alternativeDistTwo = (robots[robotIndexTwo] - places[placeIndexOne]).abs();
    double alternativeDistance = alternativeDistOne + alternativeDistTwo;

    // switch
    if(alternativeDistance < currentDistance)
    {
      placesToRobots[placeIndexOne] = robotIndexTwo;
      if(placeIndexTwo != -1)
        placesToRobots[placeIndexTwo] = robotIndexOne;

      robotsToPlaces[robotIndexOne] = placeIndexTwo;
      robotsToPlaces[robotIndexTwo] = placeIndexOne;
    }//end if
  }//end for

}//end arrangeRobots


bool StrategySymbols::setpieceParticipation()
{
  return theInstance->calculateSetPiecePose();
}

double StrategySymbols::calculateAttackDirection()
{
  const Vector2<double>& p = theInstance->soccerStrategy.attackDirection;
  return Math::toDegrees(p.angle());
}


Vector2<double> StrategySymbols::calculatePlayerPotentialField(const Vector2<double>& player, const Vector2<double>& ball)
{
  const static double a = 1500;
  const static double d = 2000;

  Vector2<double> v = player-ball;
  double t = v.abs();
  if ( t >= d-100 ) return Vector2<double>(0,0);

  double ang = v.angle();
  return Vector2<double>(cos(ang), sin(ang)) * exp(a / d - a / (d - t));
}


//returns the x-coordinate of the edge of the circle the robot shall move to reach the inputpoint with direction to another inputpoint
/**
 @param  cycle.in_direction.x    x-coordinate of point the angle of the robot should be to
         cycle.in_direction.y    y-coordinate of point the angle of the robot should be to
         cycle.robot_pose.y    x-coordinate of actual robot-position
         cycle.robot_pose.y    y-coordinate of actual robot-position
         cycle.target.x      x-coordinate of point the robot should go to
         cycle.target.y      y-coordinate of point the robot should go to

*/
double StrategySymbols::circleX()
{  
  //berechne gerade durch ziel und Punkt der die Ausrichtung bestimmt
  double quotient=(theInstance->cycle.in_direction.x-theInstance->cycle.target.x);
  if(quotient==0)quotient=0.0001;
  double m_target_goal_circle= (theInstance->cycle.in_direction.y-theInstance->cycle.target.y)/quotient;

//  double n_target_goal_circle= theInstance->cycle.in_direction.y-m_target_goal_circle*theInstance->cycle.in_direction.x;

  //berechne orthogonale zur geraden im punkt des ziels
  if(m_target_goal_circle==0)
   m_target_goal_circle=0.0001;
  double m_orthogonal_circle=-1/m_target_goal_circle;
  double n_orthogonal_circle=theInstance->cycle.target.y-m_orthogonal_circle*theInstance->cycle.target.x;
  
  
  //berechne Mittelpunkt des Kreises mit robot und ziel als randpunkte
  double x_middle=(
    theInstance->cycle.robot_pose.abs2() - theInstance->cycle.target.abs2()
    +2*n_orthogonal_circle*(theInstance->cycle.target.y-theInstance->cycle.robot_pose.y))
    /(-2*(theInstance->cycle.target.x-theInstance->cycle.robot_pose.x+m_orthogonal_circle*(theInstance->cycle.target.y-theInstance->cycle.robot_pose.y)));
  
  double y_middle=m_orthogonal_circle*x_middle+n_orthogonal_circle;

  double radius = std::sqrt(Math::sqr(theInstance->cycle.target.y-y_middle)+Math::sqr(theInstance->cycle.target.x-x_middle));
  
  //berechne parameter fuer gerade durch robot und ziel
  double m_robot_target_circle=(theInstance->cycle.target.y-theInstance->cycle.robot_pose.y)/(theInstance->cycle.target.x-theInstance->cycle.robot_pose.x);

//  double n_robot_target_circle=theInstance->cycle.robot_pose.y-m_robot_target_circle*theInstance->cycle.robot_pose.x;
  
  //punktkoordinaten auf halber strecke zwischen robot und ziel
  double y_half_circle=(theInstance->cycle.robot_pose.y+theInstance->cycle.target.y)/2;
  double x_half_circle=(theInstance->cycle.robot_pose.x+theInstance->cycle.target.x)/2;
  
  //parameter fuer zur geraden orthogonalen geraden im punkt x_half,y_half 
  if(m_robot_target_circle==0)
    m_robot_target_circle=0.0001;
  double m_orthogonal_circle_half=-1/m_robot_target_circle;
  double n_orthogonal_circle_half=y_half_circle-m_orthogonal_circle_half*x_half_circle;
  
  //Schnittpunkte zwischen Kreis und orthogonalen
  double p= -2*(x_middle+y_middle*m_orthogonal_circle_half-m_orthogonal_circle_half*n_orthogonal_circle_half)/(1+Math::sqr(m_orthogonal_circle_half));
  double q= (Math::sqr(n_orthogonal_circle_half)-2*y_middle*n_orthogonal_circle_half-Math::sqr(radius)+Math::sqr(y_middle)+Math::sqr(x_middle))/(1+Math::sqr(m_orthogonal_circle_half));
  
  double x1=-p/2+sqrt(Math::sqr(p)/4-q);
  double x2=-p/2-sqrt(Math::sqr(p)/4-q);
  double y1=m_orthogonal_circle_half*x1+n_orthogonal_circle_half;
  double y2=m_orthogonal_circle_half*x2+n_orthogonal_circle_half;
  double target_x;

  if (theInstance->cycle.in_direction.y>theInstance->cycle.target.y)
  {
    if(y1<y2)target_x=x1;
    else  target_x=x2;
  }
  else
  {
    if(y1>y2)target_x=x1;
    else target_x=x2;
  }

  //ASSERT(!Math::isNan(target_x));
  return target_x;
}//end circleX


//returns the x-coordinate of the edge of the circle the robot shall move to reach the inputpoint with direction to another inputpoint
/**
 @param  cycle.in_direction.x    x-coordinate of point the angle of the robot should be to
         cycle.in_direction.y    y-coordinate of point the angle of the robot should be to
         cycle.robot_pose.y    x-coordinate of actual robot-position
         cycle.robot_pose.y    y-coordinate of actual robot-position
         cycle.target.x      x-coordinate of point the robot should go to
         cycle.target.y      y-coordinate of point the robot should go to

*/
double StrategySymbols::circleY()
{

  //berechne gerade durch ziel und Punkt der die Ausrichtung bestimmt
  double quotient=(theInstance->cycle.in_direction.x-theInstance->cycle.target.x);
  if(quotient==0)quotient=0.0001;
  double m_target_goal_circle= (theInstance->cycle.in_direction.y-theInstance->cycle.target.y)/quotient;
//  double n_target_goal_circle= theInstance->cycle.in_direction.y-m_target_goal_circle*theInstance->cycle.in_direction.x;
  //berechne orthogonale zur geraden im punkt des ziels
  if(m_target_goal_circle==0)
    m_target_goal_circle=0.0001;
  double m_orthogonal_circle=-1/m_target_goal_circle;
  
  double n_orthogonal_circle=theInstance->cycle.target.y-m_orthogonal_circle*theInstance->cycle.target.x;
  //berechne Mittelpunkt des Kreises mit robot und ziel als randpunkte
  double x_middle=(-Math::sqr(theInstance->cycle.target.x)+Math::sqr(theInstance->cycle.robot_pose.x)-Math::sqr(theInstance->cycle.target.y)+Math::sqr(theInstance->cycle.robot_pose.y)+2*n_orthogonal_circle*(theInstance->cycle.target.y-theInstance->cycle.robot_pose.y))/(-2*(theInstance->cycle.target.x-theInstance->cycle.robot_pose.x+m_orthogonal_circle*(theInstance->cycle.target.y-theInstance->cycle.robot_pose.y)));
  double y_middle=m_orthogonal_circle*x_middle+n_orthogonal_circle;
  double radius = sqrt(Math::sqr(theInstance->cycle.target.y-y_middle)+Math::sqr(theInstance->cycle.target.x-x_middle));
  //berechne parameter fuer gerade durch robot und ziel
  double m_robot_target_circle=(theInstance->cycle.target.y-theInstance->cycle.robot_pose.y)/(theInstance->cycle.target.x-theInstance->cycle.robot_pose.x);
//  double n_robot_target_circle=theInstance->cycle.robot_pose.y-m_robot_target_circle*theInstance->cycle.robot_pose.x;
  //punktkoordinaten auf halber strecke zwischen robot und ziel
  double y_half_circle=(theInstance->cycle.robot_pose.y+theInstance->cycle.target.y)/2;
  double x_half_circle=(theInstance->cycle.robot_pose.x+theInstance->cycle.target.x)/2;
    //parameter fuer zur geraden orthogonalen geraden im punkt x_half,y_half 
  if(m_robot_target_circle==0)
   m_robot_target_circle=0.0001;
  double m_orthogonal_circle_half=-1/m_robot_target_circle;
  double n_orthogonal_circle_half=y_half_circle-m_orthogonal_circle_half*x_half_circle;
  //Schnittpunkte zwischen Kreis und orthogonalen
  double p= -2*(x_middle+y_middle*m_orthogonal_circle_half-m_orthogonal_circle_half*n_orthogonal_circle_half)/(1+Math::sqr(m_orthogonal_circle_half));
  double q= (Math::sqr(n_orthogonal_circle_half)-2*y_middle*n_orthogonal_circle_half-Math::sqr(radius)+Math::sqr(y_middle)+Math::sqr(x_middle))/(1+Math::sqr(m_orthogonal_circle_half));
  
  double x1=-p/2+sqrt(Math::sqr(p)/4-q);
  double x2=-p/2-sqrt(Math::sqr(p)/4-q);
  double y1=m_orthogonal_circle_half*x1+n_orthogonal_circle_half;
  double y2=m_orthogonal_circle_half*x2+n_orthogonal_circle_half;
  double target_y;

  if (theInstance->cycle.in_direction.y>theInstance->cycle.target.y)
  {
    if(y1<y2)target_y=y1;
    else target_y=y2;
  }
  else
  {
    if(y1>y2) target_y=y1;
    else target_y=y2;
  }
 
  double target_x=(target_y-n_orthogonal_circle_half)/m_orthogonal_circle_half;
  
  DEBUG_REQUEST("roundWalk:draw_circle",
     FIELD_DRAWING_CONTEXT;
     PEN("FF0000", 10); // farbe, liniendicke
     LINE(theInstance->cycle.robot_pose.x, theInstance->cycle.robot_pose.y, theInstance->cycle.target.x ,theInstance->cycle.target.y ); // x1,y1,x2,y2 in mm
     // LINE(theInstance->cycle.target.x, theInstance->cycle.target.y, x_target ,y_target );
     // LINE(theInstance->cycle.robot_pose.x, theInstance->cycle.robot_pose.y, x_target ,y_target );
     CIRCLE(x_middle ,y_middle,radius);
     CIRCLE(x_middle ,y_middle,100);
     CIRCLE(target_x,target_y,100);
   );

   //ASSERT(!Math::isNan(target_y));
   return target_y;
}//end circleY
