/**
* @file BallSymbols.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BallSymbols
*/

#include "BallSymbols2011.h"

#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugBufferedOutput.h>

void BallSymbols2011::registerSymbols(xabsl::Engine& engine)
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


  DEBUG_REQUEST_REGISTER("XABSL:BallSymbols:ballLeftFoot", "draw the ball model in left foot's coordinates on field", false);
  DEBUG_REQUEST_REGISTER("XABSL:BallSymbols:ballRightFoot", "draw the ball model in right foot's coordinates on field", false);

}//end registerSymbols


void BallSymbols2011::execute()
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

}//end update


BallSymbols2011* BallSymbols2011::theInstance = NULL;


double BallSymbols2011::getBallSpeed()
{
  return theInstance->ballModel.speed.abs();
}

double BallSymbols2011::getBallDistance()
{
  return theInstance->ballModel.position.abs();
}//end getBallDistance

double BallSymbols2011::getFutureBallPosX()
{
  int clipped = Math::clamp((int) theInstance->futureBallPosX_t, 0, BALLMODEL_MAX_FUTURE_SECONDS);
  return theInstance->ballModel.futurePosition[clipped].x;
}//end getBallDistance

double BallSymbols2011::getFutureBallPosY()
{
  int clipped = Math::clamp((int) theInstance->futureBallPosX_t, 0, BALLMODEL_MAX_FUTURE_SECONDS);
  return theInstance->ballModel.futurePosition[clipped].y;
}//end getBallDistance

double BallSymbols2011::getFutureBallDistance()
{
  int clipped = Math::clamp((int) theInstance->futureBallPosX_t, 0, BALLMODEL_MAX_FUTURE_SECONDS);
  return theInstance->ballModel.futurePosition[clipped].abs();
}//end getBallDistance

double BallSymbols2011::getBallAngle()
{
  return Math::toDegrees(theInstance->ballModel.position.angle());
}//end getBallAngle

double BallSymbols2011::getBallTimeSinceLastSeen()
{
  return theInstance->frameInfo.getTimeSince(
    theInstance->ballModel.frameInfoWhenBallWasSeen.getTime());
}//end getBallTimeSinceLastSeen

double BallSymbols2011::getBallTimeSeen()
{
  return theInstance->ballModel.timeBallIsSeen;
}//end getBallTimeSeen

double BallSymbols2011::getTeamBallTimeSinceLastUpdate()
{
  return theInstance->frameInfo.getTimeSince(theInstance->teamBallModel.time);
}//end getTeamBallTimeSinceLastUpdate
