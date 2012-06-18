/* 
 * File:   Grid.cpp
 * Author: Kirill Yasinovskiy
 *
 * Created on 27. Juni 2011
 */

#include "RadarGrid.h"

//debugDrawings
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Math/Line.h"

//debug
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"

//class constructor
RadarGrid::RadarGrid()
  :
  obstacleWasSeen(false),
  useBuffer(true),
  center(0,0),
  density(Math::pi2/numOfSectors),
  nearUpdate(0.5),
  farUpdate(0.2)
{
  MODIFY("RadarGrid:nearUpdate", nearUpdate);
  MODIFY("RadarGrid:farUpdate", farUpdate);
}//end RadarGrid


double RadarGrid::getDistanceForAngle(double angle) const
{
  // find the cell in the map
  int position = this->getSectorByAngle(Math::normalize(angle));
  cellsMap::const_iterator it = cells.find(position);
  // check whether we have found the cell
  if(it != cells.end()) { return it->second.mean.abs(); }
  // we haven't found the cell or the map is empty
  // so just return maxDistance
  else { return maxDistance; }
}//end getDistanceForAngle

double RadarGrid::getDistanceInCorridor(double angle, double width) const
{
  double minDistance = maxDistance;

  int minSector = this->getSectorByAngle(angle-Math::pi_2);
  int maxSector = this->getSectorByAngle(angle+Math::pi_2);

  // create a central line for corridor
  Vector2<double> base(0.0, 0.0);
  Vector2<double> direction(cos(angle), sin(angle));
  Math::Line centerLine(base, direction);

  for(int i = minSector; i < maxSector; i++)
  {
    int currentSector = i%numOfSectors;
    Vector2<double> point; 
    double angle = this->getAngleBySector(currentSector);
    double distance = getDistanceForAngle(angle);
    point.x = cos(angle) * distance;
    point.y = sin(angle) * distance;
    if(
      distance < minDistance && 
      distance > 10 &&
      abs(centerLine.minDistance(point)) < width / 2.0
      )
      minDistance = distance;
  }
  return minDistance;
}// end get distance in corridor


// set the grid with value
void RadarGrid::addObstaclePoint(const Vector2d& value)
{
  // we don't interested in distant obstacles...
  // but need to filter up values, that are to close to robot
  if (value.abs() <= maxDistance && value.abs() >= 200)
  {
    //get the sector index
    int position = this->getSectorByAngle(Math::normalize(value.angle()));
    setMean(position, value);
  }
}//end set()


void RadarGrid::addNonObstaclePoint(const Vector2d& value)
{
  // get the sector index
  int position = getSectorByAngle(value.angle());
  // find the sector in the map
  cellsMap::iterator it = cells.find(position);
  // the value with key wasn't found:
  // do nothing
  if (it == cells.end())
  {
    return;
  } 
  // we found some value, 
  // must update it
  else
  {
    // we have found the pair with this key value
    // need to update value?
    if (value.abs() > it->second.mean.abs())
    {
      // yes, we do
      // do we use buffer?
      if (useBuffer)
      {
        // we have at least 2 values in buffer
        if(it->second.buffer.getNumberOfEntries() > 1)
        {
          it->second.buffer.removeFirst();
          it->second.timeStamp = currentTime;
          getSumAndAverage(it->second.buffer, it->second.sum, it->second.mean);
        }//end if
        //the buffer have no values -> erase the cell from map
        else
        {
          cells.erase(it);
        }// end els
      }
      // we use no buffer
      // so, just remove this cell
      else 
      { 
        cells.erase(it);
      } // end else
    }
  }
}


void RadarGrid::setMean(int& position, const Vector2d& value)
{
  // find the sector in the map
  cellsMap::iterator it = cells.find(position);
  //the value with key wasn't found:
  //we have no pair with this key value
  if (it == cells.end())
  {
    std::pair<int, Cell> temp;
    temp.first = position;
    // do we use buffer?
    if (useBuffer)
    {
      temp.second.buffer.add(value);
      getSumAndAverage(temp.second.buffer, temp.second.sum, temp.second.mean);
    }
    // no, we don't
    else { temp.second.mean = value; }
    temp.second.timeStamp = currentTime;
    cells.insert(temp);
  }// end if
  //we have found the pair with this key value
  //->the buffer have at least one value
  else
  {
    // do we use buffer
    if (useBuffer)
    {
      it->second.buffer.add(value);
      getSumAndAverage(it->second.buffer, it->second.sum, it->second.mean);
    } 
    // no, we don't
    else { it->second.mean = value;}
    it->second.timeStamp = currentTime;
  }// end else
}// end setMean


//get index by angle
int RadarGrid::getSectorByAngle(const double angle) const
{
  return (int)((Math::normalize(angle) + Math::pi) / Math::pi2 * numOfSectors);
}//end getIndexByAngle


// get angle of a given sector
double RadarGrid::getAngleBySector( const int sector ) const
{
  return -Math::pi + ((double)sector + 0.5) * Math::pi2 / numOfSectors;
}


void RadarGrid::getAllObstacles(std::vector<Vector2d>& obstaclesVector) const
{
  obstaclesVector.clear();
  for(cellsMap::const_iterator it = cells.begin(); it != cells.end(); ++it)
  {
    obstaclesVector.push_back(it->second.mean);
  }
}



void RadarGrid::ageGrid()
{
  for(cellsMap::iterator it_end=cells.end(), it_next=cells.begin(),
    it = (it_next==it_end)?it_next:it_next++;
    it != it_next;
    it = (it_next==it_end)?it_next:it_next++)
  {
    // do we use buffer?
    // if yes, check whether we have old not-updated values in buffer and delete them
    if (useBuffer && (currentTime - it->second.timeStamp >= timeToForgetWithBuffer))
    {
      // we have at least 2 values in buffer
      if(it->second.buffer.getNumberOfEntries() > 1)
      {
        it->second.buffer.removeFirst();
        it->second.timeStamp = currentTime;
        getSumAndAverage(it->second.buffer, it->second.sum, it->second.mean);
      }//end if
      //the buffer have no values -> erase the cell from map
      else
      {
        cells.erase(it);
      }
    }// end useBuffer
    // no, we don't, check whether the value is too old
    if(!useBuffer && (currentTime - it->second.timeStamp >= timeToForget))
    {
      cells.erase(it);
    }
  }
}//end ageGrid

// update the model:
// apply the odometry data, erase old values
void RadarGrid::updateGridByOdometry(Pose2D& odometryDelta)
{
  //so, we have no old values in the map,
  //let's update the map with odometry data
  //iterate through all map values
  cellsMap tempCells;
  for(cellsMap::iterator it_end=cells.end(), it_next=cells.begin(),
    it = (it_next==it_end)?it_next:it_next++;
    it != it_next;
  it = (it_next==it_end)?it_next:it_next++)
  {
    // if the value wasn't updated in last cognition cycle
    // we must update it by odometry
    if (it->second.timeStamp < currentTime)
    {
      Cell newCell;
      // do we use buffer?
      if (useBuffer)
      {
        // update buffer with odometry
        newCell.buffer = it->second.buffer;
        for(int i = 0; i < newCell.buffer.getNumberOfEntries(); i++)
        {
          newCell.buffer[i] = applyOdometry(it->second.buffer[i], odometryDelta);
        }
        // calculate new mean value
        getSumAndAverage(newCell.buffer, newCell.sum, newCell.mean);
      } 
      // no, we don't
      // just apply odometry to mean value of the cell
      else
      {
        newCell.mean = applyOdometry(it->second.mean, odometryDelta);
      }
      newCell.timeStamp = it->second.timeStamp;
      std::pair<int, Cell> newPair(getSectorByAngle(Math::normalize(newCell.mean.angle())), newCell);
      // erase the old cell...
      cells.erase(it++);
      // ... and insert the new one in temp map
      tempCells.insert(newPair);
    }// end if
  }//end for
  // now we must insert tempCells map into cells map
  cells.insert(tempCells.begin(), tempCells.end());
}//end updateGrid

Vector2d RadarGrid::applyOdometry(Vector2d& value, Pose2D& odometryDelta)
{
  // first of all, create a rotation's matrix
  Vector2<double> c1(cos(odometryDelta.rotation),-sin(odometryDelta.rotation));
  Vector2<double> c2(sin(odometryDelta.rotation),cos(odometryDelta.rotation));
  Matrix2x2<double> R(c1, c2);
  // apply rotation matrix
  Vector2d upliedOdometry = R*value;
  // apply translation
  upliedOdometry -= odometryDelta.translation;
  return upliedOdometry;
}// end applyOdometry

void RadarGrid::getSumAndAverage( RingBuffer<Vector2d, bufferCapacity>& buffer, Vector2d& sum, Vector2d& average ) const
{
  sum = Vector2d(0.0, 0.0);
  for (int i = 0; i < buffer.getNumberOfEntries(); i++)
  {
    sum += buffer.getEntry(i);
  }
  average = sum / buffer.getNumberOfEntries();
}// end getSumAndAverage


void RadarGrid::checkValid()
{
  if (!this->cells.empty())
  {
    obstacleWasSeen = true;
  }
  else
  {
    obstacleWasSeen = false;
  }
}// check, whether we have some obstacles


void RadarGrid::resetGrid()
{
  this->cells.clear();
}


//draw radarGrid within FieldContext
void RadarGrid::drawFieldContext()
{
  FIELD_DRAWING_CONTEXT;
  //draw the robot
  PEN("0000ED", 20);
  ROBOT(this->center.x, this->center.y, 0);
  
  //draw the circle
  PEN("00CF00", 2);
  CIRCLE(this->center.x, this->center.y, maxDistance);
  
  //draw slices
  Vector2<double> temp(0,0);
  for (int i = numOfSectors; i > 0; --i)
  {
    PEN("00CF00", 2);
    temp.x = maxDistance*cos(density/2 + i*density);
    temp.y = maxDistance*sin(density/2 + i*density);
    LINE(this->center.x, this->center.y, temp.x, temp.y);
    PEN(ColorClasses::colorClassToHex(ColorClasses::black), 1);
    TEXT_DRAWING(temp.x, temp.y, getSectorByAngle(Math::normalize(i*density)));
  }

  //draw grid values
  cellsMap::iterator CIT = cells.begin();
  for (; CIT != cells.end(); CIT++)
  {
    // do we use buffer?
    if (useBuffer)
    {
      PEN("434343", 3);
      for (int i = 0; i < CIT->second.buffer.getNumberOfEntries(); i++)
      {
        CIRCLE(CIT->second.buffer[i].x, CIT->second.buffer[i].y, 10);
      }
    }// end if
    // otherwise just draw the mean value of the cell
    // first of all, draw the filled sectors
    Vector2d mean = CIT->second.mean;
    ASSERT(mean.x != 0 && mean.y != 0);
    double distance = mean.abs();
    double angle = mean.angle();
    int index = getSectorByAngle(angle);
    Vector2d linksEnd(distance*cos(Math::normalize(index*density+Math::pi)), distance*sin(Math::normalize(index*density+Math::pi)));
    Vector2d rechtsEnd(distance*cos(Math::normalize((index+1)*density+Math::pi)), distance*sin(Math::normalize((index+1)*density+Math::pi)));
    // if we have currently set the cell - use the red color
    if (CIT->second.timeStamp == currentTime) { PEN("FF0000", 6); }
    // otherwise - use the blue color
    else { PEN("#0000FF",6); }
    LINE(center.x, center.y, linksEnd.x, linksEnd.y);
    LINE(center.x, center.y, rechtsEnd.x, rechtsEnd.y);
    LINE(linksEnd.x, linksEnd.y, rechtsEnd.x, rechtsEnd.y);
    
    // draw mean value as a blob
    PEN("FF00FF", 20);
    CIRCLE(CIT->second.mean.x, CIT->second.mean.y, 20);
  }
}//end drawFieldContext


//draw radarGrid within ImageContext
void RadarGrid::drawImageContext()
{
  IMAGE_DRAWING_CONTEXT;
}//end drawImageContext


