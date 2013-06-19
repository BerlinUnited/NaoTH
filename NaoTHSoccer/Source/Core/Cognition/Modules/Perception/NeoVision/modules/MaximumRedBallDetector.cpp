
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
}


void MaximumRedBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  double maxSize = 150;
  double minSize = 5;
  
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
    spiderSearch.setMaxBeamLength(50);

    DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_scanlines",
      spiderSearch.setDrawScanLines(true);
    );
  
    // explore the border of the ball
    spiderSearch.scan(start, goodPoints, badPoints);

    if(goodPoints.length > 2)
    {
      Vector2<double> center;
      Vector2<double> centerOne;
      Vector2<double> centerOneOnField;
      Vector2<double> borderOneOnField;
      Vector2<double> centerTwo;
      Vector2<double> centerTwoOnField;
      Vector2<double> borderTwoOnField;
      Vector2<double> meanCenter;
      double radius;
      double radiusOne;
      double radiusTwo;

      if(calculateCircle(goodPoints, centerOne, radiusOne))
      {
        CameraGeometry::imagePixelToFieldCoord(
          getCameraMatrix(), 
          getImage().cameraInfo,
          centerOne.x, 
          centerOne.y, 
          getFieldInfo().ballRadius,
          centerOneOnField);
        borderOneOnField.x = centerOneOnField.x + radiusOne;
        borderOneOnField.y = centerOneOnField.y;
        CameraGeometry::imagePixelToFieldCoord(
          getCameraMatrix(), 
          getImage().cameraInfo,
          centerOne.x, 
          centerOne.y, 
          getFieldInfo().ballRadius,
          borderOneOnField);

        bool sizeOk = (centerOneOnField - borderOneOnField).abs() <= getFieldInfo().ballRadius * 1.1;

        if(!getImage().isInside((int) centerOne.x, (int) centerOne.y) || !sizeOk || (radiusOne >= 30 && goodPoints.length < goodPoints.length / 2) || radiusOne < minSize || radiusOne > maxSize || (start - centerOne).abs() > 0.95 * radiusOne )
        {
          DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_outtakes",
            CIRCLE_PX(ColorClasses::red, (int) centerOne.x, (int) centerOne.y, (int) radiusOne);
          );
          return;
        }
        DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_candidates",
          CIRCLE_PX(ColorClasses::skyblue, (int) centerOne.x, (int) centerOne.y, (int) radiusOne);
        );

        //double radiusErrMax =  0.05 * radiusOne;
        //for(int i = 0; i < goodPoints.length; i++)
        //{
        //  double err = fabs((centerOne - goodPoints[i]).abs() - radiusOne); 
        //  if(err <= radiusErrMax)
        //  {
        //    bestPoints.add(goodPoints[i]);
        //  }
        //}

        goodPoints.clear();
        badPoints.clear();
        spiderSearch.scan((centerOne + start) / 2, goodPoints, badPoints);

        if(calculateCircle(goodPoints, centerTwo, radiusTwo))
        {
          CameraGeometry::imagePixelToFieldCoord(
            getCameraMatrix(), 
            getImage().cameraInfo,
            centerOne.x, 
            centerOne.y, 
            getFieldInfo().ballRadius,
            centerTwoOnField);
          borderTwoOnField.x = centerTwoOnField.x + radiusTwo;
          borderTwoOnField.y = centerTwoOnField.y;
          CameraGeometry::imagePixelToFieldCoord(
            getCameraMatrix(), 
            getImage().cameraInfo,
            centerOne.x, 
            centerOne.y, 
            getFieldInfo().ballRadius,
            borderTwoOnField);

          bool sizeOk = (centerTwoOnField - borderTwoOnField).abs() <= getFieldInfo().ballRadius * 1.1;

          if(!getImage().isInside((int) centerTwo.x, (int) centerTwo.y) || !sizeOk || (radiusTwo >= 30 && goodPoints.length < goodPoints.length / 2) || radiusTwo < minSize || radiusTwo > maxSize || (start - centerTwo).abs() > 0.95 * radiusTwo)
          {
            DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_outtakes",
              CIRCLE_PX(ColorClasses::red, (int) centerTwo.x, (int) centerTwo.y, (int) radiusTwo);
            );
            return;
          }
          DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball_candidates",
            CIRCLE_PX(ColorClasses::blue, (int) centerTwo.x, (int) centerTwo.y, (int) radiusTwo);
          );
          //double radiusErrMax =  0.05 * radiusTwo;
          //int idxMaxErr = 0;
          //double maxErr = 0;
          //vector<int> testedErrIdx;
          //while(idxMaxErr > 0)
          //{
          //  idxMaxErr = -1;
          //  for(int i = 0; i < goodPoints.length; i++)
          //  {
          //    double err = fabs((centerTwo - goodPoints[i]).abs() - radiusTwo); 
          //    if(err <= radiusErrMax && err > maxErr && testedErrIdx.)
          //    {
          //      maxErr = err;
          //      idxMaxErr = i;
          //    }
          //    bestPoints.add(goodPoints[i]);
          //  }
          //  if(idxMaxErr > 0)
          //  {
          //    testedErrIdx.push_back(idxMaxErr);
          //    bestPoints.remove(idxMaxErr);


          //    if(calculateCircle(bestPoints, center, radius)))
          //    {
          //      for(int t = 0; t < 3; t++)
          //      {
          //      }
          //    }
          //  }
            //for(int i = bestPoints.length; i >= 0; i--)
            //{
            //  double err = fabs((center - bestPoints[i]).abs() - radius); 
            //  if(err > radiusErrMax)
            //  {
            //    bestPoints.remove(i);
            //  }
            //}
            double radiusErrMax =  0.05 * radiusTwo;
            for(int i = 0; i < goodPoints.length; i++)
            {
              double err = fabs((centerTwo - goodPoints[i]).abs() - radiusTwo); 
              if(err <= radiusErrMax)
              {
                bestPoints.add(goodPoints[i]);
              }
            }
            if(calculateCircle(bestPoints, center, radius))
            {

              DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball",
                CIRCLE_PX(ColorClasses::pink, (int) center.x, (int) center.y, (int) radius);
              );

              getBallPercept().ballWasSeen = true;
              getBallPercept().centerInImage = center;
              getBallPercept().radiusInImage = radius;
              CameraGeometry::imagePixelToFieldCoord(
                getCameraMatrix(), 
                getImage().cameraInfo,
                center.x, 
                center.y, 
                getFieldInfo().ballRadius,
                getBallPercept().bearingBasedOffsetOnField);


            }
            //else
            //{
            //  DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball",
            //    CIRCLE_PX(ColorClasses::green, (int) centerTwo.x, (int) centerTwo.y, (int) radiusTwo);
            //  );
            //}

          //}       

          //double area = 2 * Math::pi * radius;
          //double radiusErrMax =  0.05 * radius;
          //bestPoints.clear();
          //double minErr = 5000;
          //int idxMin = -1; 
          //int idxMax = -1; 
          //double maxErr = 0;
          ////std::cout << "-------" << std::endl;
          //for(int i = 0; i < goodPoints.length; i++)
          //{
          //  double err = fabs((center - goodPoints[i]).abs() - radius); 
          //  if(err <= radiusErrMax)
          //  {
          //    bestPoints.add(goodPoints[i]);
          //    meanCenter += goodPoints[i];
          //  }
          //  //bestPoints.add(goodPoints[i]);
          //  //std::cout << "err: " << err << std::endl;
          //  //if(err > radiusErrMax && err > maxErr)
          //  //{
          //  //  idxMax = i;
          //  //  maxErr = err;
          //  //}
          //  //if(err > radiusErrMax && err < minErr)
          //  //{
          //  //  idxMin = i;
          //  //  minErr = err;
          //  //}
          //}
          ////if(idxMax > 0)
          ////{
          ////   bestPoints.remove(idxMax);
          ////}
          ////if(idxMin > 0)
          ////{
          ////   bestPoints.remove(idxMin);
          ////}

          //meanCenter /= bestPoints.length;

          //DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball",
          //  CIRCLE_PX(ColorClasses::green, (int) center.x, (int) center.y, (int) radius);
          //);

          //double radius_;
          //if(calculateCircle(bestPoints, meanCenter, radius_) && 2 * Math::pi * radius > area)
          //{
          //  center = meanCenter;
          //  radius = radius_;
          //  DEBUG_REQUEST("NeoVision:MaximumRedBallDetector:draw_ball",
          //    CIRCLE_PX(ColorClasses::blue, (int) meanCenter.x, (int) meanCenter.y, (int) radius_);
          //  );
          //}       
        }
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

  for(int y = min; y < max; y += params.stepSize)
  {
    for(int x = 0; x < (int) IMAGE_WIDTH; x += params.stepSize)
    {
      getImage().get(x, y, pixel);
      if(poly.isInside(Vector2<int>(x, y)))
      {
        if(maxRedPeak < pixel.v && !getFieldColorPercept().isFieldColor(pixel))
        {
          maxRedPeak = pixel.v;
          peakPos.y = y;
          peakPos.x = x;
        }
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

