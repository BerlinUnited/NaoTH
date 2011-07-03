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
angleResolution(20),
center(0,0),
nearUpdate(0.5),
farUpdate(0.2)
{
  values.resize(18);
  for (int i = 0; i < 18; ++i)
  {
    this->values[i].value.x = 0;
    this->values[i].value.y = 0;
    this->values[i].age = 0;
  }
  MODIFY("RadarGrid:nearUpdate", nearUpdate);
  MODIFY("RadarGrid:farUpdate", farUpdate);
}//end RadarGrid

//get value
void RadarGrid::get(double angle, Vector2<double>& value)
{
  int position = this->getIndexByAngle(angle);
  value = this->values[position].value;
}//end get()

//set the grid with value
void RadarGrid::set(Vector2<double> value)
{
  //translate to polar coordinate system
  //estimate the angle
  double newAngle = value.angle();
  //estimate distance
  double newDist = value.abs();

  int position = this->getIndexByAngle(newAngle);

  //store the old values
  double oldDist = this->values[position].value.x;
  double oldAngle = this->values[position].value.y;

  //do we have a value?
  if (oldDist == 0)
  {
    this->values[position].value.x = newDist;
    this->values[position].value.y = newAngle;
    this->values[position].age = 3;
  }
  //yes, we do -> update the value
  else
  {
    //if the new point lies closer that the old one
    if (oldDist >= newDist)
    {
      this->values[position].value.x -= (oldDist - newDist)*nearUpdate;
    }
    //else
    else
    {
      this->values[position].value.x += (newDist - oldDist)*farUpdate;
    }
    if (abs(oldAngle) >= abs(newAngle))
    {
      this->values[position].value.y -= (oldAngle - newAngle)*nearUpdate;
    }
    //else
    else
    {
      this->values[position].value.x += (newAngle - oldAngle)*farUpdate;
    }//end else
    this->values[position].age = 3;
  }//end else
}//end set()


//get index by angle
int RadarGrid::getIndexByAngle(const double& angle)
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
  for (int i = 0; i < 18; i++)
  {
    if(this->values[i].age > 0)
    {
      this->values[i].age--;
    }//end if
  }//end for
}//end ageGrid

void RadarGrid::resetCell(int& index)
{
  this->values[index].value.x = 0;
  this->values[index].value.y = 0;
  this->values[index].age = 0;
}

//update the model:
//apply the odometry data, erase old values
void RadarGrid::updateGrid(Pose2D& odometryDelta)
{
  std::vector<Cell> newValues;
  newValues.resize(18);
  for (int i = 0; i < 18; i++)
  {
    //erase all values that are older then 1
    if (this->values[i].age <= 0)
    {
      resetCell(i);
      newValues[i] = this->values[i];
    }
    else if (this->values[i].age <= 2)
    {
      Vector2<double> temp;
      temp = applyOdometry(i, odometryDelta);
      newValues[getIndexByAngle(temp.y)].value = temp;
      newValues[getIndexByAngle(temp.y)].age = this->values[i].age;
    }
    else
    {
      newValues[i] = this->values[i];
    }
  }//end for
  values = newValues;
}

Vector2<double> RadarGrid::applyOdometry(int& index, Pose2D& odometryDelta)
{
  Vector2<double> newCell;
  //take the value and translate
  //to Cartesian coordinates
  Vector2<double> temp;
  temp.x = this->values[index].value.x*cos(this->values[index].value.y);
  temp.y = this->values[index].value.x*sin(this->values[index].value.y);
  //apply odometry translation
  temp = odometryDelta * temp;
  //translate to polar coordinates and
  newCell.x = temp.abs();
  newCell.y = temp.angle();
  return newCell;
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
  for (int i = 0; i < 18; ++i)
  {
    PEN("FF0000", 6);
    Vector2<double> rechtsEnd, linksEnd;

    linksEnd.x = values[i].value.x*cos(Math::fromDegrees(i*20-10));
    linksEnd.y = -values[i].value.x*sin(Math::fromDegrees(i*20-10));
    rechtsEnd.x = values[i].value.x*cos(Math::fromDegrees(i*20+10));
    rechtsEnd.y = -values[i].value.x*sin(Math::fromDegrees(i*20+10));

    LINE(linksEnd.x, linksEnd.y, rechtsEnd.x, rechtsEnd.y);
    LINE(this->center.x, this->center.y, rechtsEnd.x, rechtsEnd.y);
    LINE(this->center.x, this->center.y, linksEnd.x, linksEnd.y);

    PEN("FFFF00", 20);
    CIRCLE(values[i].value.x*cos(values[i].value.y), values[i].value.x*sin(values[i].value.y), 20);
  }
}//end drawFieldContext


//draw radarGrid within ImageContext
void RadarGrid::drawImageContext()
{
  IMAGE_DRAWING_CONTEXT;
}//end drawImageContext
