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

#include "Tools/Math/Line.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include <vector>

using namespace naoth;

BresenhamLineScan::BresenhamLineScan()
{
  double dir = Math::pi_2;
  setup(dir);
}

BresenhamLineScan::BresenhamLineScan(const Vector2<int>& start, const Vector2<int>& end)
{
  setup(start, end);
}

BresenhamLineScan::BresenhamLineScan(const Vector2<double>& direction)
{
  setup(direction);
}

BresenhamLineScan::BresenhamLineScan(const double& direction)
{
  setup(direction);
}

BresenhamLineScan::BresenhamLineScan(const Vector2<int>& start, const Vector2<double>& direction, const CameraInfo& cameraInfo)
{
  setup(start, direction, cameraInfo);
}

BresenhamLineScan::BresenhamLineScan(const Vector2<int>& start, const double& direction, const CameraInfo& cameraInfo)
{
  setup(start, direction, cameraInfo);
}

BresenhamLineScan::BresenhamLineScan(const Math::Line& line, const CameraInfo& cameraInfo)
{
  setup(line, cameraInfo);
}



void BresenhamLineScan::setup(const Vector2<int>& diff)
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

void BresenhamLineScan::setup(const Vector2<int>& start, const Vector2<int>& end)
{
  setup(end-start);
}

void BresenhamLineScan::setup(const double& direction)
{
  setup(Vector2<int>(static_cast<int>(cos(direction)*1024.0), static_cast<int>(sin(direction)*1024.0)));
}

void BresenhamLineScan::setup(const Vector2<double>& direction)
{
  setup(Vector2<int>(static_cast<int>(direction.x*1024.0), static_cast<int>(direction.y*1024.0)));
}

void BresenhamLineScan::setup(const Vector2<int>& start, const double& direction, const CameraInfo& cameraInfo)
{
  // Create a line through the point start with the given direction
  Math::Line line(Pose2D(direction, start));

  setup(line, cameraInfo);
}

void BresenhamLineScan::setup(const Vector2<int>& start, const Vector2<double>& direction, const CameraInfo& cameraInfo)
{
  // Create a line through the point start with the given direction
  Math::Line line(start, direction);

  setup(line, cameraInfo);
}

void BresenhamLineScan::setup(const Math::Line& line, const CameraInfo& cameraInfo)
{
  Vector2<int> pointOne;
  Vector2<int> pointTwo;

  const Vector2<int> frameUpperLeft(0,0);
  const Vector2<int> frameLowerRight(cameraInfo.resolutionWidth-1, cameraInfo.resolutionHeight-1);
  Geometry::getIntersectionPointsOfLineAndRectangle(frameUpperLeft, frameLowerRight, line, pointOne, pointTwo);

  setup(pointTwo - line.getBase());
}//end setup


