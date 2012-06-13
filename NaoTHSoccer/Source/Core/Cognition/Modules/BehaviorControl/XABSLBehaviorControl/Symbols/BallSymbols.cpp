/**
* @file BallSymbols.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BallSymbols
*/

#include "BallSymbols.h"
#include "Tools/Math/Common.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugBufferedOutput.h"

void BallSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("ball.radius", &fieldInfo.ballRadius);

  engine.registerDecimalInputSymbol("ball.x", &ballModel.position.x);
  engine.registerDecimalInputSymbol("ball.y", &ballModel.position.y);
  engine.registerDecimalInputSymbol("ball.preview.x", &ballModel.positionPreview.x);
  engine.registerDecimalInputSymbol("ball.preview.y", &ballModel.positionPreview.y);
  engine.registerDecimalInputSymbol("ball.left_foot.preview.x", &ballModel.positionPreviewInLFoot.x);
  engine.registerDecimalInputSymbol("ball.left_foot.preview.y", &ballModel.positionPreviewInLFoot.y);
  engine.registerDecimalInputSymbol("ball.right_foot.preview.x", &ballModel.positionPreviewInRFoot.x);
  engine.registerDecimalInputSymbol("ball.right_foot.preview.y", &ballModel.positionPreviewInRFoot.y);

  engine.registerDecimalInputSymbol("ball.time_since_last_seen", &getBallTimeSinceLastSeen);
  engine.registerDecimalInputSymbol("ball.time_seen", &getBallTimeSeen);
  engine.registerDecimalInputSymbol("ball.speed.x", &ballModel.speed.x);
  engine.registerDecimalInputSymbol("ball.speed.y", &ballModel.speed.y);
  engine.registerDecimalInputSymbol("ball.speed", &getBallSpeed);
  engine.registerDecimalInputSymbol("ball.future.x", &getFutureBallPosX);
  engine.registerDecimalInputSymbolDecimalParameter("ball.future.x", "ball.future.x.t", &futureBallPosX_t);
  engine.registerDecimalInputSymbol("ball.future.y", &getFutureBallPosY);
  engine.registerDecimalInputSymbolDecimalParameter("ball.future.y", "ball.future.y.t", &futureBallPosY_t);
  engine.registerDecimalInputSymbol("ball.future.distance", &getFutureBallDistance);
  engine.registerDecimalInputSymbolDecimalParameter("ball.future.distance", "ball.future.distance.t", &futureBallDistance_t);

  engine.registerDecimalInputSymbol("ball.distance", &getBallDistance);
  engine.registerDecimalInputSymbol("ball.angle", &getBallAngle);

  engine.registerDecimalInputSymbol("ball.percept.x", &ballPercept.bearingBasedOffsetOnField.x);
  engine.registerDecimalInputSymbol("ball.percept.y", &ballPercept.bearingBasedOffsetOnField.y);

  engine.registerBooleanInputSymbol("ball.was_seen", &ballPercept.ballWasSeen);
  engine.registerDecimalInputSymbol("ball.center_in_image.x", &ballPercept.centerInImage.x);
  engine.registerDecimalInputSymbol("ball.center_in_image.y", &ballPercept.centerInImage.y);

  engine.registerDecimalInputSymbol("ball.position.field.x", &ballPositionField.x);
  engine.registerDecimalInputSymbol("ball.position.field.y", &ballPositionField.y);

  engine.registerDecimalInputSymbol("ball.team.time_since_last_update", &getTeamBallTimeSinceLastUpdate);
  engine.registerDecimalInputSymbol("ball.team.position.x", &teamBallModel.position.x);
  engine.registerDecimalInputSymbol("ball.team.position.y", &teamBallModel.position.y);

  engine.registerDecimalInputSymbol("ball.left_foot.x", &ballLeftFoot.x);
  engine.registerDecimalInputSymbol("ball.left_foot.y", &ballLeftFoot.y);
  engine.registerDecimalInputSymbol("ball.right_foot.x", &ballRightFoot.x);
  engine.registerDecimalInputSymbol("ball.right_foot.y", &ballRightFoot.y);

  
  // "Pose behind the ball in attackdirection with distance"
  engine.registerDecimalInputSymbol("posBehindBallPreview.x", &getPosBehindBallFutureX);
  engine.registerDecimalInputSymbolDecimalParameter("posBehindBallPreview.x", "distanceToBall", &distance);

  engine.registerDecimalInputSymbol("posBehindBallPreview.y", &getPosBehindBallFutureY);
  engine.registerDecimalInputSymbolDecimalParameter("posBehindBallPreview.y", "distanceToBall", &distance);
  
  engine.registerDecimalInputSymbol("posBehindBallPreview.rot", &getPosBehindBallFutureRotation);
  engine.registerDecimalInputSymbolDecimalParameter("posBehindBallPreview.rot", "distanceToBall", &distance);


  DEBUG_REQUEST_REGISTER("XABSL:BallSymbols:ballLeftFoot", "draw the ball model in left foot's coordinates on field", false);
  DEBUG_REQUEST_REGISTER("XABSL:BallSymbols:ballRightFoot", "draw the ball model in right foot's coordinates on field", false);
  DEBUG_REQUEST_REGISTER("XABSL:StrategySymbols:draw_position_behind_ball", "draw the point behind the ball seen from the opp goal on field", false);

}//end registerSymbols


void BallSymbols::execute()
{
  // calculate the global position of the ball on the field
  ballPositionField = robotPose*ballModel.position;

  // transform the ball position into the feet coordinates
  const Pose3D& lFoot = kinematicChain.theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = kinematicChain.theLinks[KinematicChain::RFoot].M;
  
  Pose2D lFootPose(lFoot.rotation.getZAngle(), lFoot.translation.x, lFoot.translation.y);
  Pose2D rFootPose(rFoot.rotation.getZAngle(), rFoot.translation.x, rFoot.translation.y);

  ballLeftFoot  = lFootPose/ballModel.position;
  ballRightFoot = rFootPose/ballModel.position;

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

  // draw the position behind the ball (seen from attack direction)
  DEBUG_REQUEST("KalmanFilterBallLocator:pos_behind_ball",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 20);
    // TRANSLATION(getRobotPose().translation.x, getRobotPose().translation.y);
    // ROTATION(getRobotPose().rotation);

    // Vector2<double> targetDir = getRawAttackDirection().attackDirection;
    // targetDir.normalize(200);

    ARROW(
          posBehindBall.translation.x,
          posBehindBall.translation.y,
          getBallModel().positionPreview.x,
          getBallModel().positionPreview.y
          );

    // ROTATION(-getRobotPose().rotation);
    // TRANSLATION(-getRobotPose().translation.x, -getRobotPose().translation.y);
  );

}//end update


BallSymbols* BallSymbols::theInstance = NULL;


double BallSymbols::getBallSpeed()
{
  return theInstance->ballModel.speed.abs();
}

double BallSymbols::getBallDistance()
{
  return theInstance->ballModel.position.abs();
}//end getBallDistance

double BallSymbols::getFutureBallPosX()
{
  int clipped = Math::clamp((int) theInstance->futureBallPosX_t, 0, BALLMODEL_MAX_FUTURE_SECONDS);
  return theInstance->ballModel.futurePosition[clipped].x;
}//end getBallDistance

double BallSymbols::getFutureBallPosY()
{
  int clipped = Math::clamp((int) theInstance->futureBallPosX_t, 0, BALLMODEL_MAX_FUTURE_SECONDS);
  return theInstance->ballModel.futurePosition[clipped].y;
}//end getBallDistance

double BallSymbols::getFutureBallDistance()
{
  int clipped = Math::clamp((int) theInstance->futureBallPosX_t, 0, BALLMODEL_MAX_FUTURE_SECONDS);
  return theInstance->ballModel.futurePosition[clipped].abs();
}//end getBallDistance

double BallSymbols::getBallAngle()
{
  return Math::toDegrees(theInstance->ballModel.position.angle());
}//end getBallAngle

double BallSymbols::getBallTimeSinceLastSeen()
{
  return theInstance->frameInfo.getTimeSince(
    theInstance->ballModel.frameInfoWhenBallWasSeen.getTime());
}//end getBallTimeSinceLastSeen

double BallSymbols::getBallTimeSeen()
{
  return theInstance->ballModel.timeBallIsSeen;
}//end getBallTimeSeen

double BallSymbols::getTeamBallTimeSinceLastUpdate()
{
  return theInstance->frameInfo.getTimeSince(theInstance->teamBallModel.time);
}//end getTeamBallTimeSinceLastUpdate

void BallSymbols::calculatePosBehindBallFuture()
{
  // for better readability
  Vector2 <double> ball = theInstance->ballModel.positionPreview;
  double attack_dir = theInstance->getSoccerStrategy().attackDirection.angle();
  double distance = theInstance->distance;

  // ball.preview.x - 200*cos(angle=attack.direction), 
  theInstance->posBehindBall.translation.x = ball.x - cos(attack_dir)*distance;
  
  //  y = ball.preview.y - sin(angle = attack_direction)*200 // clip(value=attack.direction, min=-90 ,max=90)???
  theInstance->posBehindBall.translation.y = ball.y - sin(attack_dir)*distance;

  // rot = atan2(y = ball.preview.y + sin(angle=attack.direction)*200, x = ball.preview.x - cos(angle=attack.direction)*200)
  theInstance->posBehindBall.rotation = atan2(ball.y + sin(attack_dir)*distance, ball.x - cos(attack_dir)*distance);
}

double BallSymbols::getPosBehindBallFutureX()
{
  calculatePosBehindBallFuture();
  return theInstance->posBehindBall.translation.x;
}
double BallSymbols::getPosBehindBallFutureY()
{
  calculatePosBehindBallFuture();
  return theInstance->posBehindBall.translation.x;
}
double BallSymbols::getPosBehindBallFutureRotation()
{
  calculatePosBehindBallFuture();
  return Math::toDegrees(theInstance->posBehindBall.rotation);
}