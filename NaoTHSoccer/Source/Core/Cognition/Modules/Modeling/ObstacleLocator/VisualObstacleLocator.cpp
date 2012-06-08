/**
 * @file VisualObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Kirill Yasinovskiy</a>
 * Implementation of class RadarObstacleLocator
 */

#include "VisualObstacleLocator.h"

VisualObstacleLocator::VisualObstacleLocator()
{
  DEBUG_REQUEST_REGISTER("VisualObstacleLocator:RadarGrid:drawGrid", "draw the modelled Obstacles on the field", false);
  DEBUG_REQUEST_REGISTER("VisualObstacleLocator:RadarGrid:drawUltraSoundData", "draw the ultrasound Obstacles on the field", false);
}

void VisualObstacleLocator::execute()
{
  // store odometry data
  odometryDelta = getOdometryData() - lastRobotOdometry;
  lastRobotOdometry = getOdometryData();
  // set grid's time
  getRadarGrid().setCurrentTime(getFrameInfo().getTime());
   
  //set the radar grid
  for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
  {
    const ScanLineEdgelPercept::EndPoint& point = getScanLineEdgelPercept().endPoints[i];
    if(//point.posInImage.y > 10 && 
       //point.posInImage.y < 200 && 
           (point.color != (int) ColorClasses::green)
      )
    {
      getRadarGrid().addObstaclePoint(point.posOnField);
    }
    else
    {
      getRadarGrid().addNonObstaclePoint(point.posOnField);
    }
  }//end for

  getRadarGrid().ageGrid();


  odometryDelta = lastRobotOdometry - getOdometryData();
  getRadarGrid().updateGridByOdometry(odometryDelta);


  DEBUG_REQUEST("VisualObstacleLocator:RadarGrid:drawGrid",
    getRadarGrid().drawFieldContext();
  );

  //check whether we have an obstacle
  getRadarGrid().checkValid();
}//end execute
