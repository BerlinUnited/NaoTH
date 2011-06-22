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
#include "Tools/Debug/DebugImageDrawings.h"
#include <vector>


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
	error = baseError;				//added, for lines != 45 degrees
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
  intersectionPointsWithImageFrame(line, cameraInfo, pointOne, pointTwo);

  setup(pointTwo - line.getBase());
}//end setup

/*
bool BresenhamLineScan::intersectionPointsWithImageFrame(
  const Math::Line& line, const CameraInfo& cameraInfo,
  Vector2<int>& startPoint, Vector2<int>& endPoint)
{
  int yMax = cameraInfo.resolutionHeight;
  int xMax = cameraInfo.resolutionWidth;

  // base has to be inside the image
  if(line.getBase().x < 0 || line.getBase().y < 0 ||
     line.getBase().x >= xMax || line.getBase().y >= yMax)
    return false;

  startPoint = line.getBase();
  endPoint = line.getBase();

  // create the line frame for the image only once
  // TODO: unify it with Polygon in Math
  static Math::LineSegment borderLines[4] = {
    //left border line
    Math::LineSegment(Vector2<int>(0, 0), Vector2<int>(0, yMax)),
    //bottom border line
    Math::LineSegment(Vector2<int>(0, 0), Vector2<int>(xMax, 0)),
    //right border line
    Math::LineSegment(Vector2<int>(xMax, yMax), Vector2<int>(xMax,0)),
    //top border line
    Math::LineSegment(Vector2<int>(xMax, yMax), Vector2<int>(0, yMax))
  };

  Vector2<int> foundPoints[2];
  double offset[2];
  int numberOfIntersections = 0;

	// Intersect line with image border
  for(unsigned int i = 0; i < 4; i++)
  {
    double s = borderLines[i].Line::intersection(line);
    
    // the intersection point is inside the border segment
    if(s >= 0 && s <= borderLines[i].getLength())
    {
      // check whether it is the start-point or 
      Vector2<int> point = borderLines[i].point(s);
      point.x = min(point.x, xMax-1);
      point.y = min(point.y, yMax-1);
      
      // check for the corner
      if(numberOfIntersections > 0 && foundPoints[0] == point)
        continue;

      POINT_PX(ColorClasses::black, point.x, point.y);
      offset[numberOfIntersections] = line.Line::intersection(borderLines[i]);
      foundPoints[numberOfIntersections++] = point;

      if(numberOfIntersections > 1) 
        break;
    }//end if
  }//end for

  //ASSERT(numberOfIntersections == 2);
  // TODO: fix it:
  if(numberOfIntersections != 2)
    return false;

  if(offset[0] > offset[1])
  {
    startPoint = foundPoints[1];
    endPoint = foundPoints[0];
  }else
  {
    startPoint = foundPoints[0];
    endPoint = foundPoints[1];
  }

  return true;
}//end intersectionPointsWithImageFrame
*/

bool BresenhamLineScan::intersectionPointsWithImageFrame(
  const Math::Line& line, 
  const CameraInfo& cameraInfo,
  Vector2<int>& point1, 
  Vector2<int>& point2)
{
  const Vector2<int> topRight(0,0);
  const Vector2<int> bottomLeft(cameraInfo.resolutionWidth, cameraInfo.resolutionHeight);
  int foundPoints=0;
  Vector2<double> point[2];
  if (line.getDirection().x!=0)
  {
    double y1=line.getBase().y+(bottomLeft.x-line.getBase().x)*line.getDirection().y/line.getDirection().x;
    if ((y1>=bottomLeft.y)&&(y1<=topRight.y))
    {
      point[foundPoints].x=(double) bottomLeft.x;
      point[foundPoints++].y=y1;
    }
    double y2=line.getBase().y+(topRight.x-line.getBase().x)*line.getDirection().y/line.getDirection().x;
    if ((y2>=bottomLeft.y)&&(y2<=topRight.y))
    {
      point[foundPoints].x=(double) topRight.x;
      point[foundPoints++].y=y2;
    }
  }
  if (line.getDirection().y!=0)
  {
    double x1=line.getBase().x+(bottomLeft.y-line.getBase().y)*line.getDirection().x/line.getDirection().y;
    if ((x1>=bottomLeft.x)&&(x1<=topRight.x)&&(foundPoints<2))
    {
      point[foundPoints].x=x1;
      point[foundPoints].y=(double) bottomLeft.y;
      if ((foundPoints==0)||((point[0]-point[1]).abs()>0.1))
      {
        foundPoints++;
      }
    }
    double x2=line.getBase().x+(topRight.y-line.getBase().y)*line.getDirection().x/line.getDirection().y;
    if ((x2>=bottomLeft.x)&&(x2<=topRight.x)&&(foundPoints<2))
    {
      point[foundPoints].x=x2;
      point[foundPoints].y=(double) topRight.y;
      if ((foundPoints==0)||((point[0]-point[1]).abs()>0.1))
      {
        foundPoints++;
      }
    }
  }
  switch (foundPoints)
  {
  case 1:
    point1.x=(int)point[0].x;
    point2.x=point1.x;
    point1.y=(int)point[0].y;
    point2.y=point1.y;
    foundPoints++;
    return true;
  case 2:
    if ((point[1]-point[0])*line.getDirection() > 0)
    {
      point1.x=(int)point[0].x;
      point1.y=(int)point[0].y;
      point2.x=(int)point[1].x;
      point2.y=(int)point[1].y;
    }
    else
    {
      point1.x=(int)point[1].x;
      point1.y=(int)point[1].y;
      point2.x=(int)point[0].x;
      point2.y=(int)point[0].y;
    }
    return true;
  default:
    return false;
  }
}//end intersectionPointsWithImageFrame


