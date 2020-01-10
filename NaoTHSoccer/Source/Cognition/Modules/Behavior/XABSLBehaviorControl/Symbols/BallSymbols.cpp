/**
* @file BallSymbols.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BallSymbols
*/

#include "BallSymbols.h"

void BallSymbols::registerSymbols(xabsl::Engine& engine)
{
  // constant
  engine.registerDecimalInputSymbol("ball.radius", &getFieldInfo().ballRadius);

  // percept
  engine.registerDecimalInputSymbol("ball.percept.x", &ballPerceptPos.x);
  engine.registerDecimalInputSymbol("ball.percept.y", &ballPerceptPos.y);
  engine.registerBooleanInputSymbol("ball.percept.was_seen", &ballPerceptSeen);

  // model
  engine.registerDecimalInputSymbol("ball.x", &getBallModel().position.x);
  engine.registerDecimalInputSymbol("ball.y", &getBallModel().position.y);
  engine.registerDecimalInputSymbol("ball.distance", &getBallDistance);
  engine.registerDecimalInputSymbol("ball.angle", &getBallAngle);
  engine.registerDecimalInputSymbol("ball.speed.x", &getBallModel().speed.x);
  engine.registerDecimalInputSymbol("ball.speed.y", &getBallModel().speed.y);

  engine.registerDecimalInputSymbol("ball.last_known.x", &last_known_ball_preview.x);
  engine.registerDecimalInputSymbol("ball.last_known.y", &last_known_ball_preview.y);  
  
  // HACK: the future is calculated incorrectly, assume the position of the ball for now
  //engine.registerDecimalInputSymbol("ball.position_at_rest.x", &getBallModel().position_at_rest.x);
  //engine.registerDecimalInputSymbol("ball.position_at_rest.y", &getBallModel().position_at_rest.y);
  engine.registerDecimalInputSymbol("ball.position_at_rest.x", &getBallModel().position.x);
  engine.registerDecimalInputSymbol("ball.position_at_rest.y", &getBallModel().position.y);

  // HACK: the future is calculated incorrectly, assume the position of the ball for now
  //engine.registerDecimalInputSymbol("ball.future.preview.x", &futureBallPreview.x);
  //engine.registerDecimalInputSymbol("ball.future.preview.y", &futureBallPreview.y);
  engine.registerDecimalInputSymbol("ball.future.preview.x", &getBallModel().positionPreview.x);
  engine.registerDecimalInputSymbol("ball.future.preview.y", &getBallModel().positionPreview.y);

  // FIXME: this needs a correct future ball
  engine.registerDecimalInputSymbol("ball.interception.preview.x", &interceptionPointPreview.x);
  engine.registerDecimalInputSymbol("ball.interception.preview.y", &interceptionPointPreview.y);

  engine.registerBooleanInputSymbol("ball.know_where_itis", &getBallModel().knows);
  // HACK: right now know_where_itis is the same as see_where_itis
  engine.registerBooleanInputSymbol("ball.see_where_itis", &getBallModel().knows);
  // TODO: old symbols indicating the quality of the seen ball - should we still use them?
  engine.registerDecimalInputSymbol("ball.time_since_last_seen", &getBallTimeSinceLastSeen);
  engine.registerDecimalInputSymbol("ball.time_seen", &getBallTimeSeen);

  // preview
  engine.registerDecimalInputSymbol("ball.preview.x", &getBallModel().positionPreview.x);
  engine.registerDecimalInputSymbol("ball.preview.y", &getBallModel().positionPreview.y);
  engine.registerDecimalInputSymbol("ball.preview.left_foot.x", &getBallModel().positionPreviewInLFoot.x);
  engine.registerDecimalInputSymbol("ball.preview.left_foot.y", &getBallModel().positionPreviewInLFoot.y);
  engine.registerDecimalInputSymbol("ball.preview.right_foot.x", &getBallModel().positionPreviewInRFoot.x);
  engine.registerDecimalInputSymbol("ball.preview.right_foot.y", &getBallModel().positionPreviewInRFoot.y);

  // global
  engine.registerDecimalInputSymbol("ball.position.field.x", &ballPositionField.x);
  engine.registerDecimalInputSymbol("ball.position.field.y", &ballPositionField.y);

  // team
  engine.registerBooleanInputSymbol("ball.team.is_valid", &getTeamBallModel().valid);
  engine.registerDecimalInputSymbol("ball.team.position.x", &getTeamBallModel().position.x);
  engine.registerDecimalInputSymbol("ball.team.position.y", &getTeamBallModel().position.y);
  engine.registerDecimalInputSymbol("ball.team.position_field.x", &getTeamBallModel().positionOnField.x);
  engine.registerDecimalInputSymbol("ball.team.position_field.y", &getTeamBallModel().positionOnField.y);
  engine.registerDecimalInputSymbol("ball.team.rmse", &getTeamBallModel().rmse);

  DEBUG_REQUEST_REGISTER("XABSL:BallSymbols:ballPerceptPropagated", "draw propagated ball percept", false);

}//end registerSymbols


void BallSymbols::execute()
{
  // calculate the global position of the ball on the field
  ballPositionField = getRobotPose()*getBallModel().position;


  if(theInstance->getMultiBallPercept().wasSeen()) {
    //HACK: look at the first ball percept in the list
    ballPerceptPos = theInstance->getMultiBallPercept().begin()->positionOnField;
    ballPerceptSeen = true;
  } else {
    // propagate the ball percept with the odometry
    Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
    ballPerceptPos = odometryDelta*ballPerceptPos;
    ballPerceptSeen = false;
  }

  DEBUG_REQUEST("XABSL:BallSymbols:ballPerceptPropagated",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 20);
    CIRCLE(ballPerceptPos.x, ballPerceptPos.y, 50);
  );

  futureBallPreview = getMotionStatus().plannedMotion.hip / getBallModel().position_at_rest;
  last_known_ball_preview = getMotionStatus().plannedMotion.hip / getBallModel().last_known_ball;  

  Math::LineSegment ballLine(getBallModel().position, getBallModel().position_at_rest);
  interceptionPointPreview = getMotionStatus().plannedMotion.hip / ballLine.projection(Vector2d());

  lastRobotOdometry = getOdometryData();
}//end execute


BallSymbols* BallSymbols::theInstance = NULL;


double BallSymbols::getBallDistance() {
  return theInstance->getBallModel().position.abs();
}

double BallSymbols::getBallAngle() {
  return Math::toDegrees(theInstance->getBallModel().position.angle());
}

double BallSymbols::getBallTimeSinceLastSeen() {
  return theInstance->getFrameInfo().getTimeSince(theInstance->getBallModel().getFrameInfoWhenBallWasSeen().getTime());
}

double BallSymbols::getBallTimeSeen() {
  return theInstance->getBallModel().getTimeBallIsSeen();
}
