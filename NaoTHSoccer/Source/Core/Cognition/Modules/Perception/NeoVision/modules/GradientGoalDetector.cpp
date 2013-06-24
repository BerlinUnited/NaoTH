
/**
* @file GradientGoalDetector.cpp
*
* Implementation of class GradientGoalDetector
*
*/

#include "GradientGoalDetector.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/CameraGeometry.h"

#include "Tools/Debug/Stopwatch.h"

#include "Tools/ImageProcessing/BresenhamLineScan.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>


GradientGoalDetector::GradientGoalDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("NeoVision:GradientGoalDetector:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("NeoVision:GradientGoalDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:GradientGoalDetector:draw_response","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:GradientGoalDetector:draw_difference","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:GradientGoalDetector:markFootScans","..", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:GradientGoalDetector:markFootScanGoodPoints","..", false);  
}


void GradientGoalDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getGoalPercept().reset();

  Vector2<double> p1(getArtificialHorizon().begin());
  Vector2<double> p2(getArtificialHorizon().end());

  Vector2<double> direction = getArtificialHorizon().getDirection();
  
  //double horizont = min(p1.y, p2.y);

  double threshold = params.gradientThreshold;
  double thresholdY = params.minY;

  Pixel pixel;

  int aktIdx = 0;

  if(p1.y > 15 && p2.y > 15 && p1.y < (int) getImage().height() - 15 && p2.y < (int) getImage().height() - 15)
  {
    double response = 0.0;
    double responseY = 0.0;
    double lastResponse = 0.0;
    double lastResponseY = 0.0;
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
      Vector2<int> pos((int) p1.x + 2, y);
      BresenhamLineScan scanner(pos, direction, getImage().cameraInfo);
      features[aktIdx].clear();
      do
      {
        pointBuffer.add(pos);
        getImage().get(pos.x, pos.y, pixel);
        diffVU = (int) pixel.v - (int) pixel.u;
        valueBuffer.add(diffVU);
        valueBufferY.add(pixel.y);

        response = valueBuffer[4] + 2 * valueBuffer[3]  + 4 * valueBuffer[2] + valueBuffer[1] * 2 + valueBuffer[0];
        //response = valueBuffer[4] + 2 * valueBuffer[3]  - valueBuffer[1] * 2 - valueBuffer[0];
        responseY = valueBufferY[2];
        response /= 10;
        DEBUG_REQUEST("NeoVision:GradientGoalDetector:draw_scanlines",
          if(aktIdx == 2)
          {
            POINT_PX(ColorClasses::yellow, pos.x, pos.y );
          }
          else
          {
            POINT_PX(ColorClasses::gray, pos.x, pos.y );
          }
        );
        DEBUG_REQUEST("NeoVision:GradientGoalDetector:draw_difference",
          if(aktIdx == 2)
          {
            LINE_PX(ColorClasses::green, pos.x - 1, pos.y - lastDiffVU, pos.x, pos.y - diffVU);
            lastDiffVU = diffVU;
          }
        );
        DEBUG_REQUEST("NeoVision:GradientGoalDetector:draw_response",
          if(aktIdx == 2)
          {
            LINE_PX(ColorClasses::red, pos.x - 3, pos.y - (int) /*fabs*/(lastResponse), pos.x - 2, pos.y - (int)/* fabs*/(response) );
            LINE_PX(ColorClasses::white, pos.x - 3, pos.y - (int) fabs(lastResponseY / 3), pos.x - 2, pos.y - (int) fabs(responseY / 3) );
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
            DEBUG_REQUEST("NeoVision:GradientGoalDetector:markPeaks", 
              POINT_PX(ColorClasses::orange, pos.x - 2, pos.y);
            );
          }
          else
          {
            DEBUG_REQUEST("NeoVision:GradientGoalDetector:markPeaks", 
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
        scanner.getNext(pos);
      }
      while(pos.x < (int) getImage().width() - 2);
      aktIdx++;
    }//end for
  }//end if

  Vector2<double> dir(-direction.y, direction.x);

  memset(&lastTestFeatureIdx, 0, sizeof(lastTestFeatureIdx));
  bool goalPostFound = false;
  //std::cout << std::endl << " ------ " << std::endl;
  for(unsigned i = 0; i < features[0].size(); i++)
  {
    Feature& candidate = features[0][i];

    BresenhamLineScan footPointScanner(candidate.center, dir, getImage().cameraInfo);

    int goodPointsCount = 0;

    Vector2<int> pos = candidate.center;
    for(int y = 1; y < 5; y++)
    {
      footPointScanner.getNext(pos);
      DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
      );
      footPointScanner.getNext(pos);
      DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
      );
      footPointScanner.getNext(pos);
      DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
      );
      footPointScanner.getNext(pos);
      DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
      );
      footPointScanner.getNext(pos);
      DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
      );
      unsigned j = lastTestFeatureIdx[y];
      bool stop = false;
      while (!stop && j < features[y].size())
      {
        int dist = (pos - features[y][j].center).abs();

        if(dist < params.dist)
        {
          DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScanGoodPoints", 
            POINT_PX(ColorClasses::red, features[y][j].center.x, features[y][j].center.y);
          );
          lastTestFeatureIdx[y] = j;
          goodPointsCount++;
          stop = true;
          //std::cout << "good (" << i << ") " << dist << " pos: " << pos << " point:" << features[y][j].center << std::endl;
        }
        j++;
      }

    }

    if(goodPointsCount >= 3)
    {
      bool footPointFound = false;
      int c = 0;
      Pixel pixel;
      while(!footPointFound && footPointScanner.getNextWithCheck(pos))
      {
        DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScans", 
          POINT_PX(ColorClasses::white, pos.x, pos.y);
        );
        getImage().get(pos.x, pos.y, pixel);
        if(c % 2 == 0 && getFieldColorPercept().isFieldColor(pixel))
        {
          footPointFound = true;
          DEBUG_REQUEST("NeoVision:GradientGoalDetector:markFootScans", 
            CIRCLE_PX(ColorClasses::yellowOrange, pos.x, pos.y, 10);
          );
        }
        c++;
      }
      if (footPointFound)
      {
        goalPostFound = true;


      }


    }


    if(candidate.possibleObstacle)
    {
      DEBUG_REQUEST("NeoVision:GradientGoalDetector:markPeaks", 
        POINT_PX(ColorClasses::pink, candidate.center.x, candidate.center.y);
      );
    }
    else
    {
      DEBUG_REQUEST("NeoVision:GradientGoalDetector:markPeaks", 
        POINT_PX(ColorClasses::red, candidate.center.x, candidate.center.y);
      );
    }


  }
  if(goalPostFound)
  {
    //getGoalPercept().
  }

}//end execute

