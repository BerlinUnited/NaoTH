
/**
* @file StableBallDetector.cpp
*
* Implementation of class StableBallDetector
*
*/

#include "StableBallDetector.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include "Tools/Debug/Stopwatch.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>
#include "Tools/DoubleCamHelpers.h"

#include "Tools/Math/Geometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/MaximumScan.h"
#include "Tools/ImageProcessing/Filter.h"

StableBallDetector::StableBallDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:StableBallDetector:markPeakScanFull", "mark the scanned points in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:StableBallDetector:markPeakScan", "mark the scanned points in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:StableBallDetector:markPeak", "mark found maximum red peak in image", false);

  DEBUG_REQUEST_REGISTER("Vision:Detectors:StableBallDetector:drawScanlines", "", false);

  DEBUG_REQUEST_REGISTER("Vision:Detectors:StableBallDetector:draw_ball","..", false);  
}


void StableBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getBallPercept().reset();


  // STEP 1: find the starting point for the search
	Vector2i start;
  if(!findMaximumRedPoint(start) || listOfRedPoints.empty()) {
    return;
  }

  bool ballFound = false;

  // the points are sorted - the most red points are at the end
  double radius = -1;
  Vector2d center;
  for(int i = (int)listOfRedPoints.size()-1; i >= 0 ; i--) 
  {
    const Vector2i& point = listOfRedPoints[i];

    // the point is within the previously calculated ball
    if(radius > 0 && radius*radius > (center - Vector2d(point)).abs2()) {
      continue;
    }

    if(ckecknearBall(point, center, radius) >= params.minGreenPoints && radius > 0) {
      ballFound = true;
      break;
    }
  }

  if(ballFound) {

    /*
    Vector2d betterCenter;
    double betterRadius;
    ckecknearBall(center, betterCenter, betterRadius);

    calculateBallPercept(betterCenter, betterRadius);
    */
    calculateBallPercept(center, radius);
  }
}


bool StableBallDetector::findMaximumRedPoint(Vector2i& peakPos)
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return false;
  }

  const FieldPercept::FieldPoly& fieldPolygon = getFieldPercept().getValidField();

  // find the top point of the polygon
  int minY = getImage().height();
  for(int i = 0; i < fieldPolygon.length ; i++)
  {
    if(fieldPolygon.points[i].y < minY && fieldPolygon.points[i].y >= 0) {
      minY = fieldPolygon.points[i].y;
    }
  }

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return false;
  }


  //
  // STEP II: calculate the step size for the scan
  //
  int stepSizeAdjusted = params.stepSize;    
  if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(40))
  {
    stepSizeAdjusted *= 3;
  }
  else if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(10))
  {
    stepSizeAdjusted *= 2;
  }

  listOfRedPoints.clear();

  int maxRedPeak = getFieldColorPercept().range.getMax().v; // initialize with the maximal red croma of the field color
  Vector2i point;
  Pixel pixel;

  for(point.y = minY; point.y < (int) getImage().height() - 3; point.y += stepSizeAdjusted) {
    for(point.x = 0; point.x < (int) getImage().width(); point.x += stepSizeAdjusted)
    {
      getImage().get(point.x, point.y, pixel);
     
      DEBUG_REQUEST("Vision:Detectors:StableBallDetector:markPeakScanFull",
        POINT_PX(ColorClasses::blue, point.x, point.y);
      );

      if
      (
        maxRedPeak < pixel.v && // "v" is the croma RED channel
        isOrange(pixel) &&
        fieldPolygon.isInside_inline(point) // only points inside the field polygon
        //&& !getGoalPostHistograms().isPostColor(pixel) // ball is not goal like colored
        //&& getGoalPostHistograms().histogramV.mean + params.minOffsetToGoalV < pixel.v
      )
      {
        //if(ckecknearBall(point) > 1) 
        {
          maxRedPeak = (int)pixel.v;
          peakPos = point;
          listOfRedPoints.push_back(point);
        }
      }

      DEBUG_REQUEST("Vision:Detectors:StableBallDetector:markPeakScan",
        if
        (
          isOrange(pixel) &&
          fieldPolygon.isInside_inline(point) // only points inside the field polygon
        )
        {
          POINT_PX(ColorClasses::red, point.x, point.y);
        }
      );
    }
  }

  DEBUG_REQUEST("Vision:Detectors:StableBallDetector:markPeak",
    LINE_PX(ColorClasses::skyblue, peakPos.x-10, peakPos.y, peakPos.x+10, peakPos.y);
    LINE_PX(ColorClasses::skyblue, peakPos.x, peakPos.y-10, peakPos.x, peakPos.y+10);
    CIRCLE_PX(ColorClasses::white, peakPos.x, peakPos.y, 5);
  );

  // maxRedPeak is larger than its initial value
  return maxRedPeak > getFieldColorPercept().range.getMax().v;
}


int StableBallDetector::ckecknearBall(const Vector2i& start, Vector2d& center, double& radius)
{
  int green = 0;
  Vector2i endPoint;
  static std::vector<Vector2i> endPoints;
  endPoints.clear();

  if(scanForEdges(start, Vector2d(1,0), endPoint))
    endPoints.push_back(endPoint);
  if(scanForEdges(start, Vector2d(-1,0), endPoint))
  endPoints.push_back(endPoint);
  if(scanForEdges(start, Vector2d(0,1), endPoint))
  endPoints.push_back(endPoint);
  if(scanForEdges(start, Vector2d(0,-1), endPoint))
  endPoints.push_back(endPoint);

  if(scanForEdges(start, Vector2d(1,1).normalize(), endPoint))
  endPoints.push_back(endPoint);
  if(scanForEdges(start, Vector2d(-1,1).normalize(), endPoint))
  endPoints.push_back(endPoint);
  if(scanForEdges(start, Vector2d(-1,1).normalize(), endPoint))
  endPoints.push_back(endPoint);
  if(scanForEdges(start, Vector2d(-1,-1).normalize(), endPoint))
  endPoints.push_back(endPoint);

  // ... just in case ;)
  //ASSERT(endPoints.size() == 8);


  /*
  Vector2d sum;
  for(size_t i = 0; i < endPoints.size(); i++) {
    sum += endPoints[i];
  }
  
  center = sum/((double)endPoints.size());

  RingBufferWithSum<double, 8> radiusBuffer;
  for(size_t i = 0; i < endPoints.size(); i++) {
    radiusBuffer.add((center - Vector2d(endPoints[i])).abs2());
  }

  radius = sqrt(radiusBuffer.getAverage());
  */

  for(size_t i = 0; i < endPoints.size(); i++) {
    POINT_PX(ColorClasses::red, endPoints[i].x, endPoints[i].y);
  }
  Geometry::calculateCircle(endPoints, center, radius);

  return 7;
}

bool StableBallDetector::scanForEdges(const Vector2i& start, const Vector2d& direction, Vector2i& endpoint)
{
  Vector2i point(start);
  BresenhamLineScan scanner(point, direction, getImage().cameraInfo);

  // initialize the scanner
  double t_edge = 6;
  MODIFY("StableBallDetector:t_edge", t_edge);

  //Vector2i peak_point_max(start);
  Vector2i peak_point_min(start);
  //MaximumScan<Vector2i,double> positiveScan(peak_point_max, (int)t_edge);
  MaximumScan<Vector2i,double> negativeScan(peak_point_min, (int)t_edge);

  //Filter<Diff5x1, Vector2i, double, 5> filter;

  Pixel pixel;
  getImage().get(point.x, point.y, pixel);
  int f_last = (int)pixel.v - (int)pixel.u; // scan the first point
  Vector2i lastPoint(point);
  Vector2i lastBallPoint;
  bool edgeFound = false;

  //int max_length = 3;
  //int i = 0;
  while(scanner.getNextWithCheck(point) && scanner.getNextWithCheck(point) /*&& i < max_length*/)
  {
    getImage().get(point.x, point.y, pixel);
    int f_y = (int)pixel.v - (int)pixel.u;
    double g = (f_y - f_last) / Vector2d(point - lastPoint).abs();
    f_last = f_y;
    lastPoint = point;

    DEBUG_REQUEST("Vision:Detectors:StableBallDetector:drawScanlines",
      POINT_PX(ColorClasses::blue, point.x, point.y);
    );

    /*
    if(!isOrange(pixel)) {
      i++;
    } else {
      i = 0;
      lastBallPoint = point;
      edgeFound = true;
    }
    */

    /*
    // begin found
    if(positiveScan.add(point, g))
    {
      DEBUG_REQUEST("Vision:Detectors:StableBallDetector:drawScanlines",
        POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y);
      );
      lastBallPoint = point;
      edgeFound = true;
      break;
    }
    */

    // end found
    if(negativeScan.add(point, -g))
    {
      DEBUG_REQUEST("Vision:Detectors:StableBallDetector:drawScanlines",
        POINT_PX(ColorClasses::pink, peak_point_min.x, peak_point_min.y);
      );
      lastBallPoint = peak_point_min;
      edgeFound = true;
      break;
    }
  }//end while

  if(edgeFound) {
    endpoint = lastBallPoint;
  } else {
    endpoint = point;
  }

  return edgeFound; //getFieldColorPercept().isFieldColor(pixel);
}//end scanForEdges


void StableBallDetector::calculateBallPercept(const Vector2i& center, double radius)
{
  // calculate the ball
    bool projectionOK = CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  center.x, 
		  center.y, 
		  getFieldInfo().ballRadius,
		  getBallPercept().bearingBasedOffsetOnField);

    // HACK: don't take to far balls
    projectionOK = projectionOK && getBallPercept().bearingBasedOffsetOnField.abs2() < 10000*10000; // closer than 10m

    // HACK: if the ball center is in image it has to be in the field polygon 
    Vector2d ballPointToCheck(center.x, center.y - 5);
    projectionOK = projectionOK && 
      (!getImage().isInside((int)(ballPointToCheck.x+0.5), (int)(ballPointToCheck.y+0.5)) ||
        getFieldPercept().getValidField().isInside(ballPointToCheck));


    if(projectionOK) 
    {
		  getBallPercept().radiusInImage = radius;
		  getBallPercept().centerInImage = center;
		  getBallPercept().ballWasSeen = projectionOK;
		  getBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
		
      DEBUG_REQUEST("Vision:Detectors:StableBallDetector:draw_ball",
        CIRCLE_PX(ColorClasses::orange, center.x, center.y, (int)(radius+0.5));
		  );
    }
}
