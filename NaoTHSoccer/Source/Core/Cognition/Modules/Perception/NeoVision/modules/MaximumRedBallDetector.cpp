
/**
* @file MaximumRedBallDetector.cpp
*
* Implementation of class MaximumRedBallDetector
*
*/

#include "MaximumRedBallDetector.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include "Tools/Debug/Stopwatch.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>
#include "Tools/DoubleCamHelpers.h"

#include "Tools/Math/Geometry.h"

MaximumRedBallDetector::MaximumRedBallDetector()
: 
  cameraID(CameraInfo::Bottom),
  dynamicThresholdY(255.0)
{
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:markPeak", "mark found maximum red peak in image", false);
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_ball_candidates","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_ball_outtakes","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_ball","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:use_brute_force","..", true);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:use_VU_difference","..", false);  

  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_size_ball_field", "draw the size based position of the ball on field", false);
}


void MaximumRedBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getBallPercept().reset();  

  findBall();
}//end execute

void MaximumRedBallDetector::findMaximumRedPoint(Vector2<int>& peakPos)
{
  FieldPercept::FieldPoly poly = getFieldPercept().getLargestValidPoly(getArtificialHorizon());

  int maxCount = 0;
  int max = 0;
  int min = 5000;
  bool markPeak = false;

  DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:markPeak",
      markPeak = true;
  );

  for(int i = 0; i < poly.length ; i++)
  {
    if(poly.points[i].y > max && maxCount < 3)
    {
      max = poly.points[i].y;
      poly.points[i].y = 0;
      maxCount++;
    }
    if(poly.points[i].y < min && poly.points[i].y > 0)
    {
      min = poly.points[i].y;
    }
  }
  //HACK: TODO: find where some polypoint.y values are set to zero
  if(min == 5000 || min < 0)
  {
    min = 0;
  }

  if(max >= (int) IMAGE_HEIGHT)
  {
    max = (int) IMAGE_HEIGHT - 1;
  }

  Pixel pixel;
  int maxRedPeak = 0;
  poly = getFieldPercept().getLargestValidPoly(getArtificialHorizon());
  Vector2<int> point;

  for(point.y = min; point.y < max; point.y += params.stepSize) 
  {
    for(point.x = 0; point.x < (int) getImage().width(); point.x += params.stepSize)
    {
      getImage().get(point.x, point.y, pixel);
      if
      (
        maxRedPeak < pixel.v && 
        poly.isInside_inline(point) && 
        checkIfPixelIsOrange(point)
      )
      {
        maxRedPeak = pixel.v;
        peakPos = point;
      }
    }
  }

  for(int y = max; y < (int) IMAGE_HEIGHT; y += params.stepSize)
  {
    for(int x = 0; x < (int) IMAGE_WIDTH; x += params.stepSize)
    {
      getImage().get(x, y, pixel);
      if(maxRedPeak < pixel.v && !getFieldColorPercept().isFieldColor(pixel))
      {
        maxRedPeak = pixel.v;
        peakPos.y = y;
        peakPos.x = x;
      }
    }
  }

  if(markPeak)
  {
    POINT_PX(ColorClasses::skyblue, peakPos.x, peakPos.y);
    POINT_PX(ColorClasses::skyblue, peakPos.x - 1, peakPos.y - 1);
    POINT_PX(ColorClasses::skyblue, peakPos.x - 1, peakPos.y + 1);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 1, peakPos.y - 1);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 1, peakPos.y + 1);
	
	POINT_PX(ColorClasses::skyblue, peakPos.x - 2, peakPos.y - 2);
    POINT_PX(ColorClasses::skyblue, peakPos.x - 2, peakPos.y + 2);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 2, peakPos.y - 2);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 2, peakPos.y + 2);

	POINT_PX(ColorClasses::skyblue, peakPos.x - 3, peakPos.y - 3);
    POINT_PX(ColorClasses::skyblue, peakPos.x - 3, peakPos.y + 3);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 3, peakPos.y - 3);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 3, peakPos.y + 3);
  }
}

// not ready yet
Vector2d MaximumRedBallDetector::estimatePositionBySize()
{
  double y = getBallPercept().centerInImage.y - getImage().cameraInfo.getOpticalCenterY();
  double f = getImage().cameraInfo.getFocalLength();
  double r = getBallPercept().radiusInImage;

  double alpha_y = atan2(y,f) - atan2(y-r,f);
  double q = -1;
    
  if(fabs(sin(alpha_y)) > 1e-3) {
    q = getFieldInfo().ballRadius / sin(alpha_y);
  }

  if(q > -1) {
    double h = getCameraMatrix().translation.z;
    double d = sqrt(q*q - h*h);
    // assume it is valid
    Vector2d ballOffset = getBallPercept().bearingBasedOffsetOnField;
    ballOffset.normalize(d);

    DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_size_ball_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF00FF", 10);
      CIRCLE(ballOffset.x, ballOffset.y, 32.5);
    );

    return ballOffset;
  }

  return Vector2d();
}//end estimatePositionBySize


bool MaximumRedBallDetector::findBall () 
{
	Vector2<int> start;
	Vector2<double> p1(getArtificialHorizon().begin());
	Vector2<double> p2(getArtificialHorizon().end());
	if(start.y <= min(p1.y, p2.y)) 
	{
		return false;
	}

	findMaximumRedPoint(start);

	goodPoints.clear();
	badPoints.clear();
	bestPoints.clear();

  GradientSpiderScan spiderSearch(getImage(), cameraID);
	spiderSearch.setCurrentGradientThreshold(params.gradientThreshold);
	spiderSearch.setDynamicThresholdY(dynamicThresholdY);
	spiderSearch.setCurrentMeanThreshold(params.meanThreshold);
	spiderSearch.setMaxBeamLength(50);
  DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:use_VU_difference",
    spiderSearch.setUseVUdifference(true);
  );
	if(!getBodyContour().isOccupied(start))
	{ 
		DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_scanlines",
		  spiderSearch.setDrawScanLines(true);
		);
  
		// explore the border of the ball
		spiderSearch.scan(start, goodPoints, badPoints);
    double radius = 0;
    Vector2d center;
    if(Geometry::calculateCircle(goodPoints, center, radius) && checkIfPixelIsOrange(center))
    {
		  for (int i = 0; i < goodPoints.length; i++) 
      {
			  bestPoints.add(goodPoints[i]);
		  }
      goodPoints.clear();
		  badPoints.clear();
      start.x = (int) center.x;
      start.y = (int) center.y;
		  spiderSearch.scan(start, goodPoints, badPoints);
      if(Geometry::calculateCircle(goodPoints, center, radius))
      {
		    for (int i = 0; i < goodPoints.length; i++) 
        {
			    bestPoints.add(goodPoints[i]);
		    }
      
	    }
    }
		return getBestModel(bestPoints);
  }

	return false;
}

bool MaximumRedBallDetector::getBestModel(BallPointList& pointList)
{
	clearDublicatePoints(pointList);
	if(pointList.length > 2)
	{
		Vector2<double> centerBest;
		double radiusBest;
    bool bestModelFound = false;
    bool useBruteForce = false;

    DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:use_brute_force",
      useBruteForce = true;
    );
  
    if(useBruteForce)
    {
      bestModelFound = getBestBallBruteForce(pointList, centerBest, radiusBest);
    }
    else
    {
      bestModelFound = getBestBallRansac(pointList, centerBest, radiusBest);
    }

		// calculate the percept
		if(bestModelFound && radiusBest > 4 && radiusBest < 130)
		{
			DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball",
			  CIRCLE_PX(ColorClasses::orange, (int) centerBest.x, (int) centerBest.y, (int) radiusBest);
			);
			CameraGeometry::imagePixelToFieldCoord(
			getCameraMatrix(), 
			getImage().cameraInfo,
			centerBest.x, 
			centerBest.y, 
			getFieldInfo().ballRadius,
			getBallPercept().bearingBasedOffsetOnField);

			getBallPercept().radiusInImage = radiusBest;
			getBallPercept().centerInImage = centerBest;
			getBallPercept().ballWasSeen = true;
			getBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
		
			DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_size_ball_field",
			  estimatePositionBySize();
			);
      return true;
		}
	}
  return false;
}

bool MaximumRedBallDetector::getBestBallBruteForce(BallPointList& pointList, Vector2<double>& centerBest, double& radiusBest)
{
	int idxBest = -1;
	int bestCount = 0;
	Vector2<double> center;
	double radius = 0;

	for(int j = 0; j < pointList.length; j++)
	{
		possibleModells[0].add(pointList[j]);
	}

	int firstPoint = -1;
	int	secondPoint = 1;
	int	thirdPoint = 2;		
			
	for(int i = 1; true ; i++)
	{
        
		firstPoint++;
		if (firstPoint == secondPoint)
    {
			firstPoint = 0;
			secondPoint++;
		}
		if (secondPoint == thirdPoint) 
    {
			secondPoint = 1;
			thirdPoint++;
		}
		if (thirdPoint == pointList.length) 
    {
			break;
		}
		possibleModells[i].clear();		
		possibleModells[i].add(pointList[firstPoint]);
		possibleModells[i].add(pointList[secondPoint]);
		possibleModells[i].add(pointList[thirdPoint]);

		if(Geometry::calculateCircle(possibleModells[i], center, radius))
		{
			DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_candidates",
				CIRCLE_PX(ColorClasses::skyblue, (int) center.x, (int) center.y, (int) radius);
			);
			double radiusErrMax =  params.ransacPercentValid * radius;
			int count = 0;
			for(int jj = 0; jj < pointList.length; jj++)
			{
				double err = fabs((center - pointList[jj]).abs() - radius);
				if(err <= radiusErrMax)
				{
					count++;
				}
			}
			if(count >= 3 && count >= pointList.length / 2 && count >= bestCount)
			{
				idxBest =  i;
				centerBest = center;
				radiusBest = radius;
				bestCount = count;
			}
		}
	}
  return idxBest >= 0;
}

bool MaximumRedBallDetector::getBestBallRansac(BallPointList& pointList, Vector2<double>& centerBest, double& radiusBest)
{
	int maxTries = min(pointList.length, 40);
  int bestCount = 0;
  Vector2d center;
  double radius = 0;
  int idxBest = -1;

	for(int i = 1; i < maxTries; i++)
	{
		int idx1 = rand() % maxTries;
		int idx2 = rand() % maxTries;
		int idx3 = rand() % maxTries;
		for (int j=0;j<maxTries && (idx1==idx2 || idx2==idx3 || idx1==idx3); j++) 
    {
			idx1 = rand() % maxTries;
			idx2 = rand() % maxTries;
			idx3 = rand() % maxTries;
		}
		possibleModells[i].clear();
		for(int j = 0; j < pointList.length; j++)
		{
			if(j == idx1 || j == idx2 || j == idx3)
			{
			possibleModells[i].add(pointList[j]);
			}		  
		}

		if(Geometry::calculateCircle(possibleModells[i], center, radius))
		{
			DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_candidates",
			CIRCLE_PX(ColorClasses::skyblue, (int) center.x, (int) center.y, (int) radius);
			);
			double radiusErrMax =  0.05 * radius;
			int count = 0;
			for(int jj = 0; jj < pointList.length; jj++)
			{
			  double err = fabs((center - pointList[jj]).abs() - radius);
            
			  if(err <= radiusErrMax)
			  {
				    count++;
			  }
			}
			if(count >= 3 && count >= pointList.length / 2 && count >= bestCount)
			{
			  idxBest =  i;
			  centerBest = center;
			  radiusBest = radius;
			  bestCount = count;
			}
		}
	}
  return idxBest >= 0;
}

Vector2<int> MaximumRedBallDetector::getCenterOfMass (BallPointList& pointList) 
{
	int x = 0;
	int y = 0;
	for (int i =0; i< pointList.length; i++){
		x += pointList[i].x;
		y += pointList[i].y;
	}
	x = x / pointList.length;
	y = y / pointList.length;
	return Vector2<int>(x,y);
}

bool MaximumRedBallDetector::checkIfPixelIsOrange(Vector2<int> point) 
{
	Pixel pixel;
	getImage().get(point.x, point.y, pixel);
  
  if
  (
    pixel.v > pixel.u + params.maxBlueValue && 
    !getFieldColorPercept().isFieldColor(pixel) && 
    pixel.y < dynamicThresholdY
  )
  {
		return true;
  }
	return false;
}

void MaximumRedBallDetector::clearDublicatePoints (BallPointList& ballPoints) 
{
	vector<Vector2<int> > dublicatePointList;
	bool foundDublicate = false;
	for (int i=0;i<ballPoints.length; i++)
  {
		for(int j=i+1; j<ballPoints.length; j++) 
    {
			if (ballPoints[i]==ballPoints[j]) 
      {
				foundDublicate = true;	
			}
		}
		if (!foundDublicate) 
    {
			dublicatePointList.push_back(ballPoints[i]);
		} else {
			foundDublicate = false;
		}
	}
	if (dublicatePointList.size()>0) 
  {
		ballPoints.clear();
		for (int i=0; i <(int) dublicatePointList.size(); i++) 
    {
			ballPoints.add(dublicatePointList[i]);			
		}	
	}
}

