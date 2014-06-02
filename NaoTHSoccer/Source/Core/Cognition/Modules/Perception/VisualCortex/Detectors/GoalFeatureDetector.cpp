
/**
* @file GoalFeatureDetector.cpp
*
* Implementation of class GoalFeatureDetector
*
*/

#include "GoalFeatureDetector.h"

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


GoalFeatureDetector::GoalFeatureDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:draw_response","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:GoalFeatureDetector:draw_difference","..", false);  
  getGoalFeaturePercept().reset(params.numberOfScanlines);
  getGoalFeaturePerceptTop().reset(params.numberOfScanlines);
}


bool GoalFeatureDetector::execute(CameraInfo::CameraID id, bool horizon)
{
  cameraID = id;
  CANVAS_PX(cameraID);

	GT_TRACE("GoalFeatureDetector:start");
  Vector2d p1(0                   , getImage().cameraInfo.getOpticalCenterY());
  Vector2d p2(getImage().width()-1, getImage().cameraInfo.getOpticalCenterY());
  Vector2d horizonDirection(1,0);

  if(!horizon && cameraID == CameraInfo::Bottom)
  {
    int offsetY = (params.numberOfScanlines * params.scanlinesDistance) / 2 + imageBorderOffset;

    p1.y = offsetY;
    p2.y = offsetY;
  }

  if(horizon) {
    p1 = getArtificialHorizon().begin();
    p2 = getArtificialHorizon().end();
    horizonDirection = getArtificialHorizon().getDirection();
  }

  // orthogonal to the horizontal scan pointing towards the ground
  getGoalFeaturePercept().horizonNormal.x = -horizonDirection.y;
  getGoalFeaturePercept().horizonNormal.y = horizonDirection.x;

  // sanity check
  if( p1.x != 0 || 
      p2.x != getImage().width()-1 ||
      p1.y < imageBorderOffset || p1.y > getImage().height() - imageBorderOffset ||
      p2.y < imageBorderOffset || p2.y > getImage().height() - imageBorderOffset
      ) 
  {
    return false;
  }
  
  int heightOfHorizon = (int) ((p1.y + p2.y) * 0.5 + 0.5);
  // image over the horizon
  if(heightOfHorizon > (int) getImage().height() - 10) 
  { 
    return false;
  }

  // correct parameters
  //minimal 2  or more at all points have to be found
  if(params.numberOfScanlines < 2) {
    return false;
  }

  GT_TRACE("GoalFeatureDetector:initializeScan");
  
  // clamp the scanline
  p1.y = Math::clamp((int) p1.y, imageBorderOffset + 5, (int)getImage().height() - imageBorderOffset - 5);
  p2.y = Math::clamp((int) p2.y, imageBorderOffset + 5 , (int)getImage().height() - imageBorderOffset - 5);
  
  // adjust the vectors if the parameters change
  if((int)getGoalFeaturePercept().features.size() != params.numberOfScanlines)
  {
    getGoalFeaturePercept().reset(params.numberOfScanlines);
  }

  // clear the old features
  for(int i = 0; i < params.numberOfScanlines; i++) {
     getGoalFeaturePercept().features[i].clear();
  }


  GT_TRACE("GoalFeatureDetector:scanForFeatures");
  //find feature that are candidates for goal posts along scanlines 
  findFeatureCandidates(horizonDirection, p1, params.thresholdUV, params.thresholdY);
  return true;
}//end execute

void GoalFeatureDetector::findFeatureCandidates(const Vector2d& scanDir, const Vector2d& p1, double threshold, double thresholdY)
{
  double response = 0.0;
  double responseY = 0.0;
  double lastResponse = 0.0;
  double lastResponseY = 0.0;

  Pixel pixel;

  bool isCandidate = false;
  bool isObstacle = false;
    
  GoalFeaturePercept::Feature candidate;

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
        DEBUG_REQUEST("Vision:Detectors:GoalFeatureDetector:draw_scanlines",
          if(aktIdx == 2)
          {
            POINT_PX(ColorClasses::yellow, pos.x, pos.y );
          }
          else
          {
            POINT_PX(ColorClasses::gray, pos.x, pos.y );
          }
        );
        DEBUG_REQUEST("Vision:Detectors:GoalFeatureDetector:draw_difference",
          if(aktIdx == 2)
          {
            LINE_PX(ColorClasses::green, (int) pointBuffer[3].x, (int) pointBuffer[3].y - lastDiffVU, (int) pointBuffer[4].x, (int) pointBuffer[4].y - diffVU);
            lastDiffVU = diffVU;
          }
        );
        DEBUG_REQUEST("Vision:Detectors:GoalFeatureDetector:draw_response",
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
            getGoalFeaturePercept().features[aktIdx].push_back(candidate);
            if(isObstacle)
            {
              candidate.possibleObstacle = isObstacle;
              DEBUG_REQUEST("Vision:Detectors:GoalFeatureDetector:markPeaks", 
                LINE_PX(ColorClasses::orange, candidate.begin.x, candidate.begin.y, candidate.end.x, candidate.end.y);
              );
            }
            else
            {
              DEBUG_REQUEST("Vision:Detectors:GoalFeatureDetector:markPeaks", 
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
