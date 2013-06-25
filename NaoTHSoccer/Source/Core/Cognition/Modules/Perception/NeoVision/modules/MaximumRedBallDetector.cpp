
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


MaximumRedBallDetector::MaximumRedBallDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:markPeak", "mark found maximum red peak in image", false);
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_ball_candidates","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_ball_outtakes","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:MaximumRedBallDetector:draw_ball","..", false);  

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
      if(maxRedPeak < pixel.v && 
         poly.isInside_inline(point) && 
         !getFieldColorPercept().isFieldColor(pixel))
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


/**********************************************************************************
    Mathematical Foundations
  ---------------------------------------------------------------------------------
  Given: Array of Points (xi, yi), i = 1,...,n belonging to the circle.
  Consider the following circle model: (x-xi)^2 + (y-yi)^2 = r^2
  where (x, y) is the center and r the radius of the circle.

  Now we make the following substitution to make the problem linear:
    r^2 = a^2/4 + b^2/4 - c
    x   = -a/2
    y   = -b/2
  Thus we get the following transformated formula
      (x-xi)^2 + (y-yi)^2 = r^2
  <=> x^2 - 2xxi + xi^2 + y^2 - 2yyi yi^2 - r^2 = 0
  (substitution)
  <=> (-a/2)^2 - 2(-a/2)xi + xi^2 + (-b/2)^2 - 2(-b/2)yi + yi^2 - (a^2/4 + b^2/4 - c) = 0
  <=> a^2/4 + axi + xi^2 + b^2/4 + byi + yi^2 - a^2/4 - b^2/4 + c = 0
      +----------------------------------+
  <=> | axi + xi^2 + byi + yi^2 + c = 0  |
      +----------------------------------+
  <=>   axi + byi + c = - xi^2 - yi^2

  Thus we can write the Problem as a linear equation:

    Ax=B
  
  where 

    x = (a, b, c)^t
    B = - ( (x1^2 + y1^2), ... , (xn^2 + yn^2) )^t
    
        x1  y1  1
        .   .   .
    A = .   .   .
        .   .   .
        xn  yn  1
  
  Now we solve the problem my means of Least Square Method
    x = (A^tA)^(-1)A^tB
**********************************************************************************/


bool MaximumRedBallDetector::calculateCircle( const BallPointList& ballPoints, Vector2<double>& center, double& radius )
{
  if (ballPoints.length < 3)
    return false;
  
  /********************************************************
   * create the matrix M := A^tA and the vector v := A^tB
   ********************************************************/

  double Mx = 0.0;   // \sum_{k=1}^n x_k
  double My = 0.0;   // \sum_{k=1}^n y_k
  double Mxx = 0.0;  // \sum_{k=1}^n x_k^2
  double Myy = 0.0;  // \sum_{k=1}^n y_k^2
  double Mxy = 0.0;  // \sum_{k=1}^n y_k * x_k
  double Mz = 0.0;   // \sum_{k=1}^n (y_k^2 + x_k^2) = Myy + Mxx
  double Mxz = 0.0;  // \sum_{k=1}^n x_k*(y_k^2 + x_k^2)
  double Myz = 0.0;  // \sum_{k=1}^n y_k*(y_k^2 + x_k^2)
  double n = ballPoints.length;

  // calculate the entries for A^tA and A^tB
  for (int i = 0; i < n; ++i)
  {
    double x = ballPoints[i].x;
    double y = ballPoints[i].y;
    double xx = x*x;
    double yy = y*y;
    double z = xx + yy;
    
    Mx += x;
    My += y;
    Mxx += xx;
    Myy += yy;
    Mxy += x*y;
    Mz += z;
    Mxz += x*z;
    Myz += y*z;
  }//end for
  
  try
  {
    // create the matrix M := A^tA
    Matrix_nxn<double, 3> M;
    double Matrix[9] = 
    {
        Mxx, Mxy, Mx,
        Mxy, Myy, My,
        Mx , My , n
    };
    M = Matrix;
    
    // create the vector v := A^tB
    Vector_n<double, 3> v;
    
    v[0] = -Mxz;
    v[1] = -Myz;
    v[2] = -Mz;


    /********************************************************
     * solve the Problem Mx = v <=> (A^tA)x = A^tB
     ********************************************************/
    Vector_n<double, 3> x;
    x = M.solve(v);
    
    center.x = (int)(-x[0] / 2.0);
    center.y = (int)(-x[1] / 2.0);
    

    double tmpWurzel = x[0]*x[0]/4.0 + x[1]*x[1]/4.0 - x[2];
    
    if (tmpWurzel < 0.0)
      return false;
    
    radius = sqrt(tmpWurzel);
  
  }
  catch (MVException)
  {
    return false;
  }
  catch (...)
  {
    //OUTPUT(idText, text, "Unknown exception in BallDetector::calculateCircle");
    return false;
  }
  
  return true;
}//end calculateCircle


bool MaximumRedBallDetector::findBall () {
	Vector2<int> start;
	findMaximumRedPoint(start);

	Vector2<double> p1(getArtificialHorizon().begin());
	Vector2<double> p2(getArtificialHorizon().end());
	if(start.y <= min(p1.y, p2.y) || !checkIfPixelIsOrange(start)) 
	{
		return false;
	}
	goodPoints.clear();
	badPoints.clear();
	bestPoints.clear();
	Pixel pixel;  
	getImage().get(start.x, start.y, pixel);
  double diff = getBaseColorRegionPercept().spanWidthEnv.y / 100;
	double dynamicThresholdY = getBaseColorRegionPercept().maxEnv.y + (255 - getBaseColorRegionPercept().maxEnv.y) * 0.5 - diff;
	bool isBright = pixel.y > dynamicThresholdY;
  //bool isDark = pixel.y < getBaseColorRegionPerceptTop().minEnv.y + diff;
	GradientSpiderScan spiderSearch(getImage(), cameraID);
	spiderSearch.setCurrentGradientThreshold(params.gradientThreshold);
	spiderSearch.setDynamicThresholdY(dynamicThresholdY);
	spiderSearch.setCurrentMeanThreshold(params.meanThreshold);
	spiderSearch.setMaxBeamLength(50);
	if(!getBodyContour().isOccupied(start) && !isBright/* && !isDark*/)
	{ 
		DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_scanlines",
		  spiderSearch.setDrawScanLines(true);
		);
  
		// explore the border of the ball
		spiderSearch.scan(start, goodPoints, badPoints);
    double radius = 0;
    Vector2d center;
    if(calculateCircle(goodPoints, center, radius) && checkIfPixelIsOrange(center))
    {
		  for (int i = 0; i < goodPoints.length; i++) 
      {
			  bestPoints.add(goodPoints[i]);
		  }
		  //start = getCenterOfMass(goodPoints);
      goodPoints.clear();
		  badPoints.clear();
		  spiderSearch.scan(start, goodPoints, badPoints);
      if(calculateCircle(goodPoints, center, radius))
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
	if(pointList.length > 3)
	{
		Vector2<double> center;
		double radius;

		int idxBest = -1;
		int bestCount = 0;
		double bestErr = -1;
		Vector2<double> centerBest;
		double radiusBest = 0;

    //std::cout << "==========================" << std::endl;
		for(int j = 0; j < pointList.length; j++)
		{
		  possibleModells[0].add(pointList[j]);
		}
/*		int maxTries = Math::min(pointList.length, 10);
		for(int i = 1; i < maxTries; i++)
		{
		int idx1 = rand() % maxTries;
		int idx2 = rand() % maxTries;
		int idx3 = rand() % maxTries;
		for (int j=0;j<maxTries && (idx1==idx2 || idx2==idx3 || idx1==idx3); j++) {
			idx1 = rand() % maxTries;
			idx2 = rand() % maxTries;
			idx3 = rand() % maxTries;
		}
		possibleModells[i].clear();
		double meanDist = 0;
		for(int j = 0; j < pointList.length; j++)
		{
			if(j == idx1 || j == idx2 || j == idx3)
			{
			possibleModells[i].add(pointList[j]);
			meanDist += (pointList[j] - start).abs();
			}		  
		}
		meanDist /= possibleModells[i].length;

		if(calculateCircle(possibleModells[i], center, radius))
		{
			if(meanDist < radius * 0.5)
			{
			DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_outtakes",
				CIRCLE_PX(ColorClasses::red, (int) center.x, (int) center.y, (int) radius);
			);
			return;
			}

			DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_candidates",
			CIRCLE_PX(ColorClasses::skyblue, (int) center.x, (int) center.y, (int) radius);
			);
			double radiusErrMax =  0.05 * radius;
			int count = 0;
			double meanErr = 0;
			for(int jj = 0; jj < pointList.length; jj++)
			{
			double err = fabs((center - pointList[jj]).abs() - radius);
            
			if(err <= radiusErrMax)
			{
				  count++;
				  meanErr += err;
			}
			}
			if(count > bestCount && (meanErr < bestErr || bestErr < 0) )
			{
			idxBest =  i;
			centerBest = center;
			radiusBest = radius;
			bestCount = count;
			bestErr = meanErr;
			}
		}
		}*/

		int firstPoint = -1,
			secondPoint = 1,
			thirdPoint = 2;
		
			
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
			/*double meanDist = (pointList[firstPoint] - start).abs() + (pointList[secondPoint] - start).abs() + (pointList[thirdPoint] - start).abs();		
			meanDist /= possibleModells[i].length;*/

			if(calculateCircle(possibleModells[i], center, radius))
			{
			/*	if(meanDist < radius * params.percentOfRadius)
				{
					DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_outtakes",
						CIRCLE_PX(ColorClasses::red, (int) center.x, (int) center.y, (int) radius);
					);			
				} else {*/
				DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_candidates",
					CIRCLE_PX(ColorClasses::skyblue, (int) center.x, (int) center.y, (int) radius);
				);
				double radiusErrMax =  params.ransacPercentValid * radius;
				int count = 0;
				double meanErr = 0;
				for(int jj = 0; jj < pointList.length; jj++)
				{
					double err = fabs((center - pointList[jj]).abs() - radius);
            
					if(err <= radiusErrMax)
					{
						count++;
						meanErr += err;
					}
				}
				meanErr /= pointList.length;
				if(count >= 4 && count >= pointList.length / 2 && (count >= bestCount || meanErr < bestErr || bestErr < 0) )
				{
					idxBest =  i;
					centerBest = center;
					radiusBest = radius;
					bestCount = count;
					bestErr = meanErr;
				}
			}
		}

		// calculate the percept
		if(idxBest >= 0 && radiusBest > 4 && radiusBest < 130)//pointList.length / 2)
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

bool MaximumRedBallDetector::checkIfPixelIsOrange(Vector2d coord) 
{
	Pixel pixel;
  if(coord.x < 0 || coord.y < 0 || coord.x >= getImage().width() || coord.y >= getImage().height())
    return false;
	getImage().get((int) coord.x, (int) coord.y, pixel);
	if (pixel.v > pixel.u + params.maxBlueValue)
		return true;
	return false;
}

void MaximumRedBallDetector::clearDublicatePoints (BallPointList& ballPoints) {
	vector<Vector2<int>> dublicatePointList;
	bool foundDublicate = false;
	for (int i=0;i<ballPoints.length; i++){
		for(int j=i+1; j<ballPoints.length; j++) {
			if (ballPoints[i]==ballPoints[j]) {
				foundDublicate = true;	
			}
		}
		if (!foundDublicate) {
			dublicatePointList.push_back(ballPoints[i]);
		} else {
			foundDublicate = false;
		}
	}
	if (dublicatePointList.size()>0) {
		ballPoints.clear();
		for (int i=0; i <(int) dublicatePointList.size(); i++) {
			ballPoints.add(dublicatePointList[i]);			
		}	
	}
}

