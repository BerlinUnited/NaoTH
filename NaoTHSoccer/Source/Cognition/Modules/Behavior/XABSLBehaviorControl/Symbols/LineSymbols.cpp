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
  engine.registerBooleanInputSymbol("line.was_seen", &getLinePercept().lineWasSeen); 
  engine.registerDecimalInputSymbol("line.time_since_last_seen", &getLineTimeSinceLastSeen); 

  //Symbols for the estimated orthogonal point of a seen line
  engine.registerDecimalInputSymbol("line.closest.estOrthPoint.x", &getLinePercept().estOrthPointOfClosestLine.x);
  engine.registerDecimalInputSymbol("line.closest.estOrthPoint.y", &getLinePercept().estOrthPointOfClosestLine.y);
  engine.registerDecimalInputSymbol("line.closest.estDistance", &getEstDistanceToOrthPoint);
  engine.registerDecimalInputSymbol("line.closest.estOrthPoint.angle", &getAngleToEstOrthPoint);

  //Symbols for the closest point of the closest seen line
  engine.registerDecimalInputSymbol("line.closest.closestPoint.x", &getLinePercept().closestPointOfClosestLine.x);
  engine.registerDecimalInputSymbol("line.closest.closestPoint.y", &getLinePercept().closestPointOfClosestLine.y);
  engine.registerDecimalInputSymbol("line.closest.distance", &getDistanceToClosestPoint);
  engine.registerDecimalInputSymbol("line.closest.closestPoint.angle", &getAngleToEstOrthPoint);
  engine.registerDecimalInputSymbol("line.closest.length", &getLinePercept().closestLineSeenLength);


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
    linePointsBuffer.add(getLinePercept().estOrthPointOfClosestLine);
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
  return theInstance->getFrameInfo().getTimeSince(
    theInstance->getLinePercept().frameInfoWhenLineWasSeen.getTime());
}

double LineSymbols::getEstDistanceToOrthPoint()
{
  return theInstance->getLinePercept().estOrthPointOfClosestLine.abs();
}

double LineSymbols::getAngleToEstOrthPoint()
{
  return Math::toDegrees(theInstance->getLinePercept().closestPointOfClosestLine.angle());
}

double LineSymbols::getDistanceToClosestPoint()
{
  return theInstance->getLinePercept().closestPointOfClosestLine.abs();
}

double LineSymbols::getAngleToClosestPoint()
{
  return Math::toDegrees(theInstance->getLinePercept().estOrthPointOfClosestLine.angle());
}
