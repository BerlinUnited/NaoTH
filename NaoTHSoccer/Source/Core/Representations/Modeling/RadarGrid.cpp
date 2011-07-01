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

#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Debug/DebugBufferedOutput.h"

//class constructor
RadarGrid::RadarGrid()
  :
angleResolution(20),
center(0,0),
nearUpdate(0.5),
farUpdate(0.2)
{
//  values.resize(18);
  for (int i = 0; i < 18; ++i)
  {
    this->values[i]=0;
  }
}//end RadarGrid

//get value
void RadarGrid::get(double angle, double& value)
{
  int position = this->getIndexByAngle(angle);
  value = this->values[position];
}//end get()

//set the grid with value
void RadarGrid::set(Vector2<double> value)
{
  double angle = (value - this->center).angle();
  int position = this->getIndexByAngle(angle);
  //do we have a value?
  double oldValue = this->values[position];
  double newValue = (this->center - value).abs();
  if (oldValue == 0)
  {
    this->values[position] = newValue;
  }
  //yes, we do -> update the value
  else
  {
    if (oldValue >= newValue)
    {
      this->values[position] -= (oldValue - newValue)*nearUpdate;
    }
    else
    {
      this->values[position] += (newValue - oldValue)*farUpdate;
    }
  }
}//end set()

//set the grid with value
void RadarGrid::set(double angle, double value)
{
  int position = this->getIndexByAngle(angle);
  this->values[position] = value;
}//end set

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

void RadarGrid::reset()
{
  for (int i = 0; i < 18; ++i)
  {
    this->values[i] = 0;
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
    //temp.x += (*RGI)*cos(Math::fromDegrees(10 + i*20));
    //temp.y += (*RGI)*sin(Math::fromDegrees(10 + i*20));
    temp.x = 1500*cos(Math::fromDegrees(10+i*20));
    temp.y = 1500*sin(Math::fromDegrees(10+i*20));
    LINE(this->center.x, this->center.y, temp.x, temp.y);
  }
  //draw grid values
  //std::vector<double>::const_iterator RGI = values.begin();
  //int i = 0;
  Vector2<double> temp2(0,0);
  //for (; RGI != values.end(); ++RGI)
  for (int i = 0; i < 18; ++i)
  {
    
    PEN("FF0000", 6);
    //temp.x += (*RGI)*cos(Math::fromDegrees(10 + i*20));
    //temp.y += (*RGI)*sin(Math::fromDegrees(10 + i*20));
    Vector2<double> rechtsEnd, linksEnd;

    linksEnd.x = values[i]*cos(Math::fromDegrees(i*20-10));
    linksEnd.y = -values[i]*sin(Math::fromDegrees(i*20-10));
    rechtsEnd.x = values[i]*cos(Math::fromDegrees(i*20+10));
    rechtsEnd.y = -values[i]*sin(Math::fromDegrees(i*20+10));

    LINE(linksEnd.x, linksEnd.y, rechtsEnd.x, rechtsEnd.y);
    LINE(this->center.x, this->center.y, rechtsEnd.x, rechtsEnd.y);
    LINE(this->center.x, this->center.y, linksEnd.x, linksEnd.y);
    //CIRCLE(temp2.x, temp2.y, 20);
  }
}//end drawFieldContext


//draw radarGrid within ImageContext
void RadarGrid::drawImageContext()
{
  IMAGE_DRAWING_CONTEXT;
}//end drawImageContext
