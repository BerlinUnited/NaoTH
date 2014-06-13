
/**
* @file NewGoalDetector.cpp
*
* Implementation of class NewGoalDetector
*
*/

#include "NewGoalDetector.h"

#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugModify.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include <algorithm>

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


NewGoalDetector::NewGoalDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NewGoalDetector:markFootScans","..", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NewGoalDetector:markTopScans", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NewGoalDetector:markFootScanResponse","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NewGoalDetector:markFootScanGoodPoints","..", false);   
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NewGoalDetector:showColorByHistogramBottom","..", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NewGoalDetector:showColorByHistogramTop","..", false);
}


bool NewGoalDetector::execute(CameraInfo::CameraID id, bool horizon)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getGoalPercept().reset();

	GT_TRACE("NewGoalDetector:start");
  // correct parameters
  //minimal 2  or more at all points have to be found
  if(params.minGoodPoints < 2 || getGoalFeaturePercept().features.size() < 2) {
    return false;
  }
  if(params.minGoodPoints > (int) getGoalFeaturePercept().features.size()) {
    params.minGoodPoints = (int) getGoalFeaturePercept().features.size();
  }

  GT_TRACE("NewGoalDetector:initializeScan");
  
  // adjust the vectors if the parameters change
  if((int)features.size() != getGoalFeaturePercept().features.size())
  {
    features.resize(getGoalFeaturePercept().features.size());
    lastTestFeatureIdx.resize(getGoalFeaturePercept().features.size(), 0);
  }  
  
  for(size_t scanLineId = 0; scanLineId < getGoalFeaturePercept().features.size() - (params.minGoodPoints - 1); scanLineId++)
  {
    features[scanLineId].clear();
    for(size_t i = 0; i <  getGoalFeaturePercept().features[scanLineId].size(); i++)
    {
      const GoalFeaturePercept::Feature& feature = getGoalFeaturePercept().features[scanLineId][i];
      ExtFeature extFeature(feature);
      features[scanLineId].push_back(extFeature);
    }
  }


  GT_TRACE("NewGoalDetector:beginValidationOfFeatures");
  std::fill(lastTestFeatureIdx.begin(),lastTestFeatureIdx.end(),0);

  int lastSeenPostIdx = -1;

  for(size_t scanLineId = 0; scanLineId < features.size() - (params.minGoodPoints - 1); scanLineId++)
  {
    for(size_t i = 0; i <  features[scanLineId].size(); i++)
    {
      ExtFeature& candidate = features[scanLineId][i];

      if(!candidate.used)
      {
        //check found features for goodness
        goodFeatures.clear();
        checkForGoodFeatures( getGoalFeaturePercept().horizonNormal, candidate, scanLineId, params.thresholdUV, params.thresholdY);

        GT_TRACE("NewGoalDetector:getGoodFeatures");
        if(goodFeatures.size() >= (size_t) params.minGoodPoints)
        {
          Vector2d postScanDirection( getGoalFeaturePercept().horizonNormal);
          postScanDirection = fitLine(goodFeatures).getDirection();

          //scan along the post to find the foot point of it to be sure its on the field
          scanForFootPoints(postScanDirection, goodFeatures.back().center, params.thresholdUV, params.thresholdY);
          int lastPostIdxInList = getGoalPercept().getNumberOfSeenPosts() - 1;
          if
          (
            getGoalPercept().getNumberOfSeenPosts() > 0 && //any post in percept?
            lastSeenPostIdx != lastPostIdxInList && //is last index of posts in percept not same as before? a new one?
            getGoalPercept().getPost(lastPostIdxInList).positionReliable //last post in percept reliable?
          )
          {
            scanForTopPoints(getGoalPercept().getPost(lastPostIdxInList), 
              goodFeatures.front().center, params.thresholdUV, params.thresholdY);
            scanForStatisticsToFootPoint(getGoalPercept().getPost(lastPostIdxInList).basePoint,
              goodFeatures.front().center);
            lastSeenPostIdx = lastPostIdxInList; //save actually used post index
          }
        }
      }
    }//end for features[0].size()
  }
  GT_TRACE("NewGoalDetector:setGoalPostPercept");

  if(getGoalPercept().getNumberOfSeenPosts() > 0)
  {
    //calculate histogram only, if a goalpost was found
    getGoalPostHistograms().calculate();
    getGoalPostHistograms().setBorders(params.colorRegionDeviation);
  }

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
  return true;
}//end execute


void NewGoalDetector::debugStuff(CameraInfo::CameraID camID)
{
  CANVAS_PX(camID);
  cameraID = camID;
  DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:showColorByHistogramBottom",
    for(unsigned int x = 0; x < getImage().width(); x+=2)
    {
      for(unsigned int y = 0; y < getImage().height(); y+=2)
      {
        const Pixel& pixel = getImage().get(x, y);
        if(getGoalPostHistograms().isPostColor(pixel))
        {
          POINT_PX(ColorClasses::pink, x, y);
        }
      }
    }
  );
  DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:showColorByHistogramTop",
    for(unsigned int x = 0; x < getImage().width(); x+=2)
    {
      for(unsigned int y = 0; y < getImage().height(); y+=2)
      {
        const Pixel& pixel = getImage().get(x, y);
        if(getGoalPostHistograms().isPostColor(pixel))
        {
          POINT_PX(ColorClasses::pink, x, y);
        }
      }
    }
  );
}


void NewGoalDetector::clusterEdgelFeatures()
{
  double t_sim = 0.5;
  MODIFY("t_sim",t_sim);
  
  std::vector<EdgelT<double> > pairs;

  for(size_t scanIdOne = 0; scanIdOne + 1 < getGoalFeaturePercept().edgel_features.size(); scanIdOne++) {
    for(size_t i = 0; i < getGoalFeaturePercept().edgel_features[scanIdOne].size(); i++) {
      const EdgelT<double>& e1 = getGoalFeaturePercept().edgel_features[scanIdOne][i];

      size_t scanIdTwo = scanIdOne+1;
      //for(size_t scanIdTwo = scanIdOne+1; scanIdTwo < features.size(); scanIdTwo++) {
        for(size_t j = 0; j < getGoalFeaturePercept().edgel_features[scanIdTwo].size(); j++) {
          const EdgelT<double>& e2 = getGoalFeaturePercept().edgel_features[scanIdTwo][j];

          if(e1.sim(e2) > t_sim) {
            //LINE_PX(ColorClasses::red, (int)(e1.point.x+0.5), (int)(e1.point.y+0.5), (int)(e2.point.x+0.5), (int)(e2.point.y+0.5));

            EdgelT<double> pair;
            pair.point = (e1.point + e2.point)*0.5;
            pair.direction = (e2.direction + e1.direction).normalize();
            pairs.push_back(pair);
          }
        }
      //}
    }
  }


  std::vector<Cluster> clusters;

  for(size_t i = 0; i < pairs.size(); i++) {
    const EdgelT<double>& e1 = pairs[i];

    size_t cluster_idx = 0;
    double max_sim = -1;

    for(size_t j = 0; j < clusters.size(); j++) {
      Cluster& cluster = clusters[j];
      double s = cluster.sim(e1);
      if(s > max_sim) {
        max_sim = s;
        cluster_idx = j;
      }
    }

    if(max_sim > t_sim) {
      clusters[cluster_idx].add(e1);
    } else {
      Cluster cluster;
      cluster.add(e1);
      clusters.push_back(cluster);
    }
  }

  for(size_t j = 0; j < clusters.size(); j++) {
      Cluster& cluster = clusters[j];
      
      if(cluster.size() > 2) {
        Math::Line line = cluster.getLine();

        Vector2d begin, end;
        begin = line.getBase();
        end = line.getBase() + line.getDirection()*50;
        LINE_PX(ColorClasses::red, (int)(begin.x+0.5), (int)(begin.y+0.5), (int)(end.x+0.5), (int)(end.y+0.5));
      }
  }
}


void NewGoalDetector::checkForGoodFeatures(const Vector2d& scanDir, ExtFeature& candidate, int scanLineId, double threshold, double thresholdY)
{
  STOPWATCH_START("NewGoalDetector:checkForGoodFeatures");
  double response = 0.0;
  double responseY = 0.0;

  candidate.used = true;
  goodFeatures.push_back(candidate);

  Pixel pixel;

  Vector2d scanDirection(scanDir);
  Vector2i pos(candidate.center);

  double meanSquareFeatureWidth = (candidate.end - candidate.begin).abs2();
  double sumSquareFeatureWidth = meanSquareFeatureWidth;

  IMG_GET(pos.x, pos.y, pixel);
  int diffVU = (int) pixel.v - (int) pixel.u;
  for(int i = 0; i < 5; i++)
  {
    valueBuffer.add(diffVU);
    valueBufferY.add(pixel.y);
  }

  Vector2d last(pos);
  BresenhamLineScan goodFeatureScanner(pos, scanDirection, getImage().cameraInfo);
  Statistics::Histogram<256> histU;
  Statistics::Histogram<256> histV;

  double sumSquareError = 0.0;

  //scan 5 times (because 5 scanlines) 5 steps (because 5 pixel between 2 scanlines) along a normal 
  //of the horizon, to be able to validate features near that normal as good ones
  for(size_t y = scanLineId + 1; y < features.size(); y++)
  {
    bool stop = false;

    for(size_t yy = 0; !stop && yy < features.size() && goodFeatureScanner.pixelCount < goodFeatureScanner.numberOfPixels - 1; yy++)
    {
      goodFeatureScanner.getNext(pos);
      pointBuffer.add(pos);
      DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::skyblue, pos.x, pos.y);
      );
      IMG_GET(pos.x, pos.y, pixel);
      int diffVU = (int) pixel.v - (int) pixel.u;
      valueBuffer.add(diffVU);
      valueBufferY.add(pixel.y);

      histU.add(pixel.u);
      histV.add(pixel.v);

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
          double featureWidth = (features[y][j].end - features[y][j].begin).abs2();
          double squareFeatureWidthError = fabs(featureWidth - meanSquareFeatureWidth);

          features[y][j].width = featureWidth;
          features[y][j].used = true;
          goodFeatures.push_back(features[y][j]);
          Math::Line line = fitLine(goodFeatures);

          //get squared distance to new point from line
          Vector2d projection = line.projection(pos);
          double squareError = (Vector2d(pos) - projection).abs2();

          //if error in width is to big, drop the point of the good points list
          if(!params.enableFeatureWidthCheck || squareFeatureWidthError <= params.maxFeatureWidthError * meanSquareFeatureWidth)
          {
            sumSquareFeatureWidth += featureWidth;
            meanSquareFeatureWidth = sumSquareFeatureWidth / (double) goodFeatures.size();
            
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
                
              DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markFootScanGoodPoints", 
                CIRCLE_PX(ColorClasses::black, features[y][j].center.x, features[y][j].center.y, 3);
                POINT_PX(ColorClasses::red, features[y][j].center.x, features[y][j].center.y);
              );
            }
            else
            {
              goodFeatures.pop_back();
            }//end if
          }
          else
          {
            features[y][j].used = false;
            goodFeatures.pop_back();
          }//end if

          stop = true;
        }//end if
      }//end if
    }//end for
  }//end for

  if(goodFeatures.size() >= (size_t) params.minGoodPoints)
  {
    for(int i = 0; i < getGoalPostHistograms().VALUE_COUNT; i++) 
    {
      getGoalPostHistograms().histogramU.add(i, histU.rawData[i]);
      getGoalPostHistograms().histogramV.add(i, histV.rawData[i]);
    }
  }
  STOPWATCH_STOP("NewGoalDetector:checkForGoodFeatures");
}

Math::Line NewGoalDetector::fitLine(const std::vector<ExtFeature>& features) const
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

void NewGoalDetector::scanForFootPoints(const Vector2d& scanDir, Vector2i pos, double threshold, double thresholdY)
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

      DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markFootScans", 
        POINT_PX(ColorClasses::gray, pointBuffer[2].x, pointBuffer[2].y);
      );

      DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markFootScanResponse", 
        LINE_PX(ColorClasses::white,  pointBuffer[3].x + (int)lastResponse,   pointBuffer[3].y, pointBuffer[4].x + (int)response,       pointBuffer[4].y);
        LINE_PX(ColorClasses::orange, pointBuffer[3].x + (int)valueBuffer[3], pointBuffer[3].y, pointBuffer[4].x + (int)valueBuffer[4], pointBuffer[4].y);
      );

      lastResponse = response;
    }
  }//end while

  // create a new goal post
  GoalPercept::GoalPost post;
  post.basePoint = pos;
  post.positionReliable = true;

  Vector2i lastGreenPoint(post.basePoint);
  if(params.enableGreenCheck && params.footGreenScanSize > 0)
  {
    // check some pixels below the foot point
    BresenhamLineScan footPointGreenScanner(post.basePoint, scanDir, getImage().cameraInfo);
    double greenPixelCount = 0;
    for(int k = 0; k < params.footGreenScanSize && footPointGreenScanner.getNextWithCheck(pos); k++) {
      IMG_GET(pos.x, pos.y, pixel);

      if(getFieldColorPercept().isFieldColor(pixel)) {
        greenPixelCount++;
        lastGreenPoint = pos;
      }

      DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markFootScans", 
        ColorClasses::Color c = getFieldColorPercept().isFieldColor(pixel)?ColorClasses::green:ColorClasses::gray;
        POINT_PX(c, pos.x, pos.y);
      );
    }

    // 40% of the pixel below the post have to be green
    post.positionReliable = greenPixelCount > 0.4*params.footGreenScanSize;
  }
  else
  {
    lastGreenPoint.y = Math::clamp(lastGreenPoint.y + params.footGreenScanSize, 0, (int)getImage().height() - 1);
  }

  post.positionReliable = post.positionReliable &&
                         (getFieldPercept().getValidField().isInside(post.basePoint) || 
                          getFieldPercept().getValidField().isInside(lastGreenPoint) );

  // NOTE: if the projection is not successfull, then post.position = (0,0)
  CameraGeometry::imagePixelToFieldCoord(
    getCameraMatrix(),
    getImage().cameraInfo,
    post.basePoint.x, post.basePoint.y, 0.0,
    post.position);
  
  post.directionInImage = scanDir;

  if(post.positionReliable)
  {
    //calculate mean width of new found goal post
    double meanWidth = 0.0;
    for(size_t fIdx = 0; fIdx < goodFeatures.size(); fIdx++)
    {
      meanWidth += goodFeatures[fIdx].width;
    }
    meanWidth /= (double) goodFeatures.size();
    post.seenWidth = meanWidth;

    bool isDouble = false;
    if(params.enableFeatureWidthCheck)
    {
      //check if there is a post candidate found in the same post (double detections)
      for(int pIdx = 0; pIdx < getGoalPercept().getNumberOfSeenPosts(); pIdx++)
      {
        double precision = 1.0 - params.maxFeatureWidthError;
        double footPointDistance = precision * (post.basePoint - getGoalPercept().getPost(pIdx).basePoint).abs();

        if(meanWidth >= footPointDistance || getGoalPercept().getPost(pIdx).seenWidth >= footPointDistance)
        {
          post.positionReliable = false;
          isDouble = true;
          break;
        }
      }
    }

    if(params.enableFeatureWidthCheck || !isDouble)
    {
      getGoalPercept().add(post);
    }
  }

  DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markFootScans",
    ColorClasses::Color c = post.positionReliable?ColorClasses::yellowOrange:ColorClasses::red;
    CIRCLE_PX(c, (int) post.basePoint.x, (int) post.basePoint.y, 10);
  );
}

void NewGoalDetector::scanForTopPoints(GoalPercept::GoalPost& post, Vector2i pos, double threshold, double thresholdY)
{      
  //NOTE: this is only for debug (unused variable warning)
  //double lastResponse = 0.0;

  double response = 0.0;
  double responseY = 0.0;
  Pixel pixel;
  BresenhamLineScan footPointScanner(pos, -Vector2d(post.directionInImage), getImage().cameraInfo);

  // clear everything
  pointBuffer.clear();
  valueBuffer.init();
  valueBufferY.init();

  while(footPointScanner.getNextWithCheck(pos))
  {
    IMG_GET(pos.x, pos.y, pixel);
    int diffVU = (int) pixel.v - (int) pixel.u;
    
    pointBuffer.add(pos);
    valueBuffer.add(diffVU);
    valueBufferY.add(pixel.y);

    //collect some values for statisics of colors
    getGoalPostHistograms().increaseChannelValue(pixel);

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

      DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markTopScans", 
        POINT_PX(ColorClasses::pink, pointBuffer[2].x, pointBuffer[2].y);
      );

      //lastResponse = response;
    }
  }//end while

  post.topPoint = pos;

  DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markTopScans",
    CIRCLE_PX(ColorClasses::blue, (int) pos.x, (int) pos.y, 10);
  );
}

void NewGoalDetector::scanForStatisticsToFootPoint(Vector2i footPoint, Vector2i pos)
{
  Pixel pixel;
  BresenhamLineScan footPointScanner(pos, footPoint);
  while(footPointScanner.getNextWithCheck(pos))
  {
    IMG_GET(pos.x, pos.y, pixel);
    //collect some values for statisics of colors
    getGoalPostHistograms().increaseChannelValue(pixel);
  }
}

