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

//debug
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"

//class constructor
RadarGrid::RadarGrid()
  :
obstacleWasSeen(false),
angleResolution(20),
center(0,0),
nearUpdate(0.5),
farUpdate(0.2)
{
  MODIFY("RadarGrid:nearUpdate", nearUpdate);
  MODIFY("RadarGrid:farUpdate", farUpdate);
}//end RadarGrid


Vector2d RadarGrid::get(double angle) const

{
  Vector2d temp;
  int position = this->getIndexByAngle(angle);

  //check whether map is empty
  if(!cells.empty())
  {
    newMap::const_iterator it = cells.find(position);
    //do we have a value?
    if (it != cells.end())
    {
      temp = it->second.value;
    }
    else
    {
      temp.x = 0;
      temp.y = 0;
    }
  }
  //container is empty
  else
  {
    temp.x = 0;
    temp.y = 0;
  }
  return temp;
}//end get()


//set the grid with value
void RadarGrid::set(Vector2<double> value)
{
  //translate to polar coordinate system
  //estimate the angle
  double newAngle = value.angle();
  //estimate distance
  double newDist = value.abs();

  //we don't interested in distant obstacles...
  if (newDist >= 1500)
  {
    return;
  }

  int position = this->getIndexByAngle(newAngle);

  //store the old values
  newMap::iterator it = cells.find(position);
  //the value with key wasn't found:
  //we have no pair with tis key value
  if (it == cells.end())
  {
    std::pair<unsigned int, Cell> temp;
    temp.first = this->getIndexByAngle(newAngle);
    temp.second.value.x = value.abs();
    temp.second.value.y = value.angle();
    cells.insert(temp);
  }
  //we've found the pair with this key value
  else
  {
    double oldDist = it->second.value.x;
    double oldAngle = it->second.value.y;
    //if the new point lies closer that the old one
    if (oldDist >= newDist)
    {
      it->second.value.x -= (oldDist - newDist)*nearUpdate;
    }
    //else
    else
    {
      it->second.value.x += (newDist - oldDist)*farUpdate;
    }
    if (abs(oldAngle) >= abs(newAngle))
    {
      it->second.value.y -= (oldAngle - newAngle)*nearUpdate;
    }
    //else
    else
    {
      it->second.value.y += (newAngle - oldAngle)*farUpdate;
    }//end else
    it->second.age = 3;
  }//end else
}//end set()


//get index by angle
unsigned int RadarGrid::getIndexByAngle(const double& angle) const
{
  double degreeAngle = Math::toDegrees(angle);
  double temp = 0;
  if (degreeAngle < 0)
  {
    temp = abs(degreeAngle);
  }
  else
  {
    temp = Math::toDegrees(Math::pi2) - degreeAngle;
  }
  temp += 10;
  if (temp > 360)
  {
    temp -= 360;
  }
  int result = (int)(temp/angleResolution);
  ASSERT((result >= 0) && (result <= 17));
  return result;
}//end getIndexByAngle

void RadarGrid::ageGrid()
{
  newMap::iterator CIT = cells.begin();
  for (; CIT != cells.end(); ++CIT)
  {
    if(CIT->second.age > 0)
    {
      CIT->second.age--;
    }//end if
  }//end for
}//end ageGrid

//update the model:
//apply the odometry data, erase old values
void RadarGrid::updateGrid(Pose2D& odometryDelta)
{
  //erase old values
  newMap::iterator CITB = cells.begin();
  newMap::iterator CITE = cells.end();
  for (; CITB != CITE;)
  {
    //erase all values that are older then 1
    if (CITB->second.age <= 0)
    {
      cells.erase(CITB++);
    }//end if
    else
    {
      CITB++;
    }
  }//end for
  //so, we have no old values in the map,
  //let's update the map with odometry data

  //if (!cells.empty())
  //{
    CITB = cells.begin();
    CITE = cells.end();
    //iterate through all map values
    newMap newCells;
    for (; CITB != CITE;)
    {
      //if the value wasn't updated in last cognition
      //cycle
      if (CITB->second.age <= 2)
      {
        Cell newCell;
        newCell.value = CITB->second.value;
        //update with odometry
        newCell.value = applyOdometry(newCell.value, odometryDelta);
        newCell.age = CITB->second.age;
        std::pair<unsigned int, Cell> newPair(getIndexByAngle(newCell.value.y), newCell);
        cells.erase(CITB++);
        newCells.insert(std::pair<unsigned int, Cell>(getIndexByAngle(newCell.value.y), newCell));
      }
      else
      {
        CITB++;
      }
    }//end for
    //insert new values in old map
    cells.insert(newCells.begin(), newCells.end());
  //}
}//end updateGrid

Vector2d RadarGrid::applyOdometry(Vector2d someValue, Pose2D& odometryDelta)
{
  Vector2d newCell;
  //take the value and translate
  //to Cartesian coordinates
  newCell.x = someValue.x*cos(someValue.y);
  newCell.y = someValue.x*sin(someValue.y);
  //apply odometry translation
  newCell = odometryDelta * newCell;
  //translate to polar coordinates and
  newCell.x = newCell.abs();
  newCell.y = newCell.angle();
  return newCell;
}

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
  CIRCLE(this->center.x, this->center.y, 1500);
  //draw slices
  Vector2<double> temp(0,0);
  for (int i = 0; i < 18; ++i)
  {
    PEN("00CF00", 2);
    temp.x = 1500*cos(Math::fromDegrees(10+i*20));
    temp.y = 1500*sin(Math::fromDegrees(10+i*20));
    LINE(this->center.x, this->center.y, temp.x, temp.y);
  }
  //draw grid values
  Vector2<double> temp2(0,0);
  newMap::iterator CIT = cells.begin();
  for (; CIT != cells.end(); CIT++)
  {
    PEN("FF0000", 6);
    Vector2d rechtsEnd, linksEnd, centerEnd;
    centerEnd.x = CIT->second.value.x*cos(CIT->second.value.y);
    centerEnd.y = CIT->second.value.x*sin(CIT->second.value.y);

    LINE(this->center.x, this->center.y, centerEnd.x, centerEnd.y);

    LINE(linksEnd.x, linksEnd.y, rechtsEnd.x, rechtsEnd.y);
    LINE(this->center.x, this->center.y, rechtsEnd.x, rechtsEnd.y);
    LINE(this->center.x, this->center.y, linksEnd.x, linksEnd.y);

    if (CIT->second.age == 3)
    {
      PEN("ff00ff", 20);
    }
    else if (CIT->second.age == 2)
    {
      PEN("970097", 20);
    }
    else
    {
      PEN("470047", 20);
    }
    CIRCLE(CIT->second.value.x*cos(CIT->second.value.y), CIT->second.value.x*sin(CIT->second.value.y), 20);
  }
}//end drawFieldContext


//draw radarGrid within ImageContext
void RadarGrid::drawImageContext()
{
  IMAGE_DRAWING_CONTEXT;
}//end drawImageContext
