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
  getLocalObstacleModel().someObstacleWasSeen = false;
  lastRobotOdometry = getOdometryData();
  grid.init(ageThreshold, getFrameInfo());

  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawObstacleBuffer", "draw the modelled Obstacle on the field", false);
  DEBUG_REQUEST_REGISTER("UltraSoundObstacleLocator:drawObstacleBufferNumbers", "draw the values of modelled Obstacle on the field", false);

  maxValidDistance = 0.80f;
  minValidDistance = 0.15f;
  usOpeningAngle = Math::fromDegrees(30.0f);
  for(int i=0; i < this->CELLS_X; i++)
    for(int j=0; j < this->CELLS_Y; j++){
      grid.setState(i,j,Cell::FREE);
      grid.setLastUpdate(i,j,0);
    }

//  DEBUG_REQUEST_REGISTER("ObstacleLocator:drawPostBuffer", "draw the modelled Obstacle on the field", false);
//  DEBUG_REQUEST_REGISTER("ObstacleLocator:drawPost", "draw the modelled Obstacle on the field", false);
}

void UltraSoundObstacleLocator::execute()
{
  getLocalObstacleModel().obstacles.clear();
  getLocalObstacleModel().someObstacleWasSeen = false;

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
  updateGrid();

  // Compute mean of seen obstacles and use that data as estimated USObstacle  
  // in local obstacle model
  provideToLocalObstacleModel();

  //Draw ObstacleModel
  drawObstacleModel();
  drawObstacleModelNumbers();

}//end execute

void UltraSoundObstacleLocator::updateGrid()
{
  // ignore rawdata if not viable
  if(getUltraSoundReceiveData().rawdata >= maxValidDistance || getUltraSoundReceiveData().rawdata <= minValidDistance)
    return; 

  // update Grid based on rawdata? of ultrasound and the estimated projection of the ultrasound-detector-cone to the grid
  // for the "front" detector
  double x0 = getUltraSoundReceiveData().rawdata * 1000.0;
 
  int iPercept = (int)(x0/this->CELL_SIZE);

  for(int i=iPercept; i >= 0; i--){
    int y = (int)(i*tan(usOpeningAngle/2));
    for(int j=-y; j <= y; j++){
      int currentX = (int)((this->CELLS_X)/2+i); // add centre and round to nearest whole number
      int currentY = (int)((this->CELLS_Y)/2+j); // add centre and round to nearest whole number
      if(i==iPercept)
        grid.setUpdate(currentX,currentY, getFrameInfo().getTime(), 1);
      else
        grid.setUpdate(currentX,currentY, getFrameInfo().getTime(), -1);
    }
  }
  //grid.setUpdate(10,10,getFrameInfo().time, 1); // Debugtest
  //DOUT ( "BEHIND UPDATE(" << 10 << "," << 10 << ")=" <<  grid.getGrid(10,10).state << "\n\n");

} //end updateGrid

/*void ObstacleLocator::ageGrid()
{
  // age cells (remove values) that are older than x (e.g. 3 seconds)
  for(int i=0; i < this->CELLS_X; i++)
    for(int j=0; j < this->CELLS_Y; j++){
      if((unsigned int)(getFrameInfo().getTimeSince(grid.getGrid(i,j).lastUpdate) > ageThreshold))
        grid.setState(i,j,Cell::FREE);
    }
} //end ageGrid*/

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
  // add obstacle if rawdata implies it
  if(getUltraSoundReceiveData().rawdata < maxValidDistance && getUltraSoundReceiveData().rawdata > minValidDistance)
  {
    ObstacleModel::Obstacle obstacle;
    obstacle.frameInfoObstacleWasSeen = getFrameInfo();
    obstacle.position.x = getUltraSoundReceiveData().rawdata * 1000.0;
    obstacleBuffer.add(obstacle);
  }// end if

  // add obstacles based on left/right sensordate
  for(unsigned int i = 0; i < UltraSoundData::numOfUSEcho; i++)
  {
    break; // skip this for-loop for now
    if(getUltraSoundReceiveData().dataLeft[i] < maxValidDistance && getUltraSoundReceiveData().dataLeft[i] > minValidDistance)
    {
      ObstacleModel::Obstacle obstacleLeft;
      obstacleLeft.frameInfoObstacleWasSeen = getFrameInfo();
      obstacleLeft.position.x = getUltraSoundReceiveData().dataLeft[i] * 1000;
      obstacleLeft.position.y = -300;
      obstacleBuffer.add(obstacleLeft);
    }//end if

    if(getUltraSoundReceiveData().dataRight[i] < maxValidDistance && getUltraSoundReceiveData().dataRight[i] > minValidDistance)
    {
      ObstacleModel::Obstacle obstacleRight;
      obstacleRight.frameInfoObstacleWasSeen = getFrameInfo();
      obstacleRight.position.x = getUltraSoundReceiveData().dataRight[i] * 1000;
      obstacleRight.position.y = -300;
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
        obstacleBuffer[i].position.x, 
        obstacleBuffer[i].position.y,
        50);
    }//end for
  
    if(obstacleBuffer.getNumberOfEntries() > 10)
    {
      PEN("522100", 600);
      TEXT_DRAWING(getMean().x,getMean().y-300,getMean().abs());
      PEN("522100", 50);
      CIRCLE(getMean().x,
             getMean().y,
             50);

      /*PEN("ECDB50", 600); TEXT_DRAWING(getMinimum().x,getMinimum().y-300,getMinimum().abs()); 
        PEN("ECDB50", 50); CIRCLE(getMinimum().x, getMinimum().y, 50); **/

      PEN("FF0000", 120);
      TEXT_DRAWING(getLocalObstacleModel().ultraSoundObstacleEstimation.x,
                   getLocalObstacleModel().ultraSoundObstacleEstimation.y+300,
                   getLocalObstacleModel().ultraSoundObstacleEstimation.abs());
      PEN("FF0000", 50);
      CIRCLE(getLocalObstacleModel().ultraSoundObstacleEstimation.x,
             getLocalObstacleModel().ultraSoundObstacleEstimation.y,
             50);
    }

    // draw grid
    std::cout << "\t drawing grid?:\n";
    DebugDrawings::Color color2(ColorClasses::black);
    color2[DebugDrawings::Color::alpha] = 1.0;
    for(int x = 0; x < CELLS_X; x++)
      for(int y = 0; y < CELLS_Y; y++)
    {

      color2[DebugDrawings::Color::alpha] = 1.00 - (Cell::MAX_VALUE - grid.getGrid(x,y).state) / Cell::MAX_VALUE;
      if(color2[DebugDrawings::Color::alpha] == 0)
        color2[DebugDrawings::Color::alpha] = 0.14;

      PEN(color2, 20);
      //DOUT ( "(" << x << "," << y << ")=" <<  grid.getGrid(x,y).state << "  ");
      CIRCLE((x-CELLS_X/2)*CELL_SIZE, 
        -(y-CELLS_Y/2)*CELL_SIZE,
        10);
    }//end for

  );
} //end drawObstacleMode

void UltraSoundObstacleLocator::drawObstacleModelNumbers()
{
  // Inits
  DEBUG_REQUEST("UltraSoundObstacleLocator:drawObstacleBufferNumbers",
    FIELD_DRAWING_CONTEXT;
    
    // draw grid
    std::cout << "\t drawing grid numbers:\n";
    DebugDrawings::Color color2(ColorClasses::red);
    //color2[DebugDrawings::Color::alpha] = 1.0;
    for(int x = 0; x < CELLS_X; x++)
      for(int y = 0; y < CELLS_Y; y++)
    {
      PEN(color2, 20);
      TEXT_DRAWING((x-CELLS_X/2)*CELL_SIZE, 
        -(y-CELLS_Y/2)*CELL_SIZE,
        grid.getGrid(x,y).state);
    }//end for
    //Pose2D odometryDelta = lastRobotOdometry - theOdometryData;
    //TEXT_DRAWING(2000, 2000, odometryDelta);

  );
} //end drawObstacleModelNumbers

void UltraSoundObstacleLocator::provideToLocalObstacleModel()
{
  if(obstacleBuffer.getNumberOfEntries() > 10)
  {
    // getLocalObstacleModel().ultraSoundObstacleEstimation = getMean();
    getLocalObstacleModel().ultraSoundObstacleEstimation = getDampedMinimum();
    getLocalObstacleModel().someObstacleWasSeen = true;
    getLocalObstacleModel().frameWhenObstacleWasSeen = getFrameInfo();
  }//end if
  else
  {
    getLocalObstacleModel().ultraSoundObstacleEstimation.x = 2550;
    getLocalObstacleModel().ultraSoundObstacleEstimation.y = 0;
    getLocalObstacleModel().someObstacleWasSeen = false;
  }


} //end provideToLocalObstacleModel

Vector2<double> UltraSoundObstacleLocator::getMean()
{
  Vector2<double> mean;
  for(int i = 0; i < obstacleBuffer.getNumberOfEntries(); i++)
  {
    mean += obstacleBuffer[i].position;
  }//end for

  return mean / ((double)obstacleBuffer.getNumberOfEntries());
}//end getMean

Vector2<double> UltraSoundObstacleLocator::getMinimum()
{
  Vector2<double> min;
  min = obstacleBuffer[0].position;
  for(int i = 1; i < obstacleBuffer.getNumberOfEntries(); i++)
  {
    if(min.abs() > obstacleBuffer[i].position.abs())
      min = obstacleBuffer[i].position;
  }//end for

  return min;
}//end getMin

// considers both minimum and mean to approximate the actual obstacle
// the minimum is weighted stronger
Vector2<double> UltraSoundObstacleLocator::getDampedMinimum()
{
  double weight_min = 0.7;
  double weight_mean = 0.3; // weights should add up to 1!
  Vector2<double> min;
  Vector2<double> mean;
  min = obstacleBuffer[0].position;
  for(int i = 0; i < obstacleBuffer.getNumberOfEntries(); i++)
  {
    mean += obstacleBuffer[i].position;
    if(min.abs() > obstacleBuffer[i].position.abs())
      min = obstacleBuffer[i].position;
  }//end for
  mean = mean / ((double)obstacleBuffer.getNumberOfEntries());

  return (min*weight_min + mean*weight_mean);
}//end getDampedMin

void UltraSoundObstacleLocator::updateByOdometry(const Pose2D& odometryDelta)
{
  // updateByOdometry for buffer
  for(int i = 0; i < obstacleBuffer.getNumberOfEntries(); i++)
  {
    obstacleBuffer[i].position = odometryDelta * obstacleBuffer[i].position;
  }//end for

  Grid newgrid; // the grid itself, dimensions = 40 * 40, (60mm)
  for(int i=0; i < this->CELLS_X; i++)
    for(int j=0; j < this->CELLS_Y; j++){
      newgrid.setState(i,j,Cell::FREE);
      newgrid.setLastUpdate(i,j,0);
    }

  // updateByOdometry for grid
  for(int x = 0; x < CELLS_X; x++)
    for(int y = 0; y < CELLS_Y; y++)
      if(grid.getGrid(x,y).state > Cell::FREE)
      {
        // translation i = realvalue(gridvalue) + odometry
        Vector2<double> i;
        i.x = (x-CELLS_X/2)*CELL_SIZE; 
        i.y = -(y-CELLS_Y/2)*CELL_SIZE;
        //std::cout << "i before: (" << i.x << "," << i.y << ")\n";
        i = odometryDelta * i;
        //std::cout << "i after: (" << i.x << "," << i.y << ")\n\n";
        
        // newgridvalue(i)
        newgrid.setGrid((int)(i.x/CELL_SIZE + CELLS_X/2 + 0.5) , (int)(-i.y/CELL_SIZE + CELLS_Y/2 + 0.5), grid.getGrid(x,y));
      }
  grid = newgrid;
  // TODO OPTIMIZE?
}//end updateByOdometry
