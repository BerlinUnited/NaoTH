#include "StrategySymbols.h"

#include "Tools/Debug/DebugModify.h"
#include <cmath>


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
  engine.registerDecimalInputSymbol("goalie.defensive.x", &goalieDefensivePositionX);
  engine.registerDecimalInputSymbol("goalie.defensive.y", &goalieDefensivePositionY);
  engine.registerDecimalInputSymbol("goalie.defensive.a", &goalieDefensivePositionA);

  engine.registerDecimalInputSymbol("soccer_strategy.formation.x", &getSoccerStrategy().formation.x);
  engine.registerDecimalInputSymbol("soccer_strategy.formation.y", &getSoccerStrategy().formation.y);
  
  // NOTE: do we still need it?
  engine.registerBooleanInputSymbol("attack.approaching_with_right_foot", &getApproachingWithRightFoot );

  // attack direction and previews
  engine.registerDecimalInputSymbol("attack.direction", &attackDirection);
  engine.registerDecimalInputSymbol("attack.direction.preview", &attackDirectionPreviewHip);
  engine.registerDecimalInputSymbol("attack.direction.preview.left_foot", &attackDirectionPreviewLFoot);
  engine.registerDecimalInputSymbol("attack.direction.preview.right_foot", &attackDirectionPreviewRFoot);

  // action selection
  for(int i = 0; i < KickActionModel::numOfActions; ++i)
  {
    string str("attack.action_type.");
    str.append(KickActionModel::getName((KickActionModel::ActionId)i));
    engine.registerEnumElement("attack.action_type", str.c_str(), i);
  }

  engine.registerEnumeratedInputSymbol("attack.best_action", "attack.action_type", &getBestAction);
  //engine.registerDecimalInputSymbol("attack.best_action.direction", &(getKickActionModel().rotation));

  // the position of the opponents free kick; it is only valid if x != 0 && y != 0
  engine.registerDecimalInputSymbol("freekick.pos.x", &freeKickPositionX);
  engine.registerDecimalInputSymbol("freekick.pos.y", &freeKickPositionY);

  // role selection
  for(int i = 0; i < Roles::numOfStaticRoles; ++i)
  {
    string str("role.static.");
    str.append(Roles::getName((Roles::Static)i));
    engine.registerEnumElement("role.static", str.c_str(), i);
  }

  engine.registerEnumeratedInputSymbol("strategy.role", "role.static", &getStaticRole);

  for(int i = 0; i < Roles::numOfDynamicRoles; ++i)
  {
    string str("role.dynamic.");
    str.append(Roles::getName((Roles::Dynamic)i));
    engine.registerEnumElement("role.dynamic", str.c_str(), i);
  }

  engine.registerEnumeratedInputSymbol("strategy.role_dynamic", "role.dynamic", &getDynamicRole);

  engine.registerDecimalInputSymbol("strategy.position.home.x", &getHomePositionX);
  engine.registerDecimalInputSymbol("strategy.position.home.y", &getHomePositionY);
  engine.registerDecimalInputSymbol("strategy.position.own.x", &getHomePositionOwnKickoffX);
  engine.registerDecimalInputSymbol("strategy.position.own.y", &getHomePositionOwnKickoffY);
  engine.registerDecimalInputSymbol("strategy.position.opp.x", &getHomePositionOppKickoffX);
  engine.registerDecimalInputSymbol("strategy.position.opp.y", &getHomePositionOppKickoffY);

  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_attack_direction","draw the attack direction", false);
  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_simpleDefenderPose","draw the position of the defender", false);
  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_goalie_defensive_pos","draw the position of the goalie", false);
  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_free_kick_pos","draw the position of the opponents free kick", false);

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

    goalieDefensivePosition = calculateGoalieDefensivePosition();
    DEBUG_REQUEST("XABSL:StrategySymbols:draw_goalie_defensive_pos",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 50);
      CIRCLE(goalieDefensivePosition.translation.x, goalieDefensivePosition.translation.y, 30);
    );

    retrieveFreeKickPosition();
    DEBUG_REQUEST("XABSL:StrategySymbols:draw_free_kick_pos",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 20);
      LINE(freeKickPosition.x-60,freeKickPosition.y,freeKickPosition.x+60,freeKickPosition.y);
      LINE(freeKickPosition.x,freeKickPosition.y-60,freeKickPosition.x,freeKickPosition.y+60);
    );
}//end execute

// TODO: check if the model is valid
// NOTE: what about the default position if the ball was not seen?
Vector2d StrategySymbols::calculateGoalieGuardPosition()
{
  Vector2d ballPos = getRobotPose() * getBallModel().positionPreview;

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


double StrategySymbols::goalieGuardPositionX()
{
  return theInstance->calculateGoalieGuardPosition().x;
}

double StrategySymbols::goalieGuardPositionY()
{
  return theInstance->calculateGoalieGuardPosition().y;
}


Pose2D StrategySymbols::calculateGoalieDefensivePosition()
{
    // Calculates the defensive position of the goalie
    // The position is on an ellipse within the penalty area.
    // The position is calculated in such a way, that a direct shot to the middle of the goal is prevented
    const Vector2d ball = theInstance->getRobotPose()*theInstance->getBallModel().position;
    const Vector2d c(ball.x - theInstance->getFieldInfo().xPosOwnGroundline, ball.y);
    // ball is "out"
    if(ball.x <= theInstance->getFieldInfo().xPosOwnGroundline || c.x == 0) {
        return Vector2d(theInstance->getFieldInfo().xPosOwnGroundline, 0);
    }
    // direct line from goal center to the ball: f(x) = mx
    double m = c.y / c.x;

    double a = theInstance->getFieldInfo().xPosOwnPenaltyArea - theInstance->getFieldInfo().xPosOwnGroundline;
    double b = theInstance->getFieldInfo().goalWidth / 2.0;
    // position on the ellipse: x = \frac{ab}{\sqrt{b^2 + m^2 a^2}}
    double x = (a*b) / std::sqrt(b*b + m*m * a*a);
    // y = m*x
    double y = m*x;
    //std::cout << "a="<<a<<" w="<<std::atan(a)<<" r="<
    Pose2D result(std::atan2(y,x),x + theInstance->getFieldInfo().xPosOwnGroundline, y);

    // make preview because this position is used to be walked to
    result = result + getMotionStatus().plannedMotion.hip;
  
    return result;
}

double StrategySymbols::goalieDefensivePositionX()
{
    return theInstance->goalieDefensivePosition.translation.x;
}

double StrategySymbols::goalieDefensivePositionY()
{
    return theInstance->goalieDefensivePosition.translation.y;
}

double StrategySymbols::goalieDefensivePositionA()
{
    return Math::toDegrees(theInstance->goalieDefensivePosition.rotation);
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
   return theInstance->getKickActionModel().bestAction;
}

void StrategySymbols::retrieveFreeKickPosition() {
    // check if a set play for the opponent was called
    if(lastSetPlay == GameData::set_none && !getPlayerInfo().kickoff) {
        // one of our teamates fouled an opponent
        if(getGameData().setPlay == GameData::pushing_free_kick)
        {
            // retrieve last pose of the player who has fouled
            for(unsigned int i = 0; i < getGameData().ownTeam.players.size(); ++i) {
                // player is now penalized, but wasn't before
                if(getGameData().ownTeam.players[i].penalty == naoth::GameData::player_pushing && penalties[i+1] == naoth::GameData::penalty_none) {
                    // get his last position
                    if(getTeamState().hasPlayer(i+1)) {
                        freeKickPosition = getTeamState().getPlayer(i+1).pose().translation;
                    }
                }
            }
        }
        // the opponent has a corner kick
        else if(getGameData().setPlay == GameData::corner_kick)
        {
            // we're moving away from both corners!
            // if we're near the actual corner kick-in position, we have to move away, if we're on the other side,
            // it is better to move closer to the actual corner kick-in position.
            freeKickPosition = getRobotPose().translation.y >= 0 ?
                        getFieldInfo().crossings[FieldInfo::ownCornerLeft].position :
                        getFieldInfo().crossings[FieldInfo::ownCornerRight].position;
        }
    }

    // reset free kick position after free kick is over
    if((lastSetPlay == GameData::pushing_free_kick && getGameData().setPlay != GameData::pushing_free_kick) ||
       (lastSetPlay == GameData::corner_kick && getGameData().setPlay != GameData::corner_kick))
    {
        freeKickPosition.x = 0.0;
        freeKickPosition.y = 0.0;
    }

    // update last setplay and penalties of own teammates
    lastSetPlay = getGameData().setPlay;
    for(unsigned int i = 0; i < getGameData().ownTeam.players.size(); ++i) {
        penalties[i+1] = getGameData().ownTeam.players[i].penalty;
    }
}

double StrategySymbols::freeKickPositionX() {
    return theInstance->freeKickPosition.x;
}

double StrategySymbols::freeKickPositionY() {
    return theInstance->freeKickPosition.y;
}

int StrategySymbols::getStaticRole() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return it->second.role;
    }
    return Roles::unknown;
}

int StrategySymbols::getDynamicRole() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return it->second.dynamic;
    }
    return Roles::none;
}

double StrategySymbols::getHomePositionX() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return theInstance->getRoleDecisionModel().getStaticRolePosition(it->second.role).home.x;
    }
    // put unknown player on the "manual placement line"
    return theInstance->getFieldInfo().xPosOwnPenaltyArea;
}

double StrategySymbols::getHomePositionY() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return theInstance->getRoleDecisionModel().getStaticRolePosition(it->second.role).home.y;
    }
    // put unknown player on the "manual placement line"
    return theInstance->getFieldInfo().yLength/(theInstance->getPlayerInfo().playersPerTeam+2)*theInstance->getPlayerInfo().playerNumber-(theInstance->getFieldInfo().yLength*0.5);
}

double StrategySymbols::getHomePositionOwnKickoffX() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return theInstance->getRoleDecisionModel().getStaticRolePosition(it->second.role).own.x;
    }
    // put unknown player on the "manual placement line"
    return theInstance->getFieldInfo().xPosOwnPenaltyArea;
}

double StrategySymbols::getHomePositionOwnKickoffY() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return theInstance->getRoleDecisionModel().getStaticRolePosition(it->second.role).own.y;
    }
    // put unknown player on the "manual placement line"
    return theInstance->getFieldInfo().yLength/(theInstance->getPlayerInfo().playersPerTeam+2)*theInstance->getPlayerInfo().playerNumber-(theInstance->getFieldInfo().yLength*0.5);
}

double StrategySymbols::getHomePositionOppKickoffX() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return theInstance->getRoleDecisionModel().getStaticRolePosition(it->second.role).opp.x;
    }
    // put unknown player on the "manual placement line"
    return theInstance->getFieldInfo().xPosOwnPenaltyArea;
}

double StrategySymbols::getHomePositionOppKickoffY() {
    const auto& it = theInstance->getRoleDecisionModel().roles.find(theInstance->getPlayerInfo().playerNumber);
    if(it != theInstance->getRoleDecisionModel().roles.cend()) {
        return theInstance->getRoleDecisionModel().getStaticRolePosition(it->second.role).opp.y;
    }
    // put unknown player on the "manual placement line"
    return theInstance->getFieldInfo().yLength/(theInstance->getPlayerInfo().playersPerTeam+2)*theInstance->getPlayerInfo().playerNumber-(theInstance->getFieldInfo().yLength*0.5);
}
