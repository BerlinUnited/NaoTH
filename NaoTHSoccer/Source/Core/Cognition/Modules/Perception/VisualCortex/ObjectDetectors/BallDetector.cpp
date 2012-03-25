
/**
* @file BallDetector.cpp
*
* Implementation of class BallDetector
*
*/

#include "BallDetector.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/ImageProcessing/BlobList.h"
#include "Tools/CameraGeometry.h"

#include "Tools/Debug/Stopwatch.h"

BallDetector::BallDetector()
  : theBlobFinder(getColoredGrid())
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:mark_ball", "mark the ball on the image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:draw_scanlines", "draw all the scanlines", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:draw_scanlines_good_points", "draw all the godd points", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:draw_scanlines_bad_points", "draw all the bad points", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:mark_previous_ball", "draw the projection of the previous Ball Percept on the image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:random_scan", "draw the points touched by random scan", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BallDetector:mark_ball_blob", " ", false);

  // initialize the colors for the blob finder
  for(int n = 0; n < ColorClasses::numOfColors; n++)
  {
    connectedColors[n]=false;
  }
  connectedColors[ColorClasses::orange] = true;
}


void BallDetector::execute()
{
  //if there is no field percept, then, there is also no goal ?!
  /*
  if(!getFieldPercept().isValid())
  {
    getBallPercept().ballWasSeen = false;
    return;
  }
  */
  
  getBallPercept().reset();

  //initialize blob finder
  //theBlobFinder.init();

  BlobList blobList;
  //WholeArea wholeImageArea;

  //search only if field is marked as valid
  if(getFieldPercept().isValid())
  {
    //theBlobFinder.execute(blobList, connectedColors, wholeImageArea);
    STOPWATCH_START("BallDetector ~ BlobFinder");
    theBlobFinder.execute(blobList, connectedColors, getFieldPercept().getLargestValidPoly());
    STOPWATCH_STOP("BallDetector ~ BlobFinder");
  }


  // draw deteced blobs
  DEBUG_REQUEST("ImageProcessor:BallDetector:mark_ball_blob",
    for(int i = 0; i < blobList.blobNumber; i++)
    {
      const Blob& blob = blobList.blobs[i];
      RECT_PX(ColorClasses::orange,
              (unsigned int)(blob.upperLeft.x),
              (unsigned int)(blob.upperLeft.y),
              (unsigned int)(blob.lowerRight.x),
              (unsigned int)(blob.lowerRight.y));
      CIRCLE_PX(ColorClasses::orange, blob.centerOfMass.x, blob.centerOfMass.y, 4);
    }//end for
  );
  

  //search for ball if orange blobs were found
  if(blobList.blobNumber > 0)
  {
    int largest = blobList.getLargestBlob();
    const Vector2<int>& candidate = blobList.blobs[largest].centerOfMass;
    double area = blobList.blobs[largest].moments.getRawMoment(0, 0);
    
    if(area <= 1) // TODO: this limits the distance of ball recognition
    {
      // precise scan
      regionGrowExpandArea(candidate, getBallPercept().centerInImage, getBallPercept().radiusInImage);
      getBallPercept().ballWasSeen = true;
    }
    else
    {
      STOPWATCH_START("BallDetector ~ internalexecute");
      execute(candidate);
      STOPWATCH_STOP("BallDetector ~ internalexecute");
    }
  }
  else // no orange blobs found in the image 
  {
    Vector2<int> projectedBall = CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo,
        Vector3<double>(getBallPercept().bearingBasedOffsetOnField.x,
                        getBallPercept().bearingBasedOffsetOnField.y, 
                        getFieldInfo().ballRadius));

    int scanRange = (int)(2.0*getBallPercept().radiusInImage + 0.5);
    Vector2<int> candidate;

    Vector2<int> min(projectedBall.x - scanRange, projectedBall.y - scanRange);
    Vector2<int> max(projectedBall.x + scanRange, projectedBall.y + scanRange);

    if(randomScan( ColorClasses::orange, candidate, min, max))
    {
      execute(candidate);
    }

    //project the old percept in the image
    DEBUG_REQUEST("ImageProcessor:BallDetector:mark_previous_ball",
      CIRCLE_PX(ColorClasses::gray, (int)projectedBall.x, (int)projectedBall.y, (int)getBallPercept().radiusInImage);
    );
  } // end if-else


  //
  // now make a plausibility check
  //
  // get the horizon
  Vector2<double> p1(getCameraMatrix().horizon.begin());
  Vector2<double> p2(getCameraMatrix().horizon.end());
  
  // ball is over horizon
  if(getBallPercept().centerInImage.y < min(p1.y, p2.y))
    getBallPercept().ballWasSeen = false;






  /* this is highly experimental test
  Vector2<double> topBorderPoint = Geometry::imagePixelToFieldCoord(
    getCameraMatrix(),
    getBallPercept().centerInImage.x,
    getBallPercept().centerInImage.y - getBallPercept().radiusInImage,
    45);
  double estimatedBallRadius = (getBallPercept().bearingBasedOffsetOnField - topBorderPoint).abs();
  // the projected ball is more than as twice es big
  if(estimatedBallRadius > 200.0)
  getBallPercept().ballWasSeen = false;
  */


  /*
  Vector2<double> sbo; // size based
  double distanceFromCameraToBallCenter = 
    Geometry::getBallDistanceByAngleSize(int(2 * Global::getFieldDimensions().ballRadius), 2 * radiusAsAngle);
  double distanceFromCameraToBallCenterOnGround;
  if(distanceFromCameraToBallCenter > fabs(translationOfCamera.z - Global::getFieldDimensions().ballRadius))
    distanceFromCameraToBallCenterOnGround = sqrt(sqr(distanceFromCameraToBallCenter) - sqr(translationOfCamera.z  - Global::getFieldDimensions().ballRadius));
  else
    distanceFromCameraToBallCenterOnGround = 0;
  sbo.x = translationOfCamera.x + distanceFromCameraToBallCenterOnGround * cos(centerAsAngles.x);
  sbo.y = translationOfCamera.y + distanceFromCameraToBallCenterOnGround * sin(centerAsAngles.x);
   */
}//end execute


void BallDetector::execute(const Vector2<int>& start)
{
  BallPointList goodPoints;
  BallPointList badPoints;

  SpiderScan spiderSearch(getImage(), getColorTable64(), ColorClasses::orange, ColorClasses::green);
  spiderSearch.setMaxBeamLength(300);

  DEBUG_REQUEST("ImageProcessor:BallDetector:draw_scanlines",
    spiderSearch.setDrawScanLines(true);
  );
  

  // explore the border of the ball
  spiderSearch.scan(start, goodPoints, badPoints);

  // TODO: better conditions
  if(goodPoints.length < 3 || (goodPoints.length == 3 && calculateBase(goodPoints[0], goodPoints[1], goodPoints[2]) > 0.8))
    goodPoints.merge(badPoints);


  Vector2<double> centerCOM;
  double radiusCOM(0);

  // estimate the ball center as center of mas

  for(int i = 0; i < goodPoints.length; i++)
  {
    centerCOM += goodPoints[i];
  }
  for(int i = 0; i < badPoints.length; i++)
  {
    centerCOM += badPoints[i];
  }
  centerCOM /= ((double)goodPoints.length);

  for(int i = 0; i < goodPoints.length; i++)
  {
    radiusCOM = std::max(radiusCOM, (centerCOM - goodPoints[i]).abs());
  }
  for(int i = 0; i < badPoints.length; i++)
  {
    radiusCOM = std::max(radiusCOM, (centerCOM - badPoints[i]).abs());
  }


  Vector2<double> center;
  double radius(0);
  bool ballCouldBeEstimated = calculateCircle(goodPoints, center, radius);

  // check if the calculation of the ball is reliable
  if(!ballCouldBeEstimated ||
     //fabs(radius - radiusCOM) <  &&
     (center - centerCOM).abs() > 2*radiusCOM)
  {
    radius = radiusCOM;
    center = centerCOM;
  }//end if


  // set the ball request
  getBallPercept().centerInImage = center;
  getBallPercept().radiusInImage = radius;
  getBallPercept().ballWasSeen = true;

  DEBUG_REQUEST("ImageProcessor:BallDetector:mark_ball",
    CIRCLE_PX(ColorClasses::red, (int)center.x, (int)center.y, (int)radius);

    IMAGE_DRAWING_CONTEXT;
    PEN("FF9900", 1);
    CIRCLE(center.x, center.y, radius);
    CIRCLE_PX(ColorClasses::black, (unsigned int)(start.x), (unsigned int)(start.y), 1);
  );//end Debug

  DEBUG_REQUEST("ImageProcessor:BallDetector:draw_scanlines_good_points",
    for(int p=0; p<goodPoints.length; p++)
    {
      POINT_PX(ColorClasses::green, (unsigned int)(goodPoints.points[p].x),(unsigned int)(goodPoints.points[p].y));
    }//for
  );

  DEBUG_REQUEST("ImageProcessor:BallDetector:draw_scanlines_bad_points",
    for(int p=0; p<badPoints.length; p++)
    {
       POINT_PX(ColorClasses::gray, (unsigned int)(badPoints.points[p].x),(unsigned int)(badPoints.points[p].y));
    }//for
  );//end DEBUG
}//end execute


//calculate the base of three points
double BallDetector::calculateBase(Vector2<int>& x, Vector2<int>& y, Vector2<int>& z)
{
  Vector2<double> v = x-z;
  Vector2<double> w = y-z;

  v.normalize();
  w.normalize();

  return  fabs(v*w);
}//end calculateBase

bool BallDetector::randomScan(ColorClasses::Color color, Vector2<int>& result, const Vector2<int>& orgMin, const Vector2<int>& orgMax) const
{
  int maxNumberOfScanPoints = 30;
  
//  senity check
//  min.x = std::max(min.x, 0);
//  min.y = std::max(min.y, 0);
//  max.x = std::min(max.x, (int)getImage().cameraInfo.resolutionWidth-1);
//  max.y = std::min(max.y, (int)getImage().cameraInfo.resolutionHeight-1);
  // senity check

//  Vector2<int> min = getFieldPercept().getLargestValidPoly(getCameraMatrix().horizon).getClosestPoint(orgMin);
//  Vector2<int> max = getFieldPercept().getLargestValidPoly(getCameraMatrix().horizon).getClosestPoint(orgMax);
//  if(min.x >= max.x || min.y >= max.y) return false;


  // do scan
  for(int i = 0; i < maxNumberOfScanPoints; i++)
  {
    int x = 0;//Math::random(min.x, max.x);
    int y = 0;//Math::random(min.y, max.y);

    Pixel pixel = getImage().get(x,y);
    ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);

    
    DEBUG_REQUEST("ImageProcessor:BallDetector:random_scan",
      POINT_PX(currentPixelColor, x, y);
    );

    if(color == currentPixelColor)
    {
      result.x = x;
      result.y = y;
      return true;
    }
  }//end for

  return false;
}//end randomScan




void BallDetector::regionGrowExpandArea(
    const Vector2<int>& startingPoint,
    Vector2<double>& result,
    double& radius)
{

//  const int numberOfDirections = 8;
  Vector2<int> mask[] = 
  {
    Vector2<int>(-1,  0),
    Vector2<int>(-1, -1),
    Vector2<int>( 0, -1),
    Vector2<int>( 1, -1),
    Vector2<int>( 1,  0),
    Vector2<int>( 1,  1),
    Vector2<int>( 0,  1),
    Vector2<int>(-1,  1)
  };

  Vector2<double> center;
  double number = 0;
  Vector2<int> highest(startingPoint);
  Vector2<int> lowest(startingPoint);

  // TODO: check, its not a grid anymore
  //get width and height of the scaled image (uniformgrid)
  const int gridWidth =getImage().cameraInfo.resolutionWidth;
  const int gridHeight=getImage().cameraInfo.resolutionHeight;

  ColorClasses::Color color = ColorClasses::orange;

  //remember all the pixels that were visited
  static OccupancyGrid visitedPixels(gridWidth, gridHeight);
  //and reset them first
  visitedPixels.reset();

  //init a new blob creator
  //BlobCreator blobCreator;

  ArrayQueue<Vector2<int> > open;
  
  if(startingPoint.x >= 0 && startingPoint.x < gridWidth && 
     startingPoint.y >= 0 && startingPoint.y < gridHeight)
  {
    Pixel pixel = getImage().get(startingPoint.x, startingPoint.y);
    if(getColorTable64().getColorClass(pixel) == color)
      open.push(startingPoint);
  }

  STOPWATCH_START("BallDetector ~ regionGrowExpandArea");

  while(!open.empty())
  {
    // TODO: check if a reference can be used
    Vector2<int> currentPoint = open.front();
    open.pop();

    //bool currentPointIsBoundary = false;

    // (pixelIndex)
    //get the GRID coordinates of pixel
    const Pixel pixel = getImage().get(currentPoint.x, currentPoint.y);

    //check if the pixel wasn't treated allready and is of a relevant color
    if(visitedPixels(currentPoint.x, currentPoint.y)) continue; 
    
    if(getColorTable64().getColorClass(pixel) != color)
    {
//      currentPointIsBoundary = true;
      DEBUG_REQUEST("ImageProcessor:BallDetector:mark_ball_blob",
        POINT_PX(ColorClasses::red,
              (unsigned int)(currentPoint.x),
              (unsigned int)(currentPoint.y));
      );//end DEBUG
      continue;
    }//end if

    center += currentPoint;
    number++;

    if(lowest.y < currentPoint.y) lowest = currentPoint;
    if(highest.y > currentPoint.y) highest = currentPoint;

    //check if the pixel is inside the searchArea
    //TODO: give the Area in image coordinates
    //if(!searchArea.isInside(pixel)) continue;
           
    //the pixel is visited now
    visitedPixels.set(currentPoint.x, currentPoint.y, true);

    // add the pixel to the blob
    //blobCreator.addPoint(currentPoint);
    
//    bool currentPointIsImageBoundary = false;
    //check the neighboring pixels
    for(int i = 0; i < 8; i++)
    {
      Vector2<int> neighborPixel = currentPoint + mask[i];
      // check if the pixel is inside the grid
      // we have to ckeck it here, because in the case the pixel is 
      // outside the grid its index in scaledImageIndex is not defined
      if(neighborPixel.x >= 0 && neighborPixel.x < gridWidth && 
         neighborPixel.y >= 0 && neighborPixel.y < gridHeight &&
         !visitedPixels(neighborPixel.x, neighborPixel.y))
      {
        open.push(neighborPixel);
      }else
      {
//        currentPointIsImageBoundary = true;
      }
    }//end for

    // boundary
    DEBUG_REQUEST("ImageProcessor:BallDetector:mark_ball_blob",
      POINT_PX(color,
            (unsigned int)(currentPoint.x),
            (unsigned int)(currentPoint.y));
    );//end DEBUG
  }//end while
 
  STOPWATCH_STOP("BallDetector ~ regionGrowExpandArea");

  if(number > 0)
  {
    result = center/number;
    radius = (lowest - highest).abs()/2 + 1;
  }
}//end expandArea


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


bool BallDetector::calculateCircle( const BallPointList& ballPoints, Vector2<double>& center, double& radius )
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
