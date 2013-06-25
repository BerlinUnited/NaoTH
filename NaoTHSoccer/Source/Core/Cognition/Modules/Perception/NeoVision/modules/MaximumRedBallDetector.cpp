
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

  Vector2<int> start;
  findMaximumRedPoint(start);

  Vector2<double> p1(getArtificialHorizon().begin());
  Vector2<double> p2(getArtificialHorizon().end());
  if(start.y <= min(p1.y, p2.y)) 
  {
    return;
  }

  goodPoints.clear();
  badPoints.clear();
  bestPoints.clear();

  if(!getBodyContour().isOccupied(start))
  {
    GradientSpiderScan spiderSearch(getImage(), cameraID);
    spiderSearch.setCurrentGradientThreshold(params.gradientThreshold);
    spiderSearch.setCurrentMeanThreshold(params.meanThreshold);
    spiderSearch.setMaxBeamLength(50);

    DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_scanlines",
      spiderSearch.setDrawScanLines(true);
    );
  
    // explore the border of the ball
    spiderSearch.scan(start, goodPoints, badPoints);

    if(goodPoints.length > 3)
    {
      Vector2<double> center;
      double radius;

      int idxBest = -1;
      int bestCount = 0;
      double bestErr = -1;
      Vector2<double> centerBest;
      double radiusBest = 0;

      int maxTries = Math::clamp(goodPoints.length, 3, 10);
      for(int j = 0; j < goodPoints.length; j++)
      {
        possibleModells[0].add(goodPoints[j]);
      }

      for(int i = 1; i < maxTries; i++)
      {
        int idx1 = rand() % maxTries;
        int idx2 = rand() % maxTries;
        int idx3 = rand() % maxTries;
        possibleModells[i].clear();
        double meanDist = 0;
        for(int j = 0; j < goodPoints.length; j++)
        {
          if(j == idx1 || j == idx2 || j == idx3)
          {
            possibleModells[i].add(goodPoints[j]);
            meanDist += (goodPoints[j] - start).abs();
          }
        }
        meanDist /= possibleModells[i].length;

        if(calculateCircle(possibleModells[i], center, radius))
        {
          if(meanDist < radius * params.percentOfRadius)
          {
            DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_outtakes",
              CIRCLE_PX(ColorClasses::red, (int) center.x, (int) center.y, (int) radius);
            );
            return;
          }

          DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_candidates",
            CIRCLE_PX(ColorClasses::skyblue, (int) center.x, (int) center.y, (int) radius);
          );
          double radiusErrMax =  params.ransacPercentValid * radius;
          int count = 0;
          double meanErr = 0;
          for(int jj = 0; jj < goodPoints.length; jj++)
          {
            double err = fabs((center - goodPoints[jj]).abs() - radius);
            
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
      }

      // calculate the percept
      if(idxBest >= 0 && bestCount >= 4 )//goodPoints.length / 2)
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
      }
    }
  }
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
    POINT_PX(ColorClasses::skyblue, peakPos.x - 1, peakPos.y + 1);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 1, peakPos.y + 1);
    POINT_PX(ColorClasses::skyblue, peakPos.x + 1, peakPos.y - 1);
    POINT_PX(ColorClasses::skyblue, peakPos.x - 1, peakPos.y - 1);
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

