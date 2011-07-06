/**
 * @file VisualObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author Kirill Yasinovskiy
 * Implementation of class RadarObstacleLocator
 */

#include "VisualObstacleLocator.h"

VisualObstacleLocator::VisualObstacleLocator()
{
  angle_offset = 10.0 + 180.0;
  obstacleBuffer.reserve(18);
  onceExecuted = false;
  odometryDelta.translation.x = 0;
  odometryDelta.translation.y = 0;
  odometryDelta.rotation = 0;

  //ultraSound params
  minValidDistance = 0.15;
  maxValidDistance = 0.7;
  usOpeningAngle = Math::fromDegrees(30.0);

  DEBUG_REQUEST_REGISTER("VisualObstacleLocator:drawObstacleBuffer", "draw the modelled Obstacle on the field", false);
  DEBUG_REQUEST_REGISTER("VisualObstacleLocator:RadarGrid:drawGrid", "draw the modelled Obstacles on the field", false);
  DEBUG_REQUEST_REGISTER("VisualObstacleLocator:RadarGrid:drawUltraSoundData", "draw the ultrasound Obstacles on the field", false);
}

void VisualObstacleLocator::execute()
{
  if (!onceExecuted)
  {
    //set initial time
    initialTime = getFrameInfo().getTime();
    onceExecuted = true;
  }

  //get current time
  currentTime = getFrameInfo().getTime();

  //get current odometry delta
  //collect the odometry data
  odometryDelta += (lastRobotOdometry - getOdometryData());
  lastRobotOdometry = getOdometryData();

  //just age the grid
  if ((currentTime - initialTime) >= 1000)
  {
    getRadarGrid().ageGrid();
  }
  

  if(timeBuffer.getNumberOfEntries() > 0 && getFrameInfo().getTimeSince(timeBuffer.first()) > 1500)
  {
    timeBuffer.removeFirst();
    buffer.removeFirst();
  }//end if

  //set the radar grid
  for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
  {
    const ScanLineEdgelPercept::EndPoint& point = getScanLineEdgelPercept().endPoints[i];
    if(point.posInImage.y > 10 && 
       point.posInImage.y < 200 && 
        (point.color == (int) ColorClasses::white ||
         point.color == (int) ColorClasses::skyblue ||
         point.color == (int) ColorClasses::yellow)
      )
    {
      buffer.add(point.posOnField);
      timeBuffer.add(getFrameInfo().getTime());
      getRadarGrid().set(point.posOnField);
    }
  }//end for

  //set the grid with ultrasound data
  updateByUltraSoundData();

  //update the grid with odometry data
  if ((currentTime - initialTime) >= 1000)
  {
    getRadarGrid().updateGrid(odometryDelta);
    initialTime = currentTime;
    odometryDelta.translation.x = 0;
    odometryDelta.translation.y = 0;
    odometryDelta.rotation = 0;
  }

  Vector2<double> mean;
  for(int i = 0; i < buffer.getNumberOfEntries(); i++)
  {
    mean += buffer.getEntry(i);
  }//end for

  if (buffer.getNumberOfEntries() > 30)
  {
    mean /= buffer.getNumberOfEntries();

    getLocalObstacleModel().visualObstacleWasSeen = true;
    getLocalObstacleModel().posVisualObstacle = mean;
  } else {
    getLocalObstacleModel().visualObstacleWasSeen = false;
  }

  DEBUG_REQUEST("VisualObstacleLocator:drawObstacleBuffer",
    FIELD_DRAWING_CONTEXT;
    PEN("999999", 10);

    for(int i = 0; i < buffer.getNumberOfEntries(); i++)
    {
      CIRCLE(buffer[i].x, buffer[i].y, 25);
    }//end for

    PEN("FF0000", 50);
    CIRCLE(mean.x, mean.y, 50);
  );

  DEBUG_REQUEST("VisualObstacleLocator:RadarGrid:drawGrid",
    getRadarGrid().drawFieldContext();
  );


  /*
  // 
  for(int i = 0; i < theScanLineEdgelPercept.endPoints.size(); i++)
  {
    add(theScanLineEdgelPercept.endPoints[i]);
  }


  DEBUG_REQUEST("RadarObstacleLocator:drawObstacleBuffer",
    double alpha_step = Math::pi2/18;
    Vector2<double> direction(-1,0);
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 50);

    for(int i = 0; i < 18; i++)
    {
      Vector2<double> mean = direction * obstacleBuffer[i].getMean().abs();

      PEN(ColorClasses::colorClassToHex(obstacleBuffer[i].getColor()), 50);
      CIRCLE(mean.x, mean.y, 25);
      direction.rotate(alpha_step);
    }//end for
  );

  */
}//end execute

void VisualObstacleLocator::updateByUltraSoundData()
{
  if(getUltraSoundReceiveData().rawdata <= maxValidDistance || getUltraSoundReceiveData().rawdata >= minValidDistance)
    return; 

  double distance = getUltraSoundReceiveData().rawdata;
  Vector2d ultraSoundPoint1(distance, 0);
  getRadarGrid().set(ultraSoundPoint1);

  Vector2d ultraSoundPoint2(ultraSoundPoint1.x + distance*cos(usOpeningAngle), ultraSoundPoint1.y + distance*tan(usOpeningAngle));
  getRadarGrid().set(ultraSoundPoint2);

  Vector2d ultraSoundPoint3(ultraSoundPoint1.x + distance*cos(usOpeningAngle), ultraSoundPoint1.y - distance*tan(usOpeningAngle));
  getRadarGrid().set(ultraSoundPoint3);

  DEBUG_REQUEST("VisualObstacleLocator:RadarGrid:drawUltraSoundData",
    FIELD_DRAWING_CONTEXT;
    PEN("fbfb00", 2);
    LINE(0,0, ultraSoundPoint1.x, ultraSoundPoint1.y);
    LINE(0,0, ultraSoundPoint2.x, ultraSoundPoint3.y);
    LINE(0,0, ultraSoundPoint3.x, ultraSoundPoint3.y);
  );
}
