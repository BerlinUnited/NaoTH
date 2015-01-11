#include "StrategySymbols.h"

#include "Tools/Debug/DebugModify.h"
#include <Tools/Debug/DebugBufferedOutput.h>

using namespace std;

void StrategySymbols::registerSymbols(xabsl::Engine& engine)
{
  // was used for search. obsolete?
  engine.registerDecimalInputSymbol("attention.mi_point.x", &getAttentionModel().mostInterestingPoint.x );
  engine.registerDecimalInputSymbol("attention.mi_point.y", &getAttentionModel().mostInterestingPoint.y );

  // S3D team stuff
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.number", &getOppClosestToBallNumber);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.x", &getPlayersModel().oppClosestToBall.pose.translation.x);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.y", &getPlayersModel().oppClosestToBall.pose.translation.y);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.rotation", &getPlayersModel().oppClosestToBall.pose.rotation);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.time_since_last_seen", &getOppClosestToBallTimeSinceLastSeen);
  engine.registerDecimalInputSymbol("players.opp_closest_to_ball.distance_to_ball", &getOppClosestToBallDistanceToBall);

  engine.registerBooleanInputSymbol("situation.opponent_at_the_ball", &getOpponentAtTheBall);

  engine.registerDecimalInputSymbol("players.own_closest_to_ball.number", &getOwnClosestToBallNumber);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.x", &getPlayersModel().ownClosestToBall.pose.translation.x);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.y", &getPlayersModel().ownClosestToBall.pose.translation.y);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.rotation", &getPlayersModel().ownClosestToBall.pose.rotation);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.time_since_last_seen", &getOwnClosestToBallTimeSinceLastSeen);
  engine.registerDecimalInputSymbol("players.own_closest_to_ball.distance_to_ball", &getOwnClosestToBallDistanceToBall);


  // SPL defender pose
  engine.registerDecimalInputSymbol("defense.simplePose.translation.x", &simpleDefensePoseX);
  engine.registerDecimalInputSymbol("defense.simplePose.translation.y", &simpleDefensePoseY);
  engine.registerDecimalInputSymbol("defense.simplePose.rotation", &simpleDefensePoseA);

  // S3D defender pose
  engine.registerDecimalInputSymbol("defense.pose.translation.x", &defensePoseX);
  engine.registerDecimalInputSymbol("defense.pose.translation.y", &defensePoseY);
  engine.registerDecimalInputSymbol("defense.pose.rotation", &defensePoseA);

  // goalie positioning
  engine.registerDecimalInputSymbol("goalie.guardline.x", &goalieGuardPositionX);
  engine.registerDecimalInputSymbol("goalie.guardline.y", &goalieGuardPositionY);
  engine.registerDecimalInputSymbol("penalty_goalie.pos.x", &penaltyGoalieGuardPositionX);
  engine.registerDecimalInputSymbol("penalty_goalie.pos.y", &penaltyGoalieGuardPositionY);

  engine.registerDecimalInputSymbol("soccer_strategy.formation.x", &getSoccerStrategy().formation.x);
  engine.registerDecimalInputSymbol("soccer_strategy.formation.y", &getSoccerStrategy().formation.y);

  // XABSL-Option (current situation) symbol for some models.
  // informs about the currently used option (option itself must set this variable!)
  // engine.registerEnumeratedOutputSymbol("situationStatus", "StatusID", &getSituationStatusId);
  engine.registerBooleanOutputSymbol("situationStatusOwnHalf", &setSituationStatusOwnHalf, &getSituationStatusOwnHalf);
  engine.registerBooleanOutputSymbol("situationStatusOppHalf", &setSituationStatusOppHalf, &getSituationStatusOppHalf);
  
  // NOTE: do we still need it?
  engine.registerBooleanInputSymbol("attack.approaching_with_right_foot", &getApproachingWithRightFoot );

  // attack direction and previews
  engine.registerDecimalInputSymbol("attack.direction", &attackDirection);
  engine.registerDecimalInputSymbol("attack.direction.preview", &attackDirectionPreviewHip);
  engine.registerDecimalInputSymbol("attack.direction.preview.left_foot", &attackDirectionPreviewLFoot);
  engine.registerDecimalInputSymbol("attack.direction.preview.right_foot", &attackDirectionPreviewRFoot);

  // action selection
  engine.registerEnumElement("attack.action_type", "attack.action_type.ball_position", 0);
  engine.registerEnumElement("attack.action_type", "attack.action_type.kick_short", 1);
  engine.registerEnumElement("attack.action_type", "attack.action_type.kick_long", 2);
  engine.registerEnumElement("attack.action_type", "attack.action_type.sidekick_left", 3);
  engine.registerEnumElement("attack.action_type", "attack.action_type.sidekick_right", 4);
  engine.registerEnumElement("attack.action_type", "attack.action_type.none", 5);

  engine.registerEnumeratedInputSymbol("attack.best_action", "attack.action_type", &getBestAction);


  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_attack_direction","draw the attack direction", false);
  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_simpleDefenderPose","draw the position of the defender", false);

}//end registerSymbols


StrategySymbols* StrategySymbols::theInstance = NULL;


void StrategySymbols::execute()
{
  // prepare the attack direction
  // NOTE: attack direction is pointing from the ball to the goal (target point)
  //       i.e., it's not affected by the inhomogeneous part of the preview (translation)
  const Vector2d& p = getSoccerStrategy().attackDirection;
  
  // ATTENTION: since it is a vector and not a point, we apply only the rotation
  attackDirection             = Math::toDegrees(p.angle());

  attackDirectionPreviewHip   = Math::toDegrees(
    Vector2d(p).rotate(-getMotionStatus().plannedMotion.hip.rotation).angle());

  attackDirectionPreviewLFoot = Math::toDegrees(
    Vector2d(p).rotate(-getMotionStatus().plannedMotion.lFoot.rotation).angle());

  attackDirectionPreviewRFoot = Math::toDegrees(
    Vector2d(p).rotate(-getMotionStatus().plannedMotion.rFoot.rotation).angle());


  DEBUG_REQUEST("XABSL:StrategySymbols:draw_attack_direction",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 50);
    const Vector2d& ball = getBallModel().position;
    Vector2d p(200,0);
    p.rotate( Math::fromDegrees(attackDirection));
    ARROW(ball.x, ball.y, (ball.x + p.x), (ball.y + p.y));
  );

  PLOT("XABSL:attackDirection", attackDirection);

  simpleDefenderPose = calculateSimpleDefensePose();
  DEBUG_REQUEST("XABSL:StrategySymbols:draw_simpleDefenderPose",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 50);
    CIRCLE(simpleDefenderPose.translation.x, simpleDefenderPose.translation.y, 30);
  );

}//end execute

//int StrategySymbols::getSituationStatusId(){ 
//	return (int)(theInstance->situationStatus.id); 
//}

bool StrategySymbols::getSituationStatusOwnHalf(){ 
	return theInstance->getSituationStatus().ownHalf; 
}

void StrategySymbols::setSituationStatusOwnHalf(bool ownHalf){ 
	theInstance->getSituationStatus().ownHalf = ownHalf; 
}


bool StrategySymbols::getSituationStatusOppHalf(){ 
	return theInstance->getSituationStatus().oppHalf; 
}

void StrategySymbols::setSituationStatusOppHalf(bool oppHalf){ 
	theInstance->getSituationStatus().oppHalf = oppHalf; 
}

Vector2d StrategySymbols::calculateGoalieGuardPosition()
{
  Vector2d ballPos = getRobotPose()*getBallModel().futurePosition[5];

  double groundLineDistance = 500.0;
  MODIFY("StrategySymbols:groundLineDistance", groundLineDistance);
  // apply clipping
  double x = getFieldInfo().xPosOwnGroundline+groundLineDistance;
  double y = Math::clamp(ballPos.y, getFieldInfo().yPosRightGoalpost+400, getFieldInfo().yPosLeftGoalpost-400);

  Vector2d result;
  result.x = x;
  result.y = y;
  
  return result;
}

Vector2d StrategySymbols::calculatePenaltyGoalieGuardPosition()
{
  const Vector2d goalCenter(getFieldInfo().xPosOwnGroundline, 0);
  const Vector2d& ballPos = getRobotPose()*getBallModel().futurePosition[5];

  Math::LineSegment goal2BallLine(goalCenter, ballPos);

  Vector2d p1(0,getFieldInfo().yPosLeftGoalpost);
  Vector2d p2(getFieldInfo().xPosOwnPenaltyArea-250, getFieldInfo().yPosLeftGoalpost+250);
  Vector2d p3(getFieldInfo().xPosOwnPenaltyArea-250, getFieldInfo().yPosRightGoalpost-250);
  Vector2d p4(0,getFieldInfo().yPosRightGoalpost);

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

    Vector2d result(getFieldInfo().xPosOwnPenaltyArea-250,0.0);
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
  Vector2d oppGoal = theInstance->getSelfLocGoalModel().getOppGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter();
  
  Vector2d ballPose = theInstance->getBallModel().position;

  // normal vector to the RIGHT side
  oppGoal.rotateRight();

  // true = right
  // false = left
  return oppGoal*ballPose > 0;
}//end getApproachingFoot

double StrategySymbols::getOppClosestToBallNumber()
{
  return theInstance->getPlayersModel().oppClosestToBall.number;
}

double StrategySymbols::getOppClosestToBallTimeSinceLastSeen()
{
  return theInstance->getFrameInfo().getTimeSince(
    theInstance->getPlayersModel().oppClosestToBall.frameInfoWhenWasSeen.getTime());
}

double StrategySymbols::getOppClosestToBallDistanceToBall()
{
  return (theInstance->getBallModel().position - theInstance->getPlayersModel().oppClosestToBall.pose.translation).abs();
}

bool StrategySymbols::getOpponentAtTheBall()
{
  if(theInstance->getPlayersModel().oppClosestToBall.frameInfoWhenWasSeen.getFrameNumber() != theInstance->getFrameInfo().getFrameNumber())
    return false;

  // the ball model in the local cooordinates of the opponent
  Vector2d opponentBall = theInstance->getPlayersModel().oppClosestToBall.pose / theInstance->getBallModel().position;
  /*
  FIELD_DRAWING_CONTEXT;
  PEN("AA9900",20);
  CIRCLE(opponentBall.x, opponentBall.y, theInstance->fieldInfo.ballRadius);
  */
  return opponentBall.abs() < 300 && fabs(opponentBall.angle()) < Math::fromDegrees(30);
}

double StrategySymbols::getOwnClosestToBallNumber()
{
  return theInstance->getPlayersModel().ownClosestToBall.number;
}

double StrategySymbols::getOwnClosestToBallTimeSinceLastSeen()
{
  return theInstance->getFrameInfo().getTimeSince(
    theInstance->getPlayersModel().ownClosestToBall.frameInfoWhenWasSeen.getTime());
}

double StrategySymbols::getOwnClosestToBallDistanceToBall()
{
  return (theInstance->getBallModel().position - theInstance->getPlayersModel().ownClosestToBall.pose.translation).abs();
}

Pose2D StrategySymbols::calculateSimpleDefensePose()
{
  Pose2D defPose;
  Vector2d ownGoal = getSelfLocGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter();

  double d = ownGoal.abs();

  Vector2d p = getBallModel().position - ownGoal;

  if(p.abs() > d ) {
    p = p.normalize(1200) + ownGoal;

    defPose.translation = p;
    defPose.rotation = getBallModel().position.angle();
  }
  else
  {
    defPose.translation =(getBallModel().position-ownGoal)*0.5 + ownGoal;
    defPose.rotation = defPose.translation.angle();
  }

  defPose = getMotionStatus().plannedMotion.hip / defPose.translation;
  defPose.rotation = getBallModel().positionPreview.angle();

/*  FIELD_DRAWING_CONTEXT;
  PEN("00FF00", 20);
  CIRCLE(defPose.translation.x, defPose.translation.y, 30);
 */ 

  return defPose;
}

double StrategySymbols::simpleDefensePoseX() {
  return theInstance->simpleDefenderPose.translation.x; //theInstance->calculateSimpleDefensePose().translation.x;
}

double StrategySymbols::simpleDefensePoseY() {
  return theInstance->simpleDefenderPose.translation.y; //theInstance->calculateSimpleDefensePose().translation.y;
}

double StrategySymbols::simpleDefensePoseA() {
  return Math::toDegrees(theInstance->simpleDefenderPose.rotation); //Math::toDegrees(theInstance->calculateSimpleDefensePose().rotation);
}

Pose2D StrategySymbols::calculateDefensePose()
{
  Pose2D defPose;
  Vector2d ownGoal = getSelfLocGoalModel().getOwnGoal(theInstance->getCompassDirection(), theInstance->getFieldInfo()).calculateCenter();

  Pose2D g(ownGoal.angle(), ownGoal.x, ownGoal.y);
  g -= getMotionStatus().plannedMotion.hip;
  ownGoal = g.translation;
//  double d = ownGoal.abs();
//  Vector2d p = ballModel.position - ownGoal;
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

  Vector2d pM = getBallModel().positionPreview * 0.5;
  Vector2d nM = pM;
  nM.rotate(Math::pi_2);
  Math::Line lMI = Math::Line(pM, nM);
  Math::Line lBI = Math::Line(ownGoal, getBallModel().positionPreview - ownGoal);
  double i = lBI.intersection(lMI);
  Vector2d pI = lBI.point(i);
  if ( i > 0 && (pI-ownGoal).abs2() < (getBallModel().positionPreview-ownGoal).abs2() )
  {
    defPose.translation = pI;
    defPose.rotation = getBallModel().positionPreview.angle();
  }
  else
  {
    defPose.translation = (getBallModel().positionPreview+ownGoal)*0.5;
    defPose.rotation = defPose.translation.angle();
  }
  return defPose;
}

double StrategySymbols::defensePoseX() {
  return theInstance->calculateDefensePose().translation.x;
}

double StrategySymbols::defensePoseY() {
  return theInstance->calculateDefensePose().translation.y;
}

double StrategySymbols::defensePoseA() {
    return Math::toDegrees(theInstance->calculateDefensePose().rotation);
}

int StrategySymbols::getBestAction() {
   return theInstance->getActionModel().myAction;
}
