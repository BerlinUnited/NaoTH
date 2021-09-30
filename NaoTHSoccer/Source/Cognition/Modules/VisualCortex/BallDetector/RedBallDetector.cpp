
/**
* @file RedBallDetector.cpp
*
* Implementation of class RedBallDetector
*
*/

#include "RedBallDetector.h"

#include "Tools/Math/Geometry.h"
#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/MaximumScan.h"
#include "Tools/ImageProcessing/Filter.h"

RedBallDetector::RedBallDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:peak_scan:mark_full", "mark the scanned points in image", false);
  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:peak_scan:mark_candidates", "mark the scanned points in image", false);
  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:peak_scan:mark_peak", "mark found maximum red peak in image", false);

  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:drawScanlines", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:drawScanEndPoints", "", false);

  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:draw_ball_estimated","..", false);
  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:draw_ball_radius_match", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:draw_ball","..", false);
  DEBUG_REQUEST_REGISTER("Vision:RedBallDetector:draw_sanity_samples","draw samples used for ball sanity check", false);

  getDebugParameterList().add(&params);
}

RedBallDetector::~RedBallDetector()
{
  getDebugParameterList().remove(&params);
}

void RedBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  // STEP 1: find the starting point for the search
  listOfRedPoints.clear();
  if(!findMaximumRedPoint(listOfRedPoints) || listOfRedPoints.empty()) {
    return;
  }

  double radius = -1;
  Vector2d center;

  // walk through the list of red points
  // NOTE: the points are sorted - the most red points are at the end
  std::vector<Vector2i>::reverse_iterator iter = listOfRedPoints.rbegin();
  for(; iter != listOfRedPoints.rend(); ++iter)
  {
    const Vector2i& point = *iter;

    // very simple clustering: the point is within the previously calculated ball
    if(radius > 0 && 2*radius*radius > Math::sqr(center.x - point.x) + Math::sqr(center.y - point.y)) {
      continue;
    }

    // estimate the ball radius based on the projected distance of the center point
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getCameraInfo(),
      getFieldInfo().ballRadius, point.x, point.y);

    DEBUG_REQUEST("Vision:RedBallDetector:draw_ball_estimated",
      //estimatedRadius <=0 possible? will be a problem  in "radius < 2*estimatedRadius"
      if(estimatedRadius > 0) {
        CIRCLE_PX(ColorClasses::white, point.x, point.y, (int)(estimatedRadius+0.5));
      }
    );

    // scan for edges in v-u with a spider scan
    ballEndPoints.clear();
    bool goodBallCandidateFound = spiderScan(point, ballEndPoints);

    if(goodBallCandidateFound && Geometry::calculateCircle(ballEndPoints, center, radius))
    {
      DEBUG_REQUEST("Vision:RedBallDetector:draw_ball_radius_match",
        CIRCLE_PX(ColorClasses::yellow, (int)(center.x+0.5), (int)(center.y+0.5), (int)(radius+0.5));
      );


      if(radius > 0 && radius < 2*estimatedRadius) {
        if(sanityCheck(center, radius))
        {
          calculateBallPercept(center, radius);
          break;
        }
      }
    }
  }
}


bool RedBallDetector::findMaximumRedPoint(std::vector<Vector2i>& points) const
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return false;
  }

  const FieldPercept::FieldPoly& fieldPolygon = getFieldPercept().getValidField();

  // find the top point of the polygon
  int minY = getFieldPercept().getMinY();

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return false;
  }

  //
  // STEP II: calculate the step size for the scan
  //

  // make the scan more corse when the camera looks more down
  int stepSizeAdjusted = params.stepSize;
  if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(40)) {
    stepSizeAdjusted *= 3;
  } else if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(10)) {
    stepSizeAdjusted *= 2;
  }

  int maxRedPeak = 128; // initialize with the neutral value
  Vector2i point;
  Pixel pixel;
  Vector2i redPeak;

  for(point.y = (int) (getImage().height() - 3); point.y > minY; point.y -= stepSizeAdjusted) {
    for(point.x = 0; point.x < (int) getImage().width(); point.x += stepSizeAdjusted)
    {
      getImage().get(point.x, point.y, pixel);

      DEBUG_REQUEST("Vision:RedBallDetector:peak_scan:mark_full",
        POINT_PX(ColorClasses::blue, point.x, point.y);
      );

      if (
        pixel.v > maxRedPeak && // "v" is the croma RED channel
        isOrange(pixel) &&
        fieldPolygon.isInside(point) // only points inside the field polygon
      )
      {
        maxRedPeak = (int)pixel.v;
        redPeak = point;
        points.push_back(point);
      }

      DEBUG_REQUEST("Vision:RedBallDetector:peak_scan:mark_candidates",
        // only points inside the field polygon
        if ( isOrange(pixel) && fieldPolygon.isInside(point) ) {
          POINT_PX(ColorClasses::red, point.x, point.y);
        }
      );
    }
  }

  DEBUG_REQUEST("Vision:RedBallDetector:peak_scan:mark_peak",
    LINE_PX(ColorClasses::skyblue, redPeak.x-10, redPeak.y, redPeak.x+10, redPeak.y);
    LINE_PX(ColorClasses::skyblue, redPeak.x, redPeak.y-10, redPeak.x, redPeak.y+10);
    CIRCLE_PX(ColorClasses::white, redPeak.x, redPeak.y, 5);
  );

  // maxRedPeak is larger than its initial value
  return maxRedPeak > 128;
}


bool RedBallDetector::spiderScan(const Vector2i& start, std::vector<Vector2i>& endPoints) const
{
  int goodBorderPointCount = 0;
  goodBorderPointCount += scanForEdges(start, Vector2d( 1, 0), endPoints);
  goodBorderPointCount += scanForEdges(start, Vector2d(-1, 0), endPoints);
  goodBorderPointCount += scanForEdges(start, Vector2d( 0, 1), endPoints);
  goodBorderPointCount += scanForEdges(start, Vector2d( 0,-1), endPoints);

  goodBorderPointCount += scanForEdges(start, Vector2d( 1, 1).normalize(), endPoints);
  goodBorderPointCount += scanForEdges(start, Vector2d(-1, 1).normalize(), endPoints);
  goodBorderPointCount += scanForEdges(start, Vector2d( 1,-1).normalize(), endPoints);
  goodBorderPointCount += scanForEdges(start, Vector2d(-1,-1).normalize(), endPoints);

  DEBUG_REQUEST("Vision:RedBallDetector:drawScanEndPoints",
    for(size_t i = 0; i < endPoints.size(); i++)
    {
      if(goodBorderPointCount == 0) {
        POINT_PX(ColorClasses::red, endPoints[i].x, endPoints[i].y);
      } else {
        POINT_PX(ColorClasses::green, endPoints[i].x, endPoints[i].y);
      }
    }
  );
  return goodBorderPointCount > 0;
}

bool RedBallDetector::scanForEdges(const Vector2i& start, const Vector2d& direction, std::vector<Vector2i>& points) const
{
  Vector2i point(start);
  BresenhamLineScan scanner(point, direction, getImage().cameraInfo);

  // initialize the scanner
  Vector2i peak_point_min(start);
  MaximumScan<Vector2i,double> negativeScan(peak_point_min, params.thresholdGradientUV);

  Filter<Prewitt3x1, Vector2i, double, 3> filter;

  Pixel pixel;
  double stepLength = 0;
  Vector2i lastPoint(point); // needed for step length

  while(scanner.getNextWithCheck(point))
  {
    getImage().get(point.x, point.y, pixel);
    int f_y = (int)pixel.v - (int)pixel.u;

    filter.add(point, f_y);
    if(!filter.ready()) {
      // assume the step length is constant, so we only calculate it in the starting phase of the filter
      stepLength += Vector2d(point - lastPoint).abs();
      lastPoint = point;
      ASSERT(stepLength > 0);
      continue;
    }

    DEBUG_REQUEST("Vision:RedBallDetector:drawScanlines",
      POINT_PX(ColorClasses::blue, point.x, point.y);
    );

    // jump down found
    // NOTE: we scale the filter value with the stepLength to acount for diagonal scans
    if(negativeScan.add(filter.point(), -filter.value()/stepLength))
    {
      DEBUG_REQUEST("Vision:RedBallDetector:drawScanlines",
        POINT_PX(ColorClasses::pink, peak_point_min.x, peak_point_min.y);
      );
      points.push_back(peak_point_min);

      return !getFieldColorPercept().greenHSISeparator.noColor(pixel);
    }
  }//end while

  return false;
}//end scanForEdges


void RedBallDetector::calculateBallPercept(const Vector2i& center, double radius)
{
  MultiBallPercept::BallPercept ballPercept;

  // calculate the ball
  bool ballOK = CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getCameraInfo(),
        center.x,
        center.y,
        getFieldInfo().ballRadius,
        ballPercept.positionOnField);

  // HACK: don't take to far balls
  ballOK = ballOK && ballPercept.positionOnField.abs2() < 10000 * 10000; // closer than 10m

  // HACK: if the ball center is in image it has to be in the field polygon
  Vector2d ballPointToCheck(center.x, center.y - 5);
  ballOK = ballOK &&
    (!getImage().isInside((int)(ballPointToCheck.x+0.5), (int)(ballPointToCheck.y+0.5)) ||
      getFieldPercept().getValidField().isInside(ballPointToCheck));

  if(ballOK)
  {
    ballPercept.cameraId = cameraID;
    ballPercept.centerInImage = center;
    ballPercept.radiusInImage = radius;

    getMultiBallPercept().add(ballPercept);
    getMultiBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();

    DEBUG_REQUEST("Vision:RedBallDetector:draw_ball",
      CIRCLE_PX(ColorClasses::orange, center.x, center.y, (int)(radius+0.5));
    );
  }
}

// not used right now ( do we need it?)
/*
void RedBallDetector::estimateCircleSimple(const std::vector<Vector2i>& endPoints, Vector2d& center, double& radius) const
{
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
}
*/

// NOTE: this is experimental and not used yet
bool RedBallDetector::randomBallScan(const Vector2i& center, double radius) const
{
  size_t sampleSize = 21;

  size_t goodPoints = 0;
  Pixel pixel;
  for(size_t i = 0; i < sampleSize; i++)
  {
    double a = Math::random()*Math::pi2;
    double d = Math::random()*radius;

    int x = center.x + (int)(cos(a)*d + 0.5);
    int y = center.y + (int)(sin(a)*d + 0.5);

    if(getImage().isInside(x,y))
    {
      getImage().get(x, y, pixel);
      if(isOrange(pixel)) {
        goodPoints++;
      }

      DEBUG_REQUEST("Vision:BallDetector:draw_sanity_samples",
        if(isOrange(pixel)) {
          POINT_PX(ColorClasses::green, x, y);
        } else {
          POINT_PX(ColorClasses::blue, x, y);
        }
      );
    }
  }

  return static_cast<double>(goodPoints) / static_cast<double>(sampleSize) > params.thresholdSanityCheck;
}


bool RedBallDetector::sanityCheck(const Vector2i& center, double radius) const
{
  size_t sampleSize = 21;
  double maxSquareSize = sqrt(2.0*radius*2.0*radius/2.0);
  int searchSize = int(maxSquareSize/2.0);
  int width = static_cast<int>(getImage().width());
  int height = static_cast<int>(getImage().height());

  int minX = Math::clamp(center.x - searchSize, 0, width-1);
  int maxX = Math::clamp(center.x + searchSize, 0, width-1);
  int minY = Math::clamp(center.y - searchSize, 0, height-1);
  int maxY = Math::clamp(center.y + searchSize, 0, height-1);

  size_t goodPoints = 0;
  Pixel pixel;
  for(size_t i = 0; i < sampleSize; i++)
  {
    int x = Math::random(minX, maxX);
    int y = Math::random(minY, maxY);
    getImage().get(x, y, pixel);
    if(isOrange(pixel))
    {
      goodPoints++;
    }
    DEBUG_REQUEST("Vision:RedBallDetector:draw_sanity_samples",
      if(isOrange(pixel)) {
        POINT_PX(ColorClasses::green, x, y);
      } else {
        POINT_PX(ColorClasses::blue, x, y);
      }
    );
  }

  return static_cast<double>(goodPoints) / static_cast<double>(sampleSize) > params.thresholdSanityCheck;
}
