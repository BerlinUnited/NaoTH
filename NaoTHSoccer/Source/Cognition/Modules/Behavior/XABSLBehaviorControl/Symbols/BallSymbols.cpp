/**
* @file BallSymbols.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BallSymbols
*/

#include "BallSymbols.h"

#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugModify.h>
#include <Tools/Debug/DebugPlot.h>

void BallSymbols::registerSymbols(xabsl::Engine& engine)
{
  // constant
  engine.registerDecimalInputSymbol("ball.radius", &getFieldInfo().ballRadius);

  // percept
  engine.registerDecimalInputSymbol("ball.percept.x", &ballPerceptPos.x);
  engine.registerDecimalInputSymbol("ball.percept.y", &ballPerceptPos.y);
  engine.registerBooleanInputSymbol("ball.was_seen", &ballPerceptSeen);

  engine.registerBooleanInputSymbol("ball.know_where_itis", &ball_know_where_itis);

  // model
  engine.registerDecimalInputSymbol("ball.x", &getBallModel().position.x);
  engine.registerDecimalInputSymbol("ball.y", &getBallModel().position.y);
  engine.registerDecimalInputSymbol("ball.distance", &getBallDistance);
  engine.registerDecimalInputSymbol("ball.angle", &getBallAngle);
  engine.registerDecimalInputSymbol("ball.speed.x", &getBallModel().speed.x);
  engine.registerDecimalInputSymbol("ball.speed.y", &getBallModel().speed.y);
  
  engine.registerDecimalInputSymbol("ball.left_foot.x", &ballLeftFoot.x);
  engine.registerDecimalInputSymbol("ball.left_foot.y", &ballLeftFoot.y);
  engine.registerDecimalInputSymbol("ball.right_foot.x", &ballRightFoot.x);
  engine.registerDecimalInputSymbol("ball.right_foot.y", &ballRightFoot.y);
  
  // preview
  engine.registerDecimalInputSymbol("ball.preview.x", &getBallModel().positionPreview.x);
  engine.registerDecimalInputSymbol("ball.preview.y", &getBallModel().positionPreview.y);
  engine.registerDecimalInputSymbol("ball.preview.left_foot.x", &getBallModel().positionPreviewInLFoot.x);
  engine.registerDecimalInputSymbol("ball.preview.left_foot.y", &getBallModel().positionPreviewInLFoot.y);
  engine.registerDecimalInputSymbol("ball.preview.right_foot.x", &getBallModel().positionPreviewInRFoot.x);
  engine.registerDecimalInputSymbol("ball.preview.right_foot.y", &getBallModel().positionPreviewInRFoot.y);
  
  engine.registerDecimalInputSymbol("ball.time_since_last_seen", &getBallTimeSinceLastSeen);
  engine.registerDecimalInputSymbol("ball.time_seen", &getBallTimeSeen);
  
  // global
  engine.registerDecimalInputSymbol("ball.position.field.x", &ballPositionField.x);
  engine.registerDecimalInputSymbol("ball.position.field.y", &ballPositionField.y);

  // team
  engine.registerDecimalInputSymbol("ball.team.time_since_last_update", &getTeamBallTimeSinceLastUpdate);
  engine.registerDecimalInputSymbol("ball.team.position.x", &getTeamBallModel().position.x);
  engine.registerDecimalInputSymbol("ball.team.position.y", &getTeamBallModel().position.y);

  engine.registerDecimalInputSymbol("ball.team.goalie.time_since_last_update", &getTeamBallGoalieTimeSinceLastUpdate);
  engine.registerDecimalInputSymbol("ball.team.goalie.position.x", &getTeamBallModel().goaliePosition.x);
  engine.registerDecimalInputSymbol("ball.team.goalie.position.y", &getTeamBallModel().goaliePosition.y);

  engine.registerDecimalInputSymbol("ball.team.striker.time_since_last_update", &getTeamBallStrikerTimeSinceLastUpdate);
  engine.registerDecimalInputSymbol("ball.team.striker.position.x", &getTeamBallModel().strikerPosition.x);
  engine.registerDecimalInputSymbol("ball.team.striker.position.y", &getTeamBallModel().strikerPosition.y);


  DEBUG_REQUEST_REGISTER("XABSL:BallSymbols:ballLeftFoot", "draw the ball model in left foot's coordinates on field", false);
  DEBUG_REQUEST_REGISTER("XABSL:BallSymbols:ballRightFoot", "draw the ball model in right foot's coordinates on field", false);
  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_position_behind_ball", "draw the point behind the ball seen from the opp goal on field", false);

}//end registerSymbols


void BallSymbols::execute()
{
  // calculate the global position of the ball on the field
  ballPositionField = getRobotPose()*getBallModel().position;

  // transform the ball position into the feet coordinates
  const Pose3D& lFoot = getKinematicChain().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChain().theLinks[KinematicChain::RFoot].M;
  
  Pose2D lFootPose(lFoot.rotation.getZAngle(), lFoot.translation.x, lFoot.translation.y);
  Pose2D rFootPose(rFoot.rotation.getZAngle(), rFoot.translation.x, rFoot.translation.y);

  ballLeftFoot  = lFootPose/getBallModel().position;
  ballRightFoot = rFootPose/getBallModel().position;

  DEBUG_REQUEST("XABSL:BallSymbols:ballLeftFoot", 
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    CIRCLE(ballLeftFoot.x, ballLeftFoot.y, 32.5);
    PLOT("XABSL:BallSymbols:ballLeftFoot:x", ballLeftFoot.x);
    PLOT("XABSL:BallSymbols:ballLeftFoot:y", ballLeftFoot.y);
  );

  DEBUG_REQUEST("XABSL:BallSymbols:ballRightFoot", 
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 20);
    CIRCLE(ballRightFoot.x, ballRightFoot.y, 32.5);
    PLOT("XABSL:BallSymbols:ballRightFoot:x", ballRightFoot.x);
    PLOT("XABSL:BallSymbols:ballRightFoot:y", ballRightFoot.y);
  );


  ballPerceptSeen = false;

  if(theInstance->getBallPercept().ballWasSeen) {
    ballPerceptPos = getBallPercept().bearingBasedOffsetOnField;
    ballPerceptSeen = true;
  } else if(theInstance->getBallPerceptTop().ballWasSeen) {
    ballPerceptPos = getBallPerceptTop().bearingBasedOffsetOnField;
    ballPerceptSeen = true;
  }


  {
    ball_seen_filter.setParameter(parameters.ball_seen_filter.g0, parameters.ball_seen_filter.g1);
    ball_seen_filter.update(ballPerceptSeen);

    PLOT("XABSL:BallSymbols:ball_seen_likelihood", ball_seen_filter.value());

    // hysteresis
    ball_know_where_itis = ball_seen_filter.value() > (ball_know_where_itis?0.3:0.7);
  }
}//end update


BallSymbols* BallSymbols::theInstance = NULL;



double BallSymbols::getBallDistance() {
  return theInstance->getBallModel().position.abs();
}

double BallSymbols::getBallAngle() {
  return Math::toDegrees(theInstance->getBallModel().position.angle());
}

double BallSymbols::getBallTimeSinceLastSeen() {
  return theInstance->getFrameInfo().getTimeSince(theInstance->getBallModel().frameInfoWhenBallWasSeen.getTime());
}

double BallSymbols::getBallTimeSeen() {
  return theInstance->getBallModel().timeBallIsSeen;
}

double BallSymbols::getTeamBallTimeSinceLastUpdate() {
  return theInstance->getFrameInfo().getTimeSince(theInstance->getTeamBallModel().time);
}

double BallSymbols::getTeamBallGoalieTimeSinceLastUpdate() {
  return theInstance->getFrameInfo().getTimeSince(theInstance->getTeamBallModel().goalieTime);
}

double BallSymbols::getTeamBallStrikerTimeSinceLastUpdate() {
  return theInstance->getFrameInfo().getTimeSince(theInstance->getTeamBallModel().strikerTime);
}

