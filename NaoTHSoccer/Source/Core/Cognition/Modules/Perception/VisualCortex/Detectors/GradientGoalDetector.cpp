
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
#include <algorithm>

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


GradientGoalDetector::GradientGoalDetector()
: 
  cameraID(CameraInfo::Bottom),
  features(params.numberOfScanlines),
  lastTestFeatureIdx(params.numberOfScanlines)
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
  Vector2d horizonDirection(1,0);

  if(horizon) {
    p1 = getArtificialHorizon().begin();
    p2 = getArtificialHorizon().end();
    horizonDirection = getArtificialHorizon().getDirection();
  }

  // orthogonal to the horizontal scan pointing towards the ground
  Vector2d horizonNormal(-horizonDirection.y, horizonDirection.x); 


  // sanity check
  if( p1.x != 0 || 
      p2.x != getImage().width()-1 ||
      p1.y < imageBorderOffset || p1.y > getImage().height() - imageBorderOffset ||
      p2.y < imageBorderOffset || p2.y > getImage().height() - imageBorderOffset
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


  // correct parameters
  //minimal 2  or more at all points have to be found
  if(params.minGoodPoints < 2 || params.numberOfScanlines < 2) {
    return;
  }
  if(params.minGoodPoints > params.numberOfScanlines) {
    params.minGoodPoints = params.numberOfScanlines;
  }

  GT_TRACE("GradientGoalDetector:initializeScan");
  
  // clamp the scanline
  p1.y = Math::clamp((int) p1.y, imageBorderOffset + 5, (int)getImage().height() - imageBorderOffset - 5);
  p2.y = Math::clamp((int) p2.y, imageBorderOffset + 5 , (int)getImage().height() - imageBorderOffset - 5);
  
  // adjust the vectors if the parameters change
  if((int) features.size() != params.numberOfScanlines)
  {
    features.resize(params.numberOfScanlines);
    lastTestFeatureIdx.resize(params.numberOfScanlines,0);
  }

  // clear the old features
  for(int i = 0; i < params.numberOfScanlines; i++) {
     features[i].clear();
  }


  GT_TRACE("GradientGoalDetector:scanForFeatures");
  //find feature that are candidates for goal posts along scanlines 
  findFeatureCandidates(horizonDirection, p1, params.thresholdUV, params.thresholdY);


  GT_TRACE("GradientGoalDetector:beginValidationOfFeatures");
  std::fill(lastTestFeatureIdx.begin(),lastTestFeatureIdx.end(),0);

  for(int scanLineId = 0; scanLineId < params.numberOfScanlines - (params.minGoodPoints - 1); scanLineId++)
  {
    for(size_t i = 0; i < features[scanLineId].size(); i++)
    {
      Feature& candidate = features[scanLineId][i];

      if(!candidate.used)
      {
        //check found features for goodness
        goodFeatures.clear();
        checkForGoodFeatures(horizonNormal, candidate, scanLineId, params.thresholdUV, params.thresholdY);

        GT_TRACE("GradientGoalDetector:getGoodFeatures");
        if(goodFeatures.size() >= (size_t) params.minGoodPoints)
        {
          Vector2d postScanDirection(horizonNormal);
          postScanDirection = fitLine(goodFeatures).getDirection();

          //scan along the post to find the foot point of it to be sure its on the field
          scanForFootPoints(postScanDirection, goodFeatures.back().center, params.thresholdUV, params.thresholdY);
        }
      }
    }//end for features[0].size()
  }
  GT_TRACE("GradientGoalDetector:setGoalPostPercept");


  // exactly two posts are seen => assign site labels
  if(getGoalPercept().getNumberOfSeenPosts() == 2) 
  {
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

  getGoalPercept().horizonScan = horizon;
}//end execute


void GradientGoalDetector::findFeatureCandidates(const Vector2d& scanDir, const Vector2d& p1, double threshold, double thresholdY)
{
  double response = 0.0;
  double responseY = 0.0;
  double lastResponse = 0.0;
  double lastResponseY = 0.0;

  Pixel pixel;

  bool isCandidate = false;
  bool isObstacle = false;
    
  Feature candidate;

  int diffVU = 0;
  int lastDiffVU = 0;

  int offset = params.numberOfScanlines * params.scanlinesDistance / 2 + 2;  // 6 / 2 = 3

  int start = (int) p1.y - offset;
  int stop = (int) p1.y + offset;

  if(start < 0)
  {
    start = 2;
  }

  if(stop > (int) getImage().height())
  {
    start = start - (stop - getImage().height() - 2);
  }
  int y = start;
  for(int aktIdx = 0; aktIdx < params.numberOfScanlines; aktIdx++)
  {
    valueBuffer.init();
    valueBufferY.init();
    pointBuffer.init();
    isCandidate = false;
    y += params.scanlinesDistance;
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
        response = valueBuffer[4] + 2 * valueBuffer[3] + 4 * valueBuffer[2] + valueBuffer[1] * 2 + valueBuffer[0];
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
        if(response >= threshold && fabs(responseY) >= thresholdY)
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
            if(isObstacle)
            {
              candidate.possibleObstacle = isObstacle;
              DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markPeaks", 
                LINE_PX(ColorClasses::orange, candidate.begin.x, candidate.begin.y, candidate.end.x, candidate.end.y);
              );
            }
            else
            {
              DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markPeaks", 
                LINE_PX(ColorClasses::blue, candidate.begin.x, candidate.begin.y, candidate.end.x, candidate.end.y);
              );
            }
          }
          isCandidate = false;
          isObstacle = false;
        }//end if
      }
    }
    while(scanner.getNextWithCheck(pos));
  }//end for
}//end findFeatureCandidates

void GradientGoalDetector::checkForGoodFeatures(const Vector2d& scanDir, Feature& candidate, int scanLineId, double threshold, double thresholdY)
{
  STOPWATCH_START("GradientGoalDetector:checkForGoodFeatures");
  double response = 0.0;
  double responseY = 0.0;

  candidate.used = true;
  goodFeatures.push_back(candidate);

  Pixel pixel;

  Vector2d scanDirection(scanDir);

  Vector2i pos(candidate.center);

  IMG_GET(pos.x, pos.y, pixel);
  int diffVU = (int) pixel.v - (int) pixel.u;
  for(int i = 0; i < 5; i++)
  {
    valueBuffer.add(diffVU);
    valueBufferY.add(pixel.y);
  }

  Vector2d last(pos);
  BresenhamLineScan goodFeatureScanner(pos, scanDirection, getImage().cameraInfo);

  double sumSquareError = 0.0;

  //scan 5 times (because 5 scanlines) 5 steps (because 5 pixel between 2 scanlines) along a normal 
  //of the horizon, to be able to validate features near that normal as good ones
  for(int y = scanLineId + 1; y < params.numberOfScanlines; y++)
  {
    bool stop = false;

    for(int yy = 0; !stop && yy < params.scanlinesDistance && goodFeatureScanner.pixelCount < goodFeatureScanner.numberOfPixels - 1; yy++)
    {
      goodFeatureScanner.getNext(pos);
      pointBuffer.add(pos);
      DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
      );
      IMG_GET(pos.x, pos.y, pixel);
      int diffVU = (int) pixel.v - (int) pixel.u;
      valueBuffer.add(diffVU);
      valueBufferY.add(pixel.y);

      response = valueBuffer[4] + 2 * valueBuffer[3]  + 4 * valueBuffer[2] + valueBuffer[1] * 2 + valueBuffer[0];
      responseY = valueBufferY[2];
      response /= 10;
      if(response < threshold || fabs(responseY) < thresholdY)
      {
        stop = true;
      }
    }

    //try to find matching features of next scanline
    for (unsigned j = lastTestFeatureIdx[y]; !stop && j < features[y].size(); j++)
    {
      if(!features[y][j].used)
      {
        int dist = (pos - features[y][j].center).abs();
        if(dist < params.maxFeatureDeviation)
        {
          features[y][j].used = true;
          goodFeatures.push_back(features[y][j]);
          Math::Line line = fitLine(goodFeatures);

          //get squared distance to new point from line
          Vector2d projection = line.projection(pos);
          double squareError = (Vector2d(pos) - projection).abs2();

          //if error does not raise leave the point to the good points list else drop it out
          if(squareError <= params.maxFootScanSquareError || sumSquareError == 0.0)
          {
            scanDirection = line.getDirection(); // 
            sumSquareError += squareError;
            lastTestFeatureIdx[y] = j;
            Vector2i newPos(projection); // cast Vector2d to Vector2i
          
            if(!getImage().isInside(newPos.x, newPos.y))
            {
              newPos = features[y][j].center;
            }
            goodFeatureScanner.setup(newPos, scanDirection, getImage().cameraInfo);
            pos = newPos;
                
            DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScanGoodPoints", 
              CIRCLE_PX(ColorClasses::black, features[y][j].center.x, features[y][j].center.y, 3);
              POINT_PX(ColorClasses::red, features[y][j].center.x, features[y][j].center.y);
            );
          }
          else
          {
            goodFeatures.pop_back();
          }
          stop = true;
        }
      }
    }
  }//end for
  STOPWATCH_STOP("GradientGoalDetector:checkForGoodFeatures");
}

Math::Line GradientGoalDetector::fitLine(const std::vector<Feature>& features) const
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
  cv::fitLine(cv::Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);

  Vector2d x(line[2], line[3]);
  Vector2d v(line[0], line[1]);
  
  // always point down in the image
  if(v.y < 0) {
    v *= -1.0;
  }

  return Math::Line(x, v);
}

void GradientGoalDetector::scanForFootPoints(const Vector2d& scanDir, Vector2i pos, double threshold, double thresholdY)
{      
  //NOTE: this is only for debug
  double lastResponse = 0.0;

  double response = 0.0;
  double responseY = 0.0;
  Pixel pixel;
  BresenhamLineScan footPointScanner(pos, scanDir, getImage().cameraInfo);

  while(footPointScanner.getNextWithCheck(pos))
  {
    IMG_GET(pos.x, pos.y, pixel);
    int diffVU = (int) pixel.v - (int) pixel.u;
    
    pointBuffer.add(pos);
    valueBuffer.add(diffVU);
    valueBufferY.add(pixel.y);

    if(pointBuffer.isFull())
    {
      response = valueBuffer[4] + 2 * valueBuffer[3]  + 4 * valueBuffer[2] + valueBuffer[1] * 2 + valueBuffer[0];
      responseY = valueBufferY[2];
      response /= 10;

      // end of the post found
      if(response < threshold || fabs(responseY) < thresholdY) {
        pos = pointBuffer[2];
        break;
      }

      DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::gray, pointBuffer[2].x, pointBuffer[2].y);
      );

      DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScanResponse", 
        LINE_PX(ColorClasses::white,  pointBuffer[3].x + (int)lastResponse,   pointBuffer[3].y, pointBuffer[4].x + (int)response,       pointBuffer[4].y);
        LINE_PX(ColorClasses::orange, pointBuffer[3].x + (int)valueBuffer[3], pointBuffer[3].y, pointBuffer[4].x + (int)valueBuffer[4], pointBuffer[4].y);
      );

      lastResponse = response;
    }
  }//end while


  //goalPostFound = true;
  GoalPercept::GoalPost post;
  post.basePoint = pos;
  getGoalPercept().add(post);


  // check some pixels below the foot point
  double greenPixelCount = 0;
  for(int k = 0; k < params.footGreenScanSize && footPointScanner.getNextWithCheck(pos); k++) {
    IMG_GET(pos.x, pos.y, pixel);
    greenPixelCount += getFieldColorPercept().isFieldColor(pixel);

    DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans", 
      ColorClasses::Color c = getFieldColorPercept().isFieldColor(pixel)?ColorClasses::green:ColorClasses::gray;
      POINT_PX(c, pos.x, pos.y);
    );
  }

  // 40% of the pixel below the post have to be green
  post.positionReliable = greenPixelCount > 0 && 
                          params.footGreenScanSize > 0 && 
                          greenPixelCount/params.footGreenScanSize > 0.4;

  post.positionReliable = post.positionReliable && CameraGeometry::imagePixelToFieldCoord(
    getCameraMatrix(),
    getImage().cameraInfo,
    post.basePoint.x, post.basePoint.y, 0.0,
    post.position);


  DEBUG_REQUEST("Vision:Detectors:GradientGoalDetector:markFootScans",
    ColorClasses::Color c = post.positionReliable?ColorClasses::yellowOrange:ColorClasses::red;
    CIRCLE_PX(c, (int) post.basePoint.x, (int) post.basePoint.y, 10);
  );
}

