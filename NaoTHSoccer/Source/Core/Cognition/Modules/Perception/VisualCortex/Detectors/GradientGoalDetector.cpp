
/**
* @file GradientGoalDetector.cpp
*
* Implementation of class GradientGoalDetector
*
*/

#include "GradientGoalDetector.h"

#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


GradientGoalDetector::GradientGoalDetector()
: 
  cameraID(CameraInfo::Bottom),
  features(numberOfScanlines),
  lastTestFeatureIdx(numberOfScanlines)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:draw_response","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:draw_difference","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:markFootScans","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:markFootScanResponse","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:markFootScanGoodPoints","..", false);   
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GradientGoalDetector:use_horizon","..", false);
}


void GradientGoalDetector::execute(CameraInfo::CameraID id, bool horizon)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getGoalPercept().reset();
	GT_TRACE("GradientGoalDetector:start");
  Vector2d p1(0                   , getImage().cameraInfo.getOpticalCenterY());
  Vector2d p2(getImage().width()-1, getImage().cameraInfo.getOpticalCenterY());
  Vector2d direction(1,0);

  if(horizon) {
    p1 = getArtificialHorizon().begin();
    p2 = getArtificialHorizon().end();
    direction = getArtificialHorizon().getDirection();
  }

  // sanity check
  if( p1.x != 0 || 
      p2.x != getImage().width()-1 ||
      p1.y < 5 || p1.y > getImage().height() - 5 ||
      p2.y < 5 || p2.y > getImage().height() - 5
      ) 
  {
    return;
  }
  
  int heightOfHorizon = (int) ((p1.y + p2.y) * 0.5 + 0.5);
  // image over the horizon
  if(heightOfHorizon > (int) getImage().height() - 10) 
  { 
    return;
  }

  GT_TRACE("GradientGoalDetector:initializeScan");
  
  // clamp the scanline
  p1.y = Math::clamp((int) p1.y, imageBorderOffset + 5, (int)getImage().height() - imageBorderOffset - 5);
  p2.y = Math::clamp((int) p2.y, imageBorderOffset + 5 , (int)getImage().height() - imageBorderOffset - 5);
  
  const double threshold = params.gradientThreshold;
  const double thresholdY = params.minY;

  // clear the features
  for(size_t i = 0; i < 5; i++) {
     features[i].clear();
  }

  GT_TRACE("GradientGoalDetector:scanForFeatures");
  if
  (
    p1.y > imageBorderOffset && 
    p2.y > imageBorderOffset && 
    p1.y < (int) getImage().height() - imageBorderOffset && 
    p2.y < (int) getImage().height() - imageBorderOffset
  )
  {
    //find feature that are candidates for goal posts along scanlines 
    findFeatureCandidates(direction, p1, threshold, thresholdY);
  }//end if


  GT_TRACE("GradientGoalDetector:beginValidationOfFeatures");
  std::fill(lastTestFeatureIdx.begin(),lastTestFeatureIdx.end(),0);
  //goalPosts.clear();

  Vector2d scanDirection(-direction.y, direction.x); // orthogonal to the horizontal scan

  for(size_t i = 0; i < features[0].size(); i++)
  {
    const Feature& candidate = features[0][i];

    //check found features for goodness
    const std::vector<Feature>& goodFeatures = checkForGoodFeatures(scanDirection, candidate, threshold, thresholdY);

    GT_TRACE("GradientGoalDetector:getGoodFeatures");
    if(goodFeatures.size() >= (size_t) params.minGoodPoints)
    {
      Vector2d postScanDirection(scanDirection);
      if(goodFeatures.size() > 1) {
        postScanDirection = findBestDownScanDirection(goodFeatures);
      }

      //scan along the post to find the foot point of it to be sure its on the field
      scanForFootPoints(postScanDirection, goodFeatures.back().center, threshold, thresholdY, horizon);
    }//end if

    //mark if candites is post or other possible obstacle
    DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScanGoodPoints", 
      for(size_t i = 0; i < goodFeatures.size(); i++) {
        const Feature& feature = goodFeatures[i];
        if(feature.possibleObstacle) {
          POINT_PX(ColorClasses::pink, feature.center.x, feature.center.y);
        } else {
          POINT_PX(ColorClasses::red, feature.center.x, feature.center.y);
        }
      }
    );

  }//end for features[0].size()
  GT_TRACE("GradientGoalDetector:setGoalPostPercept");

  // exactly two posts are seen => assign site labels
  if(getGoalPercept().getNumberOfSeenPosts() == 2) {
    GoalPercept::GoalPost& postOne = getGoalPercept().getPost(0);
    GoalPercept::GoalPost& postTwo = getGoalPercept().getPost(1);
    
    // sort: which one is left or right
    if(postOne.basePoint.x > postTwo.basePoint.x)
    {
      postOne.type = GoalPercept::GoalPost::rightPost;
      postTwo.type = GoalPercept::GoalPost::leftPost;
    } else {
      postOne.type = GoalPercept::GoalPost::leftPost;
      postTwo.type = GoalPercept::GoalPost::rightPost;
    }
  }
}//end execute

void GradientGoalDetector::findFeatureCandidates(const Vector2d& scanDir, const Vector2d& p1, double threshold, double thresholdY)
{
  double response = 0.0;
  double responseY = 0.0;
  double lastResponse = 0.0;
  double lastResponseY = 0.0;

  Pixel pixel;

  int aktIdx = 0;

  bool isCandidate = false;
  bool isObstacle = false;
    
  Feature candidate;

  int diffVU = 0;
  int lastDiffVU = 0;

  for(int y = (int) p1.y - 12; y <= (int) p1.y + 12; y += 6)
  {
    valueBuffer.init();
    valueBufferY.init();
    pointBuffer.init();
    isCandidate = false;
    Vector2i pos((int) p1.x + 2, y);
    BresenhamLineScan scanner(pos, scanDir, getImage().cameraInfo);
    do
    {
      pointBuffer.add(pos);
      IMG_GET(pos.x, pos.y, pixel);
      diffVU = (int) pixel.v - (int) pixel.u;
      valueBuffer.add(diffVU);
      valueBufferY.add(pixel.y);

      if(pointBuffer.isFull())
      {
        response = valueBuffer[4] + 2 * valueBuffer[3]  + 4 * valueBuffer[2] + valueBuffer[1] * 2 + valueBuffer[0];
        //response = valueBuffer[4] + 2 * valueBuffer[3]  - valueBuffer[1] * 2 - valueBuffer[0];
        responseY = valueBufferY[2];
        response /= 10;
        DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:draw_scanlines",
          if(aktIdx == 2)
          {
            POINT_PX(ColorClasses::yellow, pos.x, pos.y );
          }
          else
          {
            POINT_PX(ColorClasses::gray, pos.x, pos.y );
          }
        );
        DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:draw_difference",
          if(aktIdx == 2)
          {
            LINE_PX(ColorClasses::green, (int) pointBuffer[3].x, (int) pointBuffer[3].y - lastDiffVU, (int) pointBuffer[4].x, (int) pointBuffer[4].y - diffVU);
            lastDiffVU = diffVU;
          }
        );
        DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:draw_response",
          if(aktIdx == 2)
          {
            LINE_PX(ColorClasses::red, (int) pointBuffer[1].x, (int) pointBuffer[1].y - (int) /*fabs*/(lastResponse), (int) pointBuffer[2].x, (int) pointBuffer[2].y - (int)/* fabs*/(response) );
            LINE_PX(ColorClasses::white, (int) pointBuffer[1].x, (int) pointBuffer[1].y - (int) fabs(lastResponseY / 3), (int) pointBuffer[2].x, (int) pointBuffer[2].y - (int) fabs(responseY / 3) );
            lastResponse = response;
            lastResponseY = responseY;
          }
        );
        if(/*fabs*/(response) >= threshold && fabs(responseY) >= thresholdY)
        {
          if(!isCandidate)
          {
            candidate.begin = pointBuffer[2];
            candidate.center = pointBuffer[2];
            candidate.end = pointBuffer[2];
            candidate.responseAtBegin.x = response;
            candidate.responseAtBegin.y = diffVU;
            if(diffVU < 0)
            {
              isObstacle = true;
            }
          }
          isCandidate = true;
          if(isObstacle)
          {
            candidate.possibleObstacle = isObstacle;
            DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markPeaks", 
              POINT_PX(ColorClasses::orange, pos.x - 2, pos.y);
            );
          }
          else
          {
            DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markPeaks", 
              POINT_PX(ColorClasses::blue, pos.x - 2, pos.y);
            );
          }
        }
        else
        {
          if(isCandidate)
          {
            candidate.end = pointBuffer[2];
            candidate.center = (candidate.end + candidate.begin) / 2;
            candidate.responseAtEnd.x = response;
            candidate.responseAtEnd.y = diffVU;
            features[aktIdx].push_back(candidate);
          }
          isCandidate = false;
          isObstacle = false;
        }//end if
      }
    }
    while(scanner.getNextWithCheck(pos));
    aktIdx++;
  }//end for
}//end getFeatureResponses

std::vector<GradientGoalDetector::Feature> GradientGoalDetector::checkForGoodFeatures(const Vector2d& scanDir, const Feature& candidate, double threshold, double thresholdY)
{
  double response = 0.0;
  double responseY = 0.0;

  std::vector<Feature> goodFeatures;
  goodFeatures.push_back(candidate);

  Pixel pixel;

  Vector2d scanDirection(scanDir);

  Vector2i pos = candidate.center;
  valueBuffer.init();
  valueBufferY.init();
  pointBuffer.init();
    
  Vector2d last(pos);
  //scan 5 times (because 5 scanlines) 5 steps (because 5 pixel between 2 scanlines) along a normal 
  //of the horizon, to be able to validate features near that normal as good ones
  for(size_t y = 1; y < features.size(); y++)
  {
    bool stop = false;
    BresenhamLineScan goodFeatureScanner(candidate.center, scanDirection, getImage().cameraInfo);
    for(int yy = 0; yy < 5; yy++)
    {
      if(goodFeatureScanner.getNextWithCheck(pos))
      {
        DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
          POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
        );
        pointBuffer.add(pos);
        IMG_GET(pos.x, pos.y, pixel);
        int diffVU = (int) pixel.v - (int) pixel.u;
        valueBuffer.add(diffVU);
        valueBufferY.add(pixel.y);

        if(yy > 0)
        {
          response = valueBuffer[4] + 2 * valueBuffer[3]  + 4 * valueBuffer[2] + valueBuffer[1] * 2 + valueBuffer[0];
          //response = valueBuffer[4] + 2 * valueBuffer[3]  - valueBuffer[1] * 2 - valueBuffer[0];
          responseY = valueBufferY[2];
          response /= 10;
          if(/*fabs*/(response) < params.responseHoldFactor * threshold || fabs(responseY) < params.responseHoldFactor * thresholdY)
          {
            stop = true;
          }
        }
      }
    }
    unsigned j = lastTestFeatureIdx[y];
    while (!stop && j < features[y].size())
    {
      int dist = (pos - features[y][j].center).abs();
      if(dist < params.dist)
      {
        lastTestFeatureIdx[y] = j;
        goodFeatures.push_back(features[y][j]);
        pos = features[y][j].center;
        scanDirection = findBestDownScanDirection(goodFeatures);
        stop = true;
          //std::cout << "good (" << i << ") " << dist << " pos: " << pos << " point:" << features[y][j].center << std::endl;
      }
      j++;
    }
  }//end for
  return goodFeatures;
}

Vector2d GradientGoalDetector::findBestDownScanDirection(const std::vector<Feature>& features)
{
  ASSERT(features.size() >= 2);

  // prepare the points for opencv
  std::vector<cv::Point2f> points(features.size());
  for(size_t j = 0; j < features.size(); j++) {
    points[j].x = 0.5f*((float)features[j].begin.x + (float)features[j].end.x);
    points[j].y = 0.5f*((float)features[j].begin.y + (float)features[j].end.y);
  }
  
  // estimate the line
  cv::Vec4f line;
  fitLine(cv::Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);

  Vector2d v(line[0], line[1]);
  //Vector2d x(line[2], line[3]);

  // always point upwards in image
  return (v.y < 0) ? v*-1.0 : v;
}


void GradientGoalDetector::scanForFootPoints(const Vector2d& scanDir, Vector2i pos, double threshold, double thresholdY, bool horizon)
{      
  double response = 0.0;
  double responseY = 0.0;
  double lastResponse = 0.0;

  Pixel pixel;

  bool footPointFound = false;
  bool stop = false;
  Pixel pixel1;
  Pixel pixel2;
  Pixel pixel3;

  int lastDiffVU = 0;

  BresenhamLineScan footPointScanner(pos, scanDir, getImage().cameraInfo);

  while(!stop && footPointScanner.getNextWithCheck(pos))
  {
    pointBuffer.add(pos);
    IMG_GET(pos.x, pos.y, pixel);
    int diffVU = (int) pixel.v - (int) pixel.u;
    valueBuffer.add(diffVU);
    valueBufferY.add(pixel.y);

    response = valueBuffer[4] + 2 * valueBuffer[3]  + 4 * valueBuffer[2] + valueBuffer[1] * 2 + valueBuffer[0];
    //response = valueBuffer[4] + 2 * valueBuffer[3]  - valueBuffer[1] * 2 - valueBuffer[0];
    responseY = valueBufferY[2];
    response /= 10;

    DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
      if(getFieldColorPercept().isFieldColor(pixel))
      {
          POINT_PX(ColorClasses::green, (int) pointBuffer[2].x, (int) pointBuffer[2].y);
      }
      else
      {
        POINT_PX(ColorClasses::gray, (int) pointBuffer[2].x, (int) pointBuffer[2].y);
      }
    );
    DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScanResponse", 
      LINE_PX(ColorClasses::white, (int) pointBuffer[1].x + (int) lastResponse, (int) pointBuffer[1].y, (int) pointBuffer[2].x + (int) response, (int) pointBuffer[2].y);
      LINE_PX(ColorClasses::orange, (int) pointBuffer[1].x + lastDiffVU, (int) pointBuffer[1].y, (int) pointBuffer[2].x + diffVU, (int) pointBuffer[2].y);
    );
    lastDiffVU = diffVU;
    lastResponse = (int) response;
 
    IMG_GET((int) pointBuffer[4].x, (int) pointBuffer[4].y, pixel1);
    IMG_GET((int) pointBuffer[3].x, (int) pointBuffer[3].y, pixel2);
    IMG_GET((int) pointBuffer[2].x, (int) pointBuffer[2].y, pixel3);
    if(/*fabs*/(response) < params.responseHoldFactor * threshold || fabs(responseY) < params.responseHoldFactor * thresholdY)
    {
      stop = true;
      if
      (
        //c > params.minScanPointsAfterGoodPoints && 
        (getFieldColorPercept().isFieldColor(pixel1) || getFieldColorPercept().isFieldColor(pixel2) || getFieldColorPercept().isFieldColor(pixel3))
      )
      {
        footPointFound = true;
      }
      else
      {
        int t = 0;
        int count = 0;
        while( t < 10 && footPointScanner.getNextWithCheck(pos))
        {
          IMG_GET(pos.x, pos.y, pixel);
          if(getFieldColorPercept().isFieldColor(pixel))
          {
            count++;
          }
          DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
            if(getFieldColorPercept().isFieldColor(pixel))
            {
                POINT_PX(ColorClasses::green, pos.x, pos.y);
            }
            else
            {
              POINT_PX(ColorClasses::black, pos.x, pos.y);
            }
          );
          t++;
        }
        if(count > 4)
        {
          footPointFound = true;
        }
      }
    }
  }//end while

  GT_TRACE("GradientGoalDetector:5");
  if(footPointFound)
  {
    //goalPostFound = true;
    GoalPercept::GoalPost post;

    post.basePoint = Vector2i(pointBuffer[2]);
    CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(),
      getImage().cameraInfo,
      post.basePoint.x, post.basePoint.y, 0.0,
      post.position);

    post.positionReliable = true;
    //goalPosts.push_back(post);
    getGoalPercept().add(post);
    getGoalPercept().horizonScan = horizon;

    DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
      CIRCLE_PX(ColorClasses::yellowOrange, (int) pointBuffer[2].x, (int) pointBuffer[2].y, 10);
    );
  }
  else
  {
    DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
      CIRCLE_PX(ColorClasses::red, (int) pointBuffer[2].x, (int) pointBuffer[2].y, 10);
    );
  }
}

