/**
 * @file RadarObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of class RadarObstacleLocator
 */

#include "RadarObstacleLocator.h"

RadarObstacleLocator::RadarObstacleLocator()
{
  angle_offset = 10.0 + 180.0;
  obstacleBuffer.reserve(18);
  onceExecuted = false;

  DEBUG_REQUEST_REGISTER("RadarObstacleLocator:drawObstacleBuffer", "draw the modelled Obstacle on the field", false);
  DEBUG_REQUEST_REGISTER("RadarObstacleLocator:RadarGrid:drawGrid", "draw the modelled Obstacles on the field", false);
}

void RadarObstacleLocator::execute()
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
  odometryDelta += (lastRobotOdometry - getOdometryData());
  
  lastRobotOdometry = getOdometryData();

  if ((currentTime - initialTime) >= 1000)
  {
    getRadarGrid().ageGrid();
    getRadarGrid().updateGrid(odometryDelta);
    initialTime = currentTime;
    odometryDelta.translation.x = 0;
    odometryDelta.translation.y = 0;
    odometryDelta.rotation = 0;
  }
  

  if(timeBuffer.getNumberOfEntries() > 0 && getFrameInfo().getTimeSince(timeBuffer.first()) > 1500)
  {
    timeBuffer.removeFirst();
    buffer.removeFirst();
  }//end if


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

  DEBUG_REQUEST("RadarObstacleLocator:drawObstacleBuffer",
    FIELD_DRAWING_CONTEXT;
    PEN("999999", 10);

    for(int i = 0; i < buffer.getNumberOfEntries(); i++)
    {
      CIRCLE(buffer[i].x, buffer[i].y, 25);
    }//end for

    PEN("FF0000", 50);
    CIRCLE(mean.x, mean.y, 50);
  );

  DEBUG_REQUEST("RadarObstacleLocator:RadarGrid:drawGrid",
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
