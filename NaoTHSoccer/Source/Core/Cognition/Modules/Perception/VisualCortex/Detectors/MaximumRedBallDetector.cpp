
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
  dynamicThresholdY(255.0),
  possibleModells(570)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:markPeak", "mark found maximum red peak in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:markPeakScan", "mark the scanned points in image", false);
  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:second_scan_execute", "..", true);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:mark_best_points","",false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:mark_best_matching_points","",false);

  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:draw_ball_candidates","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:draw_ball_outtakes","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:draw_ball","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:use_brute_force","..", true);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:use_VU_difference","..", false);  

  DEBUG_REQUEST_REGISTER("Vision:Detectors:MaximumRedBallDetector:draw_size_ball_field", "draw the size based position of the ball on field", false);
}


void MaximumRedBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getBallPercept().reset();

  double diff = getOverTimeHistogram().spanWidthEnv.y * 0.35 + 0.5;
	dynamicThresholdY = getOverTimeHistogram().maxEnv.y - diff;

  findBall();
}//end execute

bool MaximumRedBallDetector::findMaximumRedPoint(Vector2i& peakPos)
{
  if(!getFieldPercept().valid) {
    return false;
  }

  FieldPercept::FieldPoly poly = getFieldPercept().getValidField();

  
  // find the top corner of the polygon
  int minY = 5000;
  for(int i = 0; i < poly.length ; i++)
  {
    if(poly.points[i].y < minY && poly.points[i].y > 0) {
      minY = poly.points[i].y;
    }
  }

  // polygon is empty
  if(minY == 5000 || minY < 0) {
    return false;
  }

  Pixel pixel;
  int maxRedPeak = -1;
  poly = getFieldPercept().getValidField();
  Vector2i point;

  for(point.y = minY; point.y < (int) getImage().height() - 3 ; point.y += params.stepSize) {
    for(point.x = 0; point.x < (int) getImage().width(); point.x += params.stepSize)
    {
      getImage().get(point.x, point.y, pixel);
      if
      (
        maxRedPeak < pixel.v && 
        poly.isInside_inline(point) && 
        checkIfPixelIsOrange(pixel)
        && !getBodyContour().isOccupied(point)
      )
      {
        maxRedPeak = pixel.v;
        peakPos = point;
      }

      DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:markPeakScan",
        POINT_PX(ColorClasses::red, point.x, point.y);
      );
    }
  }

  DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:markPeak",
    LINE_PX(ColorClasses::skyblue, peakPos.x-3, peakPos.y, peakPos.x+3, peakPos.y);
    LINE_PX(ColorClasses::skyblue, peakPos.x, peakPos.y-3, peakPos.x, peakPos.y+3);
  );

  return maxRedPeak >= 0;
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

    DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:draw_size_ball_field",
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

  // STEP 1: find the starting point for the search
	Vector2i start;
	if(!findMaximumRedPoint(start)) {
    return false;
  }


  // STEP 2: scan for the border points
  GradientSpiderScan spiderSearch(getImage(), cameraID);
  spiderSearch.setImageColorChannelNumber(2); // scan in the V channel
	spiderSearch.setCurrentGradientThreshold(params.gradientThreshold);
	spiderSearch.setDynamicThresholdY(dynamicThresholdY);
	spiderSearch.setCurrentMeanThreshold(params.meanThreshold);
	spiderSearch.setMaxBeamLength(params.maxScanlineSteps);

  DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:use_VU_difference",
    spiderSearch.setUseVUdifference(true);
  );

	DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:draw_scanlines",
		spiderSearch.setDrawScanLines(true);
	);
  
  goodPoints.clear(); // 
	badPoints.clear(); // 
	bestPoints.clear(); //
  // explore the border of the ball
	spiderSearch.scan(start, goodPoints, badPoints);

  // no good points found
  if(goodPoints.length <= 0) {
    return false;
  }

	for (int i = 0; i < goodPoints.length; i++) {
		bestPoints.add(goodPoints[i]);
	}

  // execute a second scan from the center of mass from the first scan
  DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:second_scan_execute",
    Vector2i center = getCenterOfMass(goodPoints);
    Pixel centerPixel;
    getImage().get(center.x, center.y, centerPixel);

    if(checkIfPixelIsOrange(centerPixel))
    {
      goodPoints.clear();
		  badPoints.clear();
		  spiderSearch.scan(center, goodPoints, badPoints);
    
		  for (int i = 0; i < goodPoints.length; i++) {
			  bestPoints.add(goodPoints[i]);
		  }
    }
  );

  GT_TRACE("MaximumRedBallDetector:4");
  clearDublicatePoints(bestPoints);

  // display the final points
  DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:mark_best_points",
    for(int i = 0; i < bestPoints.length; i++) {
      LINE_PX(ColorClasses::red, bestPoints[i].x-2, bestPoints[i].y, bestPoints[i].x-2, bestPoints[i].y);
      LINE_PX(ColorClasses::red, bestPoints[i].x, bestPoints[i].y-2, bestPoints[i].x, bestPoints[i].y+2);
    }
  );

  GT_TRACE("MaximumRedBallDetector:5");
  // STEP 3: try to match a ball model
	return getBestModel(bestPoints, start);
}

bool MaximumRedBallDetector::getBestModel(const BallPointList& pointList, const Vector2i& start)
{
  GT_TRACE("MaximumRedBallDetector:2");
  if(pointList.length < 3) {
    return false;
  }
  
	Vector2d centerBest;
	double radiusBest;

  bool bestModelFound = false;
  bool useBruteForce = false;

  DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:use_brute_force",
    useBruteForce = true;
  );
  
  if(useBruteForce) {
    bestModelFound = getBestBallBruteForce(pointList, start, centerBest, radiusBest);
  } else {
    bestModelFound = getBestBallRansac(pointList, start, centerBest, radiusBest);
  }

	// calculate the percept
	if(bestModelFound)
	{
		bool projectionOK = CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  centerBest.x, 
		  centerBest.y, 
		  getFieldInfo().ballRadius,
		  getBallPercept().bearingBasedOffsetOnField);

		getBallPercept().radiusInImage = radiusBest;
		getBallPercept().centerInImage = centerBest;
		getBallPercept().ballWasSeen = projectionOK;
		getBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
		
    DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:draw_ball",
			CIRCLE_PX(ColorClasses::orange, (int) centerBest.x, (int) centerBest.y, (int) radiusBest);
		);
		DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:draw_size_ball_field",
			estimatePositionBySize();
		);

    return true;
	}

  return false;
}

bool MaximumRedBallDetector::getBestBallBruteForce(const BallPointList& pointList, const Vector2i& start, Vector2d& centerBest, double& radiusBest)
{
	int idxBest = -1;
	int bestCount = 0;
	Vector2d center;
	double radius = 0;
	double bestErr = 10000.0;
  Vector2i boundingBoxMin(getImage().width() - 1, getImage().height() - 1);
  Vector2i boundingBoxMax;
  	GT_TRACE("MaximumRedBallDetector:1");
  // initialize the first model with all avaliable points
  possibleModells[0].clear();
	for(int j = 0; j < pointList.length; j++) 
  {
		possibleModells[0].add(pointList[j]);

    boundingBoxMin.x = min(boundingBoxMin.x, pointList[j].x);
    boundingBoxMin.y = min(boundingBoxMin.y, pointList[j].y);
    boundingBoxMax.x = max(boundingBoxMax.x, pointList[j].x);
    boundingBoxMax.y = max(boundingBoxMax.y, pointList[j].y);
	}

  double diag2 = (boundingBoxMax - boundingBoxMin).abs2();

	int firstPoint = -1;
	int	secondPoint = 1;
	int	thirdPoint = 2;		
			
  BallPointList list;

  for(size_t i = 1; i < possibleModells.size(); i++)
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
		  DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:draw_ball_candidates",
			  CIRCLE_PX(ColorClasses::skyblue, (int) center.x, (int) center.y, (int) radius);
		  );

      // calculate the number of inliers
		  double radiusErrMax =  params.ransacPercentValid * radius;
		  int count = 0;
      double meanError = 0.0;
      list.clear();
		  for(int j = 0; j < pointList.length; j++)
		  {
			  double err = fabs((center - Vector2d(pointList[j])).abs() - radius);
			  if(err <= radiusErrMax) {
				  count++;
          list.add(pointList[j]);
			  }
        meanError += err;
		  }

      if(pointList.length > 0) {
        meanError /= (double)(pointList.length);
      }

		  if( count*2 >= pointList.length && 
          (count > bestCount || (count == bestCount && meanError < bestErr) ) &&
          (Vector2d(start) - center).abs2() <= radius*radius &&
          radius >= params.minSizeInImage && radius <= params.maxSizeInImage &&
          diag2 > radius * radius 
      )
		  {
			  centerBest = center;
			  radiusBest = radius;
			  bestCount = count;
        bestErr = meanError;
        idxBest = (int) i;
        bestMatchedBallpoints.clear();
	      for(int j = 0; j < list.length; j++)
	      {
          bestMatchedBallpoints.add(list[j]);
	      }
		  }
	  }
	}


	DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:mark_best_matching_points",
    if(idxBest >= 0)
    {
		  drawUsedPoints(bestMatchedBallpoints);
    }
	);

  return idxBest >= 0;
}

bool MaximumRedBallDetector::getBestBallRansac(const BallPointList& pointList, const Vector2i& start, Vector2d& centerBest, double& radiusBest)
{
	int maxTries = min(pointList.length, params.maxRansacTries);
	int idxBest = -1;
	int bestCount = 0;
	Vector2d center;
	double radius = 0;
	double bestErr = 10000.0;
  BallPointList list;
  Vector2i boundingBoxMin(getImage().width() - 1, getImage().height() - 1);
  Vector2i boundingBoxMax;

  // initialize the first model with all avaliable points
  possibleModells[0].clear();
	for(int j = 0; j < pointList.length; j++) 
  {
		possibleModells[0].add(pointList[j]);
    boundingBoxMin.x = min(boundingBoxMin.x, pointList[j].x);
    boundingBoxMin.y = min(boundingBoxMin.y, pointList[j].y);
    boundingBoxMax.x = max(boundingBoxMax.x, pointList[j].x);
    boundingBoxMax.y = max(boundingBoxMax.y, pointList[j].y);
	}

  double diag2 = (boundingBoxMax - boundingBoxMin).abs2();

  for(int i = 1; i < maxTries && i < (int)possibleModells.size(); i++)
	{
    int idx1 = Math::random(pointList.length);
		int idx2 = Math::random(pointList.length);
		int idx3 = Math::random(pointList.length);

		for (int j=0;j<maxTries && (idx1==idx2 || idx2==idx3 || idx1==idx3); j++) 
    {
			idx1 = Math::random(pointList.length);
			idx2 = Math::random(pointList.length);
			idx3 = Math::random(pointList.length);
		}

		possibleModells[i].clear();

    if(idx1==idx2 || idx2==idx3 || idx1==idx3) {
      continue;
    }

    possibleModells[i].add(pointList[idx1]);
    possibleModells[i].add(pointList[idx2]);
    possibleModells[i].add(pointList[idx3]);

    if(Geometry::calculateCircle(possibleModells[i], center, radius))
	  {
		  DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:draw_ball_candidates",
			  CIRCLE_PX(ColorClasses::skyblue, (int) center.x, (int) center.y, (int) radius);
		  );

      // calculate the number of inliers
		  double radiusErrMax =  params.ransacPercentValid * radius;
		  int count = 0;
      double meanError = 0.0;
      list.clear();
		  for(int j = 0; j < pointList.length; j++)
		  {
			  double err = fabs((center - Vector2d(pointList[j])).abs() - radius);
			  if(err <= radiusErrMax) {
				  count++;
          list.add(pointList[j]);
			  }
        meanError += err;
		  }

      if(pointList.length > 0) {
        meanError /= (double)(pointList.length);
      }

		  if( count*2 >= pointList.length && 
          (count > bestCount || (count == bestCount && meanError < bestErr) ) &&
          (Vector2d(start) - center).abs2() <= radius*radius &&
          radius >= params.minSizeInImage && radius <= params.maxSizeInImage &&
          diag2 > radius * radius 
      )
		  {
			  centerBest = center;
			  radiusBest = radius;
			  bestCount = count;
        bestErr = meanError;
        idxBest = i;
        bestMatchedBallpoints.clear();
	      for(int j = 0; j < list.length; j++)
	      {
          bestMatchedBallpoints.add(list[j]);
	      }
		  }
	  }
  }

	DEBUG_REQUEST("Vision:Detectors:MaximumRedBallDetector:mark_best_matching_points",
    if(idxBest >= 0)
    {
		  drawUsedPoints(bestMatchedBallpoints);
    }
	);

  return idxBest >= 0;
}

void MaximumRedBallDetector::drawUsedPoints(const BallPointList& pointList)
{
	for(int i = 0; i < pointList.length; i++)
	{
    LINE_PX(ColorClasses::green, pointList[i].x-2, pointList[i].y, pointList[i].x-2, pointList[i].y);
    LINE_PX(ColorClasses::green, pointList[i].x, pointList[i].y-2, pointList[i].x, pointList[i].y+2);
	}
}

Vector2i MaximumRedBallDetector::getCenterOfMass (BallPointList& pointList) 
{
 	Vector2d mean;
  ASSERT(pointList.length > 0);
	for (int i =0; i< pointList.length; i++)
  {
		mean += pointList[i];
	}
	mean /= pointList.length;
	return mean;
}

bool MaximumRedBallDetector::checkIfPixelIsOrange(const Pixel& pixel) 
{
  return
    pixel.v > pixel.u + params.maxBlueValue && // check the UV-difference
    pixel.v > params.maxRedValue &&
    !getFieldColorPercept().isFieldColor(pixel) && // check green
    pixel.y < dynamicThresholdY; // check too bright (white etc.)
}

void MaximumRedBallDetector::clearDublicatePoints (BallPointList& ballPoints) 
{
	vector<Vector2i > dublicatePointList;
	
	for (int i=0;i<ballPoints.length; i++) 
  {
    bool foundDublicate = false;
		for(int j=i+1; j<ballPoints.length; j++) 
    {
			if (ballPoints[i]==ballPoints[j]) {
				foundDublicate = true;
        break;
			}
		}
		if (!foundDublicate) {
			dublicatePointList.push_back(ballPoints[i]);
		}
	}

  if ((int)dublicatePointList.size() < ballPoints.length) 
  {
		ballPoints.clear();
		for (size_t i=0; i < dublicatePointList.size(); i++) {
			ballPoints.add(dublicatePointList[i]);			
		}
	}
}

