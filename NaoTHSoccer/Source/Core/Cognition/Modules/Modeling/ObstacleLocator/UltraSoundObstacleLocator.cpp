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

// in accordance to the Aldeberan documentation and the raw values
const double UltraSoundObstacleLocator::invalidDistanceValue =  2550.0;
const double UltraSoundObstacleLocator::maxValidDistance = 500.0;

UltraSoundObstacleLocator::UltraSoundObstacleLocator()
  : wasFrontBlockedInLastFrame(false)
{

  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawObstacle", "draw the modelled Obstacle on the field", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawSensorData", "draw the measured echos", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawBuffer", "draw buffer of measurements", false);

  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:mode:CAPTURE_LEFT", "left Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:mode:CAPTURE_RIGHT", "right Receiver", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:mode:CAPTURE_BOTH", "left and right capture", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:mode:TRANSMIT_LEFT", "left Transmitter", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:mode:TRANSMIT_RIGHT", "right Transmitter", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:mode:TRANSMIT_BOTH", "left and right transmitter", false);

  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:mode:PERIODIC", "send US periodically", false);
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

  // default state
  unsigned int transmitter = UltraSoundSendData::TRANSMIT_LEFT;
  unsigned int receiver = UltraSoundSendData::CAPTURE_BOTH;

  DEBUG_REQUEST("UltraSoundObstacleLocator:mode:CAPTURE_LEFT",
    receiver = UltraSoundSendData::CAPTURE_LEFT;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:mode:CAPTURE_RIGHT",
    receiver = UltraSoundSendData::CAPTURE_RIGHT;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:mode:CAPTURE_BOTH",
    receiver = UltraSoundSendData::CAPTURE_BOTH;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:mode:TRANSMIT_LEFT",
    transmitter = UltraSoundSendData::TRANSMIT_LEFT;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:mode:TRANSMIT_RIGHT",
    transmitter = UltraSoundSendData::TRANSMIT_RIGHT;
  );
  DEBUG_REQUEST("UltraSoundObstacleLocator:mode:TRANSMIT_BOTH",
    transmitter = UltraSoundSendData::TRANSMIT_BOTH;
  );

  unsigned int mode = transmitter + receiver;

  DEBUG_REQUEST("UltraSoundObstacleLocator:mode:PERIODIC",
    mode += UltraSoundSendData::PERIODIC;
  );

  // Update mode 
  if(mode != getUltraSoundSendData().mode) {
    getUltraSoundSendData().setMode(mode);
  }
}//end execute

void UltraSoundObstacleLocator::drawObstacleModel()
{
  
  DEBUG_REQUEST("UltraSoundObstacleLocator:drawBuffer",
    FIELD_DRAWING_CONTEXT;
    
    PEN("FF0000", 25);
    for(int i = 0; i < bufferLeft.size(); i++) {
      double dist = bufferLeft[i];
      CIRCLE( dist, dist, 10);
    }

    PEN("0000FF", 25);
    for(int i = 0; i < bufferRight.size(); i++) {
      double dist = bufferRight[i];
      CIRCLE( dist, -dist, 10);
    }
  );


  DEBUG_REQUEST("UltraSoundObstacleLocator:drawSensorData",
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
  );
  
  // draw model
  DEBUG_REQUEST("UltraSoundObstacleLocator:drawObstacle",
    PEN("FF0000", 50);
    CIRCLE(
      getObstacleModel().leftDistance,
      getObstacleModel().leftDistance,
      50);

    PEN("0000FF", 50);
    CIRCLE(
      getObstacleModel().rightDistance,
      -getObstacleModel().rightDistance,
      50);

    PEN("000000", 50);
    LINE(getObstacleModel().frontDistance, -200, getObstacleModel().frontDistance, 200);
  );
} //end drawObstacleMode

void UltraSoundObstacleLocator::fillBuffer()
{
  double leftMeasurement = getUltraSoundReceiveData().dataLeft[0];
  double rightMeasurement = getUltraSoundReceiveData().dataRight[0];

  if(rightMeasurement >= UltraSoundReceiveData::MIN_DIST && 
     rightMeasurement < UltraSoundReceiveData::INVALIDE &&
     bufferRight.last() != rightMeasurement)
  {
    bufferRight.add(rightMeasurement * 1000.0);
  }

  if(leftMeasurement >= UltraSoundReceiveData::MIN_DIST && 
     leftMeasurement < UltraSoundReceiveData::INVALIDE &&
     bufferLeft.last() != leftMeasurement)
  {
    bufferLeft.add(leftMeasurement * 1000.0);
  }
}//end fillBuffer

void UltraSoundObstacleLocator::provideToLocalObstacleModel()
{
  
  ObstacleModel& model = getObstacleModel();

  if(bufferLeft.size() == 0) {
    model.leftDistance = invalidDistanceValue;
  } else {
    model.leftDistance = bufferLeft.getMedian();
  }

  if(bufferRight.size() == 0) {
    model.rightDistance = invalidDistanceValue;
  } else {
    model.rightDistance = bufferRight.getMedian();
  }


  if(model.leftDistance <= maxValidDistance || model.rightDistance <= maxValidDistance)
  {
    model.frontDistance = std::min(model.leftDistance, model.rightDistance);
  }

  if(model.frontDistance < parameters.minBlockedDistance)
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

