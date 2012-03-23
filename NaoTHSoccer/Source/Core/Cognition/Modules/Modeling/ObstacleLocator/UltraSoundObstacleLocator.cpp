/**
 * @file ObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
 * @author <a href="mailto:krienelk@informatik.hu-berlin.de">Dominik Krienelke</a>
 * Implementation of class ObstacleLocator
 */

//#include "Tools/Math/Pose2D.h"
#include <limits>

#include "UltraSoundObstacleLocator.h"

UltraSoundObstacleLocator::UltraSoundObstacleLocator()
  : wasFrontBlockedInLastFrame(false)
{
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawObstacles", "draw the modelled Obstacle on the field", false);
}

void UltraSoundObstacleLocator::execute()
{
  getObstacleModel().leftDistance = std::numeric_limits<double>::max();
  getObstacleModel().rightDistance = std::numeric_limits<double>::max();
  getObstacleModel().frontDistance = std::numeric_limits<double>::max();

  // negative odometry

  // Compute mean of seen obstacles and use that data as estimated USObstacle  
  // in local obstacle model
  provideToLocalObstacleModel();

  //Draw ObstacleModel
  drawObstacleModel();

}//end execute



void UltraSoundObstacleLocator::drawObstacleModel()
{
  // Inits
  DEBUG_REQUEST("UltraSoundObstacleLocator:drawObstacles",
    FIELD_DRAWING_CONTEXT;
    DebugDrawings::Color colorLeft(ColorClasses::blue);
    DebugDrawings::Color colorRight(ColorClasses::red);
    colorLeft[DebugDrawings::Color::alpha] = 0.5;
    colorRight[DebugDrawings::Color::alpha] = 0.5;

    // draw raw sensor data
    for(unsigned int i = 0; i < UltraSoundData::numOfUSEcho; i++)
    {

      double distLeft = getUltraSoundReceiveData().dataLeft[i] * 1000.0;
      double distRight = getUltraSoundReceiveData().dataRight[i] * 1000.0;

      PEN(colorLeft, 25);
      CIRCLE(
        distLeft,
        distLeft,
        50);

      PEN(colorRight, 25);
      CIRCLE(
        distRight,
        -distRight,
        50);
    }//end for

    // draw model
    colorLeft[DebugDrawings::Color::alpha] = 1.0;
    colorRight[DebugDrawings::Color::alpha] = 1.0;

    PEN(colorLeft, 50);
    CIRCLE(
      getObstacleModel().leftDistance,
      getObstacleModel().leftDistance,
      50);

    PEN(colorRight, 50);
    CIRCLE(
      getObstacleModel().rightDistance,
      -getObstacleModel().rightDistance,
      50);

    DebugDrawings::Color colorFront(ColorClasses::black);
    PEN(colorFront, 50);
    LINE(getObstacleModel().frontDistance, -200, getObstacleModel().frontDistance, 200);

  );
} //end drawObstacleMode

void UltraSoundObstacleLocator::provideToLocalObstacleModel()
{
  std::vector<double> left;
  std::vector<double> right;

  // add obstacles based on left/right sensordata and filter out too far away obstacles
  for(unsigned int i = 0; i < UltraSoundData::numOfUSEcho; i++)
  {

    double distanceLeft = getUltraSoundReceiveData().dataLeft[i] * 1000.0;
    if(distanceLeft <= maxValidDistance)
    {
      left.push_back(distanceLeft);
    }

    double distanceRight = getUltraSoundReceiveData().dataRight[i] * 1000.0;
    if(distanceRight <= maxValidDistance)
    {
      right.push_back(distanceRight);
    }
  }//end for each raw sensor input

  ObstacleModel& model = getObstacleModel();
  model.leftDistance = getNearestDistance(left);
  model.rightDistance = getNearestDistance(right);
  if(model.leftDistance <= maxValidDistance && model.rightDistance <= maxValidDistance)
  {
    model.frontDistance = std::min(model.leftDistance, model.rightDistance);
  }

  if(model.frontDistance < minBlockedDistance)
  {
    if(!wasFrontBlockedInLastFrame)
    {
      timeWhenFrontBlockStarted = getFrameInfo();
    }
    wasFrontBlockedInLastFrame = true;
    model.blockedTime = getFrameInfo().getTimeSince(timeWhenFrontBlockStarted.getTime());
  }
  else
  {
    model.blockedTime = -1;
    wasFrontBlockedInLastFrame = false;
  }

} //end provideToLocalObstacleModel

double UltraSoundObstacleLocator::getNearestDistance(const std::vector<double> &dists)
{
  if(dists.size() > 0)
  {
    // the first one is always the smallest TODO: check if that assumption made in the naoqi doc always holds
    return dists[0];
  }

  return std::numeric_limits<double>::max();

}
