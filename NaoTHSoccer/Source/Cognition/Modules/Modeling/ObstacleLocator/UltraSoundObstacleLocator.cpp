/**
 * @file ObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
 * @author <a href="mailto:krienelk@informatik.hu-berlin.de">Dominik Krienelke</a>
 * Implementation of class ObstacleLocator
 */

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

  getDebugParameterList().add(&params);
}

UltraSoundObstacleLocator::~UltraSoundObstacleLocator() 
{
  getDebugParameterList().remove(&params);
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
    Color colorLeft(Color::blue);
    Color colorRight(Color::red);
    colorLeft[Color::Alpha] = 0.5;
    colorRight[Color::Alpha] = 0.5;
    // draw raw sensor data
    for(unsigned int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++)
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
	FIELD_DRAWING_CONTEXT;
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

bool UltraSoundObstacleLocator::isNewDataAvaliable() const
{
  // NOTE: this can lead to unexpected/undesired behavior, if the sensor - for whatever reson -
  //       returns the same data for some time!
  for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++) {
    if(getUltraSoundReceiveData().dataLeft[i] != lastValidUltraSoundReceiveData.dataLeft[i] ||
       getUltraSoundReceiveData().dataRight[i] != lastValidUltraSoundReceiveData.dataRight[i])
    {
      return true;
    }
  }
  return false;
}

void UltraSoundObstacleLocator::fillBuffer()
{
  if(!isNewDataAvaliable()) {
    return;
  }
  lastValidUltraSoundReceiveData = getUltraSoundReceiveData();
  
  // http://doc.aldebaran.com/2-1/family/nao_dcm/actuator_sensor_names.html#actuator-sensor-list-nao
  // http://doc.aldebaran.com/2-8/family/nao_technical/lola/actuator_sensor_names.html#sonars
  // http://doc.aldebaran.com/1-14/family/robots/sonar_robot.html

  // convert to mm
  double leftMeasurement = getUltraSoundReceiveData().dataLeft[0] * 1000.0;
  double rightMeasurement = getUltraSoundReceiveData().dataRight[0] * 1000.0;

  // for v5 and v6 the 0.0 value should be treated as invalid
  // NOTE: Stella 2021/04/14 does not explain the full behavior yet
  if(leftMeasurement == 0.0){
    leftMeasurement = invalidDistanceValue;
  }
  if (rightMeasurement == 0.0) {
    rightMeasurement = invalidDistanceValue;
  }

  bufferRight.add(rightMeasurement);
  bufferLeft.add(leftMeasurement);
}

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

  if(model.frontDistance < params.minBlockedDistance)
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

