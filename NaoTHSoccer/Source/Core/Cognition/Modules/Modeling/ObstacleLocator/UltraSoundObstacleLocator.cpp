/**
 * @file ObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
 * @author <a href="mailto:krienelk@informatik.hu-berlin.de">Dominik Krienelke</a>
 * Implementation of class ObstacleLocator
 */

//#include "Tools/Math/Pose2D.h"


#include "UltraSoundObstacleLocator.h"

UltraSoundObstacleLocator::UltraSoundObstacleLocator()
{
  lastTimeObstacleWasSeen = 0;
  ageThreshold = 5000; // 5 seconds
  getObstacleModel().someObstacleWasSeen = false;
  lastRobotOdometry = getOdometryData();

  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawObstacleBuffer", "draw the modelled Obstacle on the field", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawObstacleBufferNumbers", "draw the values of modelled Obstacle on the field", false);

  maxValidDistance = 0.80f;
  minValidDistance = 0.15f;
  usOpeningAngle = Math::fromDegrees(30.0f);

}

void UltraSoundObstacleLocator::execute()
{
  getObstacleModel().obstaclesLeft.clear();
  getObstacleModel().obstaclesRight.clear();
  getObstacleModel().someObstacleWasSeen = false;

  // negative odometry
  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  lastRobotOdometry = getOdometryData();

  // remove old percepts
  ageBuffer();
  //ageGrid();
  
  // update by odometry
  updateByOdometry(odometryDelta);

  // add obstacles to buffer
  updateBuffer();

  // Compute mean of seen obstacles and use that data as estimated USObstacle  
  // in local obstacle model
  provideToLocalObstacleModel();

  //Draw ObstacleModel
  drawObstacleModel();

}//end execute

void UltraSoundObstacleLocator::ageBuffer()
{
  if(obstacleBuffer.getNumberOfEntries() > 0 && 
    static_cast<unsigned int>(getFrameInfo().getTimeSince(obstacleBuffer.first().frameInfoObstacleWasSeen.getTime())) > ageThreshold)
  {
    obstacleBuffer.removeFirst();
  } 
}

void UltraSoundObstacleLocator::updateBuffer()
{
  // add obstacles based on left/right sensordate
  for(unsigned int i = 0; i < UltraSoundData::numOfUSEcho; i++)
  {
    break; // skip this for-loop for now
    if(getUltraSoundReceiveData().dataLeft[i] < maxValidDistance && getUltraSoundReceiveData().dataLeft[i] > minValidDistance)
    {
      ObstacleModel::Obstacle obstacleLeft;
      obstacleLeft.frameInfoObstacleWasSeen = getFrameInfo();
      obstacleLeft.distance = getUltraSoundReceiveData().dataLeft[i] * 1000;
      obstacleBuffer.add(obstacleLeft);
    }//end if

    if(getUltraSoundReceiveData().dataRight[i] < maxValidDistance && getUltraSoundReceiveData().dataRight[i] > minValidDistance)
    {
      ObstacleModel::Obstacle obstacleRight;
      obstacleRight.frameInfoObstacleWasSeen = getFrameInfo();
      obstacleRight.distance = getUltraSoundReceiveData().dataRight[i] * 1000;
      obstacleBuffer.add(obstacleRight);
    }//end if
  }//end for
} //end updateBuffer


void UltraSoundObstacleLocator::drawObstacleModel()
{
  // Inits
  DEBUG_REQUEST("UltraSoundObstacleLocator:drawObstacleBuffer",
    FIELD_DRAWING_CONTEXT;
    DebugDrawings::Color color(ColorClasses::blue);
    color[DebugDrawings::Color::alpha] = 1.0;

  // Draw obstacle buffer
    std::cout << "Drawing obstacle model:\n";
    for(int i = 0; i < obstacleBuffer.getNumberOfEntries(); i++)
    {
      color[DebugDrawings::Color::alpha] = 1.0 - ((double)i)/((double)obstacleBuffer.getNumberOfEntries());
      PEN(color, 50);
      CIRCLE(
        obstacleBuffer[i].distance,
        0,
        50);
    }//end for
  
    if(obstacleBuffer.getNumberOfEntries() > 10)
    {
      PEN("522100", 600);
      TEXT_DRAWING(getMean(), getMean()-300,getMean());
      PEN("522100", 50);
      CIRCLE(getMean(),
             getMean(),
             50);
    }
  );
} //end drawObstacleMode

void UltraSoundObstacleLocator::provideToLocalObstacleModel()
{
  // TODO: fill obstacleModel


} //end provideToLocalObstacleModel

double UltraSoundObstacleLocator::getMean()
{
  double mean;
  for(int i = 0; i < obstacleBuffer.getNumberOfEntries(); i++)
  {
    mean += obstacleBuffer[i].distance;
  }//end for

  return mean / ((double)obstacleBuffer.getNumberOfEntries());
}//end getMean

double UltraSoundObstacleLocator::getMinimum()
{
  double min;
  min = obstacleBuffer[0].distance;
  for(int i = 1; i < obstacleBuffer.getNumberOfEntries(); i++)
  {
    if(min > obstacleBuffer[i].distance)
      min = obstacleBuffer[i].distance;
  }//end for

  return min;
}//end getMin

void UltraSoundObstacleLocator::updateByOdometry(const Pose2D& odometryDelta)
{
  // TODO: update obstacle distance buffer by odometry
}//end updateByOdometry
