/**
 * @file LineSymbols.cpp
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class LineSymbols
 *
 * Currently not used
 * Experimental feature
 *
 * In certain situations it is better to use local features like lines to navigate.
 */

#include "LineSymbols.h"

#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugDrawings.h>

void LineSymbols::registerSymbols(xabsl::Engine& engine)
{

  //general line symbols
  engine.registerBooleanInputSymbol("line.was_seen", &linePercept.lineWasSeen); 
  engine.registerDecimalInputSymbol("line.time_since_last_seen", &getLineTimeSinceLastSeen); 

  //Symbols for the estimated orthogonal point of a seen line
  engine.registerDecimalInputSymbol("line.closest.estOrthPoint.x", &linePercept.estOrthPointOfClosestLine.x);
  engine.registerDecimalInputSymbol("line.closest.estOrthPoint.y", &linePercept.estOrthPointOfClosestLine.y);
  engine.registerDecimalInputSymbol("line.closest.estDistance", &getEstDistanceToOrthPoint);
  engine.registerDecimalInputSymbol("line.closest.estOrthPoint.angle", &getAngleToEstOrthPoint);

  //Symbols for the closest point of the closest seen line
  engine.registerDecimalInputSymbol("line.closest.closestPoint.x", &linePercept.closestPointOfClosestLine.x);
  engine.registerDecimalInputSymbol("line.closest.closestPoint.y", &linePercept.closestPointOfClosestLine.y);
  engine.registerDecimalInputSymbol("line.closest.distance", &getDistanceToClosestPoint);
  engine.registerDecimalInputSymbol("line.closest.closestPoint.angle", &getAngleToEstOrthPoint);
  engine.registerDecimalInputSymbol("line.closest.length", &linePercept.closestLineSeenLength);


  engine.registerDecimalInputSymbol("line.closest.projection.x", &linePointsBufferMean.x);
  engine.registerDecimalInputSymbol("line.closest.projection.y", &linePointsBufferMean.y);


  DEBUG_REQUEST_REGISTER("LineSymbols:linePointsBuffer", "...", false);
}//end registerSymbols

LineSymbols* LineSymbols::theInstance = NULL;

void LineSymbols::execute()
{
  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  for(int i = 0; i < linePointsBuffer.size(); i++)
  {
    linePointsBuffer[i] = odometryDelta*linePointsBuffer[i];
  }

  if(getLinePercept().lineWasSeen)
  {
    linePointsBuffer.add(linePercept.estOrthPointOfClosestLine);
  }

  if(linePointsBuffer.size() > 0)
  {
    linePointsBufferMean = Vector2d();
    for(int i = 0; i < linePointsBuffer.size(); i++) {
      linePointsBufferMean += linePointsBuffer[i];
    }
    linePointsBufferMean /= linePointsBuffer.size();
  }

  lastRobotOdometry = getOdometryData();

  DEBUG_REQUEST("LineSymbols:linePointsBuffer",
    for(int i = 0; i < linePointsBuffer.size(); i++)
    {
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 1);
      FILLOVAL(linePointsBuffer[i].x, linePointsBuffer[i].y, 10, 10);
    }
  );
}

double LineSymbols::getLineTimeSinceLastSeen()
{
  return theInstance->frameInfo.getTimeSince(
    theInstance->linePercept.frameInfoWhenLineWasSeen.getTime());

}//end getLineTimeSinceLastSeen

double LineSymbols::getEstDistanceToOrthPoint()
{
  return theInstance->linePercept.estOrthPointOfClosestLine.abs();

}//end getEstDistanceToOrthPoint

double LineSymbols::getAngleToEstOrthPoint()
{
  return Math::toDegrees(theInstance->linePercept.closestPointOfClosestLine.angle());

}//end getAngleToEstOrthPoint

double LineSymbols::getDistanceToClosestPoint()
{
  return theInstance->linePercept.closestPointOfClosestLine.abs();

}//end getDistanceToClosestPoint

double LineSymbols::getAngleToClosestPoint()
{
  return Math::toDegrees(theInstance->linePercept.estOrthPointOfClosestLine.angle());

}//end getAngleToClosestPoint
