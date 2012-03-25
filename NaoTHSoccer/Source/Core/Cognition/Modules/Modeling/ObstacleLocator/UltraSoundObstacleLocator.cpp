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

// in accordance to the Aldeberan documentation and the raw values
const double UltraSoundObstacleLocator::invalidDistanceValue =  2550.0;
const double UltraSoundObstacleLocator::maxValidDistance = 500.0;
const double UltraSoundObstacleLocator::minBlockedDistance = 350.0;

UltraSoundObstacleLocator::UltraSoundObstacleLocator()
  : wasFrontBlockedInLastFrame(false)
{

  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawObstacles", "draw the modelled Obstacle on the field", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:setMode_0", "left Transmitter left Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:setMode_1", "left Transmitter right Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:setMode_2", "right Transmitter left Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:setMode_3", "right Transmitter right Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:setMode_4", "left and right Transmitter and Receiver in two captures with one command", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:setMode_12", "left and right Transmitter and Receiver in two captures with one command and two transmitters at same time", false);
}

void UltraSoundObstacleLocator::execute()
{
  getObstacleModel().leftDistance = invalidDistanceValue;
  getObstacleModel().rightDistance = invalidDistanceValue;
  getObstacleModel().frontDistance = invalidDistanceValue;

  fillBuffer();

  // Compute mean of seen obstacles and use that data as estimated USObstacle  
  // in local obstacle model
  provideToLocalObstacleModel();

  //Draw ObstacleModel
  drawObstacleModel();

  unsigned int mode = UltraSoundSendData::TWO_CAPTURES + UltraSoundSendData::TWO_TRANSMITTERS;

  DEBUG_REQUEST("UltraSoundObstacleLocator:setMode_0",
    mode = 0;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:setMode_2",
    mode = 2;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:setMode_3",
    mode = 3;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:setMode_4",
    mode = 4;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:setMode_12",
    mode = 12;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:setMode_1",
    mode = 1;
  );

  // Update mode 
  if(mode != getUltraSoundSendData().mode)
  {
    getUltraSoundSendData().setMode(mode);
  }

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

void UltraSoundObstacleLocator::fillBuffer()
{
  bufferLeft.add(getUltraSoundReceiveData().dataLeft[UltraSoundData::echo_0] * 1000.0);
  bufferRight.add(getUltraSoundReceiveData().dataRight[UltraSoundData::echo_0] * 1000.0);
}

void UltraSoundObstacleLocator::provideToLocalObstacleModel()
{
  double sum = 0.0;
  int count = 0;
  for(int i=0; i < bufferLeft.getNumberOfEntries(); i++)
  {
    if(bufferLeft[i] <= maxValidDistance)
    {
      sum += bufferLeft[i];
      count++;
    }
  }
  ObstacleModel& model = getObstacleModel();

  if(count == 0)
  {
    model.leftDistance = invalidDistanceValue;
  }
  else
  {
    model.leftDistance = sum / (double) count;
  }

  sum = 0.0;
  count = 0;
  for(int i=0; i < bufferRight.getNumberOfEntries(); i++)
  {
    if(bufferRight[i] <= maxValidDistance)
    {
      sum += bufferRight[i];
      count++;
    }
  }
  if(count == 0)
  {
    model.rightDistance = invalidDistanceValue;
  }
  else
  {
    model.rightDistance = sum / (double) count;
  }

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

