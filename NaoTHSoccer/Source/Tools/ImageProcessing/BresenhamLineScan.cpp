/**
* @file Tools/ImageProcessing/BresenhamLineScan.cpp
* 
* Utility class which performs the Bresenham algorithm for line scanning
*
* @author <a href="mailto:timlaue@tzi.de">Tim Laue</a>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
* @author <a href="mailto:oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a> (revised constructors and commenting) 
*/

#include "BresenhamLineScan.h"

#include "Tools/Math/Geometry.h"

BresenhamLineScan::BresenhamLineScan()
{
  double dir = Math::pi_2;
  setup(dir);
}

BresenhamLineScan::BresenhamLineScan(const Vector2i& start, const Vector2i& end)
{
  setup(start, end);
}

BresenhamLineScan::BresenhamLineScan(const Vector2d& direction)
{
  setup(direction);
}

BresenhamLineScan::BresenhamLineScan(const double direction)
{
  setup(direction);
}

BresenhamLineScan::BresenhamLineScan(const Vector2i& start, const Vector2d& direction, const unsigned int width, const unsigned int height)
{
  setup(start, direction, width, height);
}

BresenhamLineScan::BresenhamLineScan(const Vector2i& start, const double direction, const unsigned int width, const unsigned int height)
{
  setup(start, direction, width, height);
}

BresenhamLineScan::BresenhamLineScan(const Math::Line& line, const unsigned int width, const unsigned int height)
{
  setup(line, width, height);
}



void BresenhamLineScan::setup(const Vector2i& diff)
{
  int dx = diff.x;
  int dy = diff.y;
  int incX = ((dx>0) ? 1:-1);
  int incY = ((dy>0) ? 1:-1);
  int absDx(abs(dx));
  int absDy(abs(dy));
  alongX = (absDy < absDx);
  if(alongX)
  {
    baseError = -absDx;
    delta = 2*absDy;
    standardOffset.x = incX;
    standardOffset.y = 0;
    correctionOffset.x = 0;
    correctionOffset.y = incY;
    numberOfPixels = absDx;
  }
  else
  {
    baseError = -absDy;
    delta = 2*absDx;
    standardOffset.x = 0;
    standardOffset.y = incY;
    correctionOffset.x = incX;
    correctionOffset.y = 0;
    numberOfPixels = absDy;
  }
  resetError = 2*baseError;
  error = baseError;        //added, for lines != 45 degrees
  pixelCount = 0;
}//end setup

void BresenhamLineScan::setup(const Vector2i& start, const Vector2i& end)
{
  setup(end-start);
}

void BresenhamLineScan::setup(const double direction)
{
  setup(Vector2i(static_cast<int>(cos(direction)*1024.0), static_cast<int>(sin(direction)*1024.0)));
}

void BresenhamLineScan::setup(const Vector2d& direction)
{
  setup(Vector2i(static_cast<int>(direction.x*1024.0), static_cast<int>(direction.y*1024.0)));
}

void BresenhamLineScan::setup(const Vector2i& start, const double direction, const unsigned int width, const unsigned int height)
{
  // Create a line through the point start with the given direction
  Math::Line line(Pose2D(direction, start));

  setup(line, width, height);
}

void BresenhamLineScan::setup(const Vector2i& start, const Vector2d& direction, const unsigned int width, const unsigned int height)
{
  // Create a line through the point start with the given direction
  Math::Line line(start, direction);

  setup(line, width, height);
}

void BresenhamLineScan::setup(const Math::Line& line, const unsigned int width, const unsigned int height)
{
  Vector2i pointOne;
  Vector2i pointTwo;

  const Vector2i frameUpperLeft(0,0);
  const Vector2i frameLowerRight(width-1, height-1);
  Geometry::getIntersectionPointsOfLineAndRectangle(frameUpperLeft, frameLowerRight, line, pointOne, pointTwo);

  setup(pointTwo - line.getBase());
}//end setup


