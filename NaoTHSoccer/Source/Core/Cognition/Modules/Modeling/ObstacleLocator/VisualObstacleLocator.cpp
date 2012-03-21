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
  DEBUG_REQUEST_REGISTER("VisualObstacleLocator:drawObstacleBuffer", "draw the modelled Obstacle on the field", false);
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

//   if(timeBuffer.getNumberOfEntries() > 0 && getFrameInfo().getTimeSince(timeBuffer.first()) > 1500)
//   {
//     timeBuffer.removeFirst();
//     buffer.removeFirst();
//   }//end if

  getRadarGrid().ageGrid();

  //set the radar grid
  for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
  {
    const ScanLineEdgelPercept::EndPoint& point = getScanLineEdgelPercept().endPoints[i];
    if(point.posInImage.y > 10 && 
       point.posInImage.y < 200 && 
           (point.color != (int) ColorClasses::green)
      )
    {
      //buffer.add(point.posOnField);
//       buffer.add(point);
//       timeBuffer.add(getFrameInfo().getTime());
      getRadarGrid().addObstaclePoint(point.posOnField);
    }
  }//end for

  odometryDelta = lastRobotOdometry - getOdometryData();
  getRadarGrid().updateGridByOdometry(odometryDelta);

//   Vector2<double> mean;
//   for(int i = 0; i < buffer.getNumberOfEntries(); i++)
//   {
//     mean += buffer.getEntry(i).posOnField;
//   }//end for
// 
//   if (buffer.getNumberOfEntries() > 30)
//   {
//     mean /= buffer.getNumberOfEntries();
//     getLocalObstacleModel().visualObstacleWasSeen = true;
//     getLocalObstacleModel().someObstacleWasSeen = true;
//     getLocalObstacleModel().posVisualObstacle = mean;
//   } else {
//     getLocalObstacleModel().visualObstacleWasSeen = false;
//   }

  DEBUG_REQUEST("VisualObstacleLocator:drawObstacleBuffer",
//     FIELD_DRAWING_CONTEXT;
// 
//     for(int i = 0; i < buffer.getNumberOfEntries(); i++)
//     {
//       PEN(ColorClasses::colorClassToHex(buffer[i].color), 10);
//       CIRCLE(buffer[i].posOnField.x, buffer[i].posOnField.y, 25);
//     }//end for
// 
//     PEN("FF0000", 50);
//     CIRCLE(mean.x, mean.y, 50);
  );

  DEBUG_REQUEST("VisualObstacleLocator:RadarGrid:drawGrid",
    getRadarGrid().drawFieldContext();
  );

  //check whether we have an obstacle
  getRadarGrid().checkValid();
}//end execute
