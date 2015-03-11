
/**
* @file GoalCrossBarDetector.cpp
*
* Implementation of class GoalCrossBarDetector
*
*/

#include "GoalCrossBarDetector.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/Math/Geometry.h"

#define POST_HEIGHT 80.0
#define CROSSBAR_LENGTH 150.0
#define BAR_WIDTH 10.0


GoalCrossBarDetector::GoalCrossBarDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:draw_scanlines", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:markGradients", "mark found gradients in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:showTestBackProjection", "mark found gradients in image", false);

  getDebugParameterList().add(&parameters);
}

GoalCrossBarDetector::~GoalCrossBarDetector()
{
  getDebugParameterList().remove(&parameters);
}

bool GoalCrossBarDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  DEBUG_REQUEST("Vision:GoalCrossBarDetector:showTestBackProjection",
    testBackProjection();
  );

  for(size_t i = 0; i < features.size(); i++)
  {
    features[i].clear();
  }
  positiveTestFeatures.clear();

  if(getGoalPercept().getNumberOfSeenPosts() == 1)
  {
    checkSinglePost(getGoalPercept().getPost(0));
  }
  else if(getGoalPercept().getNumberOfSeenPosts() == 2)
  {
    GoalPercept::GoalPost post0 = getGoalPercept().getPost(0);
    GoalPercept::GoalPost post1 = getGoalPercept().getPost(1);
    if(checkProjectedPostDistance(post0, post1))
    {    
      //scan from post most left in image
      if(post1.topPoint.x < post0.topPoint.x)
      {
        checkBothPosts(post1, post0);
      }
      else
      {
        checkBothPosts(post0, post1);
      }
    }
    else
    {
      checkSinglePost(getGoalPercept().getPost(0));
      checkSinglePost(getGoalPercept().getPost(1));
    }

  } 
  else if(getGoalPercept().getNumberOfSeenPosts() > 2)
  {

  }


  return true;
}//end execute

void GoalCrossBarDetector::checkSinglePost(const GoalPercept::GoalPost& post)
{
  Vector2i start;
  Vector2d direction;
  bool barFound = estimateCrossBarDirection(post, start, direction);
  if(barFound)
  {
    //estimate crossbar length by rate between crossbar length and bar width of the real goal
    double relWidth = CROSSBAR_LENGTH / BAR_WIDTH * post.seenWidth;
    //estimate crossbar length by rate between crossbar length and goal height of the real goal
    double relHeight = CROSSBAR_LENGTH / POST_HEIGHT * post.seenHeight;
    //take the mean of them as estimated length
    double estimatedCrossBarLength = (relWidth + relHeight) * 0.5;

    Vector2d offset =  direction * post.seenWidth * 0.5;
    Vector2i end = start + direction * estimatedCrossBarLength;
    scanAlongCrossBar(start, end, direction.rotateRight(),  post.seenWidth);
  }
}

void GoalCrossBarDetector::checkBothPosts(const GoalPercept::GoalPost& postLeft, const GoalPercept::GoalPost& postRight)
{
    Vector2i estimatedCrossBar;

    Vector2d meanPostDirection = (postLeft.directionInImage + postRight.directionInImage).normalize(); 
    double meanPostWidth =  0.5 * (postLeft.seenWidth + postRight.seenWidth);
    double meanprojWidth =  0.5 * (getBackProjectedTopWidth(postLeft) + getBackProjectedTopWidth(postLeft));

    Vector2d offset =  meanPostDirection * meanPostWidth * 0.5;

    Vector2i start = getBackProjectedTopPoint(postLeft);;
    Vector2i end = getBackProjectedTopPoint(postRight);;

    scanAlongCrossBar(start, end, meanPostDirection, meanPostWidth);
}



bool GoalCrossBarDetector::checkProjectedPostDistance(const GoalPercept::GoalPost& post0, const GoalPercept::GoalPost& post1)
{
  Vector2d p0 = post0.position;
  Vector2d p1 = post1.position;

  return fabs((p1 - p0).abs() - getFieldInfo().goalWidth) / getFieldInfo().goalWidth <= 0.2;
}

Vector2d GoalCrossBarDetector::getBackProjectedTopPoint(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  Vector3d pt1(p1.x, p1.y, getFieldInfo().goalHeight);

  Vector2i topInImage1;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 
  return topInImage1;
}

int GoalCrossBarDetector::getBackProjectedTopWidth(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  double halfWidth = getFieldInfo().goalWidth * 0.5;
  Vector3d pt1(p1.x, p1.y - halfWidth, getFieldInfo().goalHeight);
  Vector3d pt2(p1.x, p1.y + halfWidth, getFieldInfo().goalHeight);

  Vector2i topInImage1;
  Vector2i topInImage2;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt2,topInImage2);


  return (topInImage1 - topInImage2).abs();
}


bool GoalCrossBarDetector::estimateCrossBarDirection(const GoalPercept::GoalPost& post, Vector2i& start, Vector2d& direction)
{
  direction = post.directionInImage; 
  direction.rotateRight();

  start = getBackProjectedTopPoint(post);

  Vector2d scanOffset = post.directionInImage * post.seenWidth * 3.0;
  
  Vector2i pointLeft(post.topPoint - direction * parameters.scanlinesDistance);
  Vector2i pointRight(post.topPoint + direction * parameters.scanlinesDistance);

  Vector2i downStart(pointLeft - scanOffset);
  Vector2i downEnd(pointLeft + scanOffset);

  scanDown(0, pointLeft - scanOffset, pointLeft + scanOffset);
  scanDown(1, pointRight - scanOffset, pointRight + scanOffset);

  size_t ii = 0;
  size_t jj = 0;
  double widthDiff = 1000.0;
  bool barFound = false;
  for(size_t i = 0; i <= 1; i++)
  {
    for(size_t j = 0; j < features[i].size(); j++)
    {
      double d = fabs(features[i][j].width - post.seenWidth);
      if(d < widthDiff &&  features[i][j].width >= post.seenWidth * parameters.barWidthSimilarity)
      {
        ii = i;
        jj = j;
        widthDiff = d;
        barFound = true;
      }
    }
  }

  if(barFound)
  {
    //remember this feature
    positiveTestFeatures.push_back(features[ii][jj]);
    if(ii == 0)
    {
      //if scan direction is left, flip dircetion
      direction = -direction;
    }
    //calculate a second test feature for better scan direction estimation
    Vector2i nextPoint(features[ii][jj].point + direction * parameters.scanlinesDistance);
    scanDown(2, nextPoint - scanOffset, nextPoint + scanOffset);
   
    if(ii == 0)
    {
      //if left scan flip feature direction
      direction -= features[ii][jj].direction;
    }
    else
    {
      direction += features[ii][jj].direction;
    }
    size_t jjj = 0;
    double widthDiff = 1000.0;
    bool featureFound = false;
    for(size_t j = 0; j < features[2].size(); j++)
    {
      double d = fabs(features[2][j].width - post.seenWidth);
      if(d < widthDiff &&  features[2][j].width >= post.seenWidth * parameters.barWidthSimilarity)
      {
        jjj = j;
        widthDiff = d;
        featureFound = true;
      }
    }
    direction.normalize();

    ////calculate better start point by intersecting the goal post line with the estimated scan line
    //Math::Line scanLine(features[ii][jj].point, direction);
    //Math::Line postLine(post.basePoint, post.directionInImage);
    //double d = postLine.intersection(scanLine);
    //start = (postLine.point(d) + post.topPoint) * 0.5;

    if(featureFound)
    {
      //remember this feature
      positiveTestFeatures.push_back(features[2][jjj]);
      if(ii == 0)
      {
        //if left scan flip feature direction
        direction -= features[2][jjj].direction;
      }
      else
      {
        direction += features[2][jjj].direction;
      }
      features[2].clear();
    }
    features[0].clear();
    features[1].clear();

    if (direction.y > 0)
    {
      direction.y = 0.0;
    }
    direction.normalize();




    ////calculate top point in post from top point corrected with half the width of the bar
    //Vector2i postPoint(post.topPoint);// + post.directionInImage * post.seenWidth * 0.5);
    //direction = (features[ii][jj].direction + features[ii][jj].point - postPoint).normalize();
  }
  return barFound;
}


void GoalCrossBarDetector::scanAlongCrossBar(const Vector2i& start, const Vector2i& end, const Vector2d& direction, double width)
{
  BresenhamLineScan barScan(start, end);

  Vector2i pos(start);
  Vector2d scanOffset = direction * width * 3.0;
  
  size_t dist = parameters.scanlinesDistance;

  Filter<Gauss5x1, Vector2i, double, 5> filter;
  Pixel pixel;

  for(size_t actDist = 0; actDist < (size_t) barScan.numberOfPixels; actDist++)
  {
    barScan.getNext(pos);

    //ignore points outside of image
    if(!getImage().isInside(pos.x,pos.y))
    {
      continue;
    }
    getImage().get(pos.x, pos.y, pixel);

    DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_scanlines",
      POINT_PX(ColorClasses::gray, pos.x,pos.y);
    );
    
    int pixValue = parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));
    filter.add(pos, pixValue);

    //if(filter.ready() && filter.value() < parameters.threshold) 
    //{
    //  pos.y -= static_cast<int>(width * parameters.barWidthSimilarity);
    //  continue;
    //  //break; 
    //}

    if(actDist % dist == dist / 2)
    {
      Vector2i downStart(pos - scanOffset);
      Vector2i downEnd(pos + scanOffset);

      size_t id = actDist / dist;
      //if(id >= lastCrossBarScanLineId)
      {
        scanDown(id, downStart, downEnd);
      }
    }
  }
}//end ScanAlongCrossBar 

size_t GoalCrossBarDetector::scanDown(size_t id, const Vector2i& downStart, const Vector2i& downEnd)
{
  if(parameters.useColorFeatures)
  {
    return scanDownColor(id, downStart, downEnd);
  }
  else
  {
    return scanDownDiff(id, downStart, downEnd);
  }
}

size_t GoalCrossBarDetector::scanDownColor(size_t id, const Vector2i& downStart, const Vector2i& downEnd)
{
  lastCrossBarScanLineId = id;
  if(features.size() <= id)
  {
    features.push_back(std::vector<GoalBarFeature>());
  }

  BresenhamLineScan downScan(downStart, downEnd);
  Vector2i pos(downStart);

  Filter<Gauss5x1, Vector2i, double, 5> filter;
    
  Vector2i begin;
  bool begin_found = false;
  double jump = 0;

  Pixel pixel;

  for(int p = 0; p < downScan.numberOfPixels; p++)
  {
    downScan.getNext(pos);
    //ignore pixels outside of image
    if(!getImage().isInside(pos.x,pos.y))
    {
      continue;
    }
    getImage().get(pos.x, pos.y, pixel);
    int pixValue = parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));

    DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_scanlines",
      POINT_PX(ColorClasses::gray, pos.x, pos.y );
    );

    filter.add(pos, pixValue);
    if(!filter.ready()) {
      continue;
    }

    if(!begin_found)
    {
      if(filter.value() > parameters.threshold)
      {
        begin = filter.point();
        DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks",
          POINT_PX(ColorClasses::red, begin.x, begin.y );
        );
        jump = filter.value() - jump;
        begin_found = true;
      }
      else
      {
        jump = filter.value();
      }
    }

    if(begin_found && filter.value() + jump*0.5 < parameters.threshold)
    {
      const Vector2i& end = filter.point();
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks",
        POINT_PX(ColorClasses::red, end.x, end.y );
      );

      Vector2d gradientBegin = parameters.detectWhiteGoals ? calculateGradientY(begin) : calculateGradientUV(begin);
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markGradients", 
        LINE_PX(ColorClasses::black, begin.x, begin.y, begin.x + (int)(10*gradientBegin.x+0.5), begin.y + (int)(10*gradientBegin.y+0.5));
      );
      Vector2d gradientEnd = parameters.detectWhiteGoals ? calculateGradientY(end) : calculateGradientUV(end);
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markGradients", 
        LINE_PX(ColorClasses::black, end.x, end.y, end.x + (int)(10*gradientEnd.x+0.5), end.y + (int)(10*gradientEnd.y+0.5));
      );
      int featureWidth = (end - begin).abs();
      if(fabs(gradientBegin*gradientEnd) > parameters.thresholdFeatureGradient && featureWidth < parameters.maxFeatureWidth) 
      {
        DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks", 
          LINE_PX(ColorClasses::blue, begin.x, begin.y, end.x, end.y);
          POINT_PX(ColorClasses::red, begin.x, begin.y );
          POINT_PX(ColorClasses::red, end.x, end.y );
        );

        GoalBarFeature crossBarFeature;
        crossBarFeature.point = Vector2d(begin + end)*0.5;
        crossBarFeature.direction = (gradientBegin - gradientEnd).normalize();
        //postFeature.begin = begin;
        //postFeature.end = end;
        crossBarFeature.width = featureWidth;
          
        features[id].push_back(crossBarFeature);
      }
      begin_found = false;
    }
  }
  return features[id].size();
}

size_t GoalCrossBarDetector::scanDownDiff(size_t id, const Vector2i& downStart, const Vector2i& downEnd)
{
  lastCrossBarScanLineId = id;
  if(features.size() <= id)
  {
    features.push_back(std::vector<GoalBarFeature>());
  }

  BresenhamLineScan downScan(downStart, downEnd);
  Vector2i pos(downStart);

//  Filter<Prewitt3x1, Vector2i, double, 3> filter;
  Filter<Diff5x1, Vector2i, double, 5> filter;

  // initialize the scanner
  Vector2i peak_point_max(pos);
  Vector2i peak_point_min(pos);
  MaximumScan<Vector2i,double> positiveScan(peak_point_max, parameters.thresholdGradient);
  MaximumScan<Vector2i,double> negativeScan(peak_point_min, parameters.thresholdGradient);

  bool begin_found = false;

  Pixel pixel;
  for(int p = 0; p < downScan.numberOfPixels; p++)
  {
    downScan.getNext(pos);
    //ignore pixels outside of image
    if(!getImage().isInside(pos.x,pos.y))
    {
      continue;
    }
    getImage().get(pos.x, pos.y, pixel);
    int pixValue =  parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));

      DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_scanlines",
      POINT_PX(ColorClasses::gray, pos.x, pos.y );
    );

    filter.add(pos, pixValue);
    if(!filter.ready()) {
      continue;
    }

    if(positiveScan.add(filter.point(), filter.value()))
    {
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks",
        POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y );
      );
      begin_found = true;
    }

    if(negativeScan.add(filter.point(), -filter.value()))
    {
      Vector2d gradientBegin = parameters.detectWhiteGoals ? calculateGradientY(peak_point_max) : calculateGradientUV(peak_point_max);
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markGradients",
        LINE_PX(ColorClasses::black, peak_point_max.x, peak_point_max.y, peak_point_max.x + (int)(10*gradientBegin.x+0.5), peak_point_max.y + (int)(10*gradientBegin.y+0.5));
      );
      Vector2d gradientEnd = parameters.detectWhiteGoals ? calculateGradientY(peak_point_min) : calculateGradientUV(peak_point_min);
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markGradients",
        LINE_PX(ColorClasses::black, peak_point_min.x, peak_point_min.y, peak_point_min.x + (int)(10*gradientEnd.x+0.5), peak_point_min.y + (int)(10*gradientEnd.y+0.5));
      );
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks",
        POINT_PX(ColorClasses::pink, peak_point_min.x, peak_point_min.y );
      );
      // double edgel
      int featureWidth = (peak_point_max - peak_point_min).abs();
      if(begin_found && fabs(gradientBegin*gradientEnd) > parameters.thresholdFeatureGradient && featureWidth < parameters.maxFeatureWidth) 
      {
        DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks",
          LINE_PX(ColorClasses::blue, peak_point_max.x, peak_point_max.y, peak_point_min.x, peak_point_min.y);
          POINT_PX(ColorClasses::red, peak_point_max.x, peak_point_max.y );
          POINT_PX(ColorClasses::red, peak_point_min.x, peak_point_min.y );
        );
        GoalBarFeature crossBarFeature;
        crossBarFeature.point = Vector2d(peak_point_max + peak_point_min)*0.5;
        crossBarFeature.direction = (gradientBegin - gradientEnd).normalize();
        //crossBarFeature.begin = peak_point_max;
        //crossBarFeature.end = peak_point_min;
        crossBarFeature.width = featureWidth;

        features[id].push_back(crossBarFeature);
      }
      begin_found = false;
    }
  }//end while
  return features[id].size();
}

void GoalCrossBarDetector::testBackProjection()
{
  if(getGoalPercept().getNumberOfSeenPosts() > 1)
  {
    for(int i = 1; i < getGoalPercept().getNumberOfSeenPosts(); i++)
    {
      Vector2d p1 = getGoalPercept().getPost(i - 1).position;
      Vector2d p2 = getGoalPercept().getPost(i).position;
      Vector3d pb1(p1.x, p1.y, 0.0);
      Vector3d pb2(p2.x, p2.y, 0.0);
      Vector3d pt1(p1.x, p1.y, getFieldInfo().goalHeight);
      Vector3d pt2(p2.x, p2.y, getFieldInfo().goalHeight);

      Vector2i baseInImage1;
      Vector2i topInImage1;
      Vector2i baseInImage2;
      Vector2i topInImage2;

      CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pb1,baseInImage1); 
      CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 

      CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pb2,baseInImage2); 
      CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt2,topInImage2); 

      LINE_PX(ColorClasses::red, baseInImage1.x, baseInImage1.y, topInImage1.x, topInImage1.y);
      LINE_PX(ColorClasses::red, baseInImage2.x, baseInImage2.y, topInImage2.x, topInImage2.y);

    }
  }
  else if(getGoalPercept().getNumberOfSeenPosts() > 0)
  {
      Vector2d p1 = getGoalPercept().getPost(0).position;
      Vector3d pb1(p1.x, p1.y, 0.0);
      Vector3d pt1(p1.x, p1.y, getFieldInfo().goalHeight);

      Vector2i baseInImage1;
      Vector2i topInImage1;
      CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pb1,baseInImage1); 
      CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 

      LINE_PX(ColorClasses::red, baseInImage1.x, baseInImage1.y, topInImage1.x, topInImage1.y);

  }
}


Vector2d GoalCrossBarDetector::calculateGradientUV(const Vector2i& point) const
{
  // no angle at the border (shouldn't happen)
  if( point.x < 2 || point.x + 3 > (int)getImage().width() ||
      point.y < 2 || point.y + 3 > (int)getImage().height() ) {
    return Vector2d();
  }

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  
  Vector2d gradientU;
  gradientU.x =
       (int)getImage().getU(point.x-2, point.y+2)
    +2*(int)getImage().getU(point.x  , point.y+2)
    +  (int)getImage().getU(point.x+2, point.y+2)
    -  (int)getImage().getU(point.x-2, point.y-2)
    -2*(int)getImage().getU(point.x  , point.y-2)
    -  (int)getImage().getU(point.x+2, point.y-2);

  gradientU.y =
       (int)getImage().getU(point.x-2, point.y-2)
    +2*(int)getImage().getU(point.x-2, point.y  )
    +  (int)getImage().getU(point.x-2, point.y+2)
    -  (int)getImage().getU(point.x+2, point.y-2)
    -2*(int)getImage().getU(point.x+2, point.y  )
    -  (int)getImage().getU(point.x+2, point.y+2);


  Vector2d gradientV;
  gradientV.x =
       (int)getImage().getV(point.x-2, point.y+2)
    +2*(int)getImage().getV(point.x  , point.y+2)
    +  (int)getImage().getV(point.x+2, point.y+2)
    -  (int)getImage().getV(point.x-2, point.y-2)
    -2*(int)getImage().getV(point.x  , point.y-2)
    -  (int)getImage().getV(point.x+2, point.y-2);

  gradientV.y =
       (int)getImage().getV(point.x-2, point.y-2)
    +2*(int)getImage().getV(point.x-2, point.y  )
    +  (int)getImage().getV(point.x-2, point.y+2)
    -  (int)getImage().getV(point.x+2, point.y-2)
    -2*(int)getImage().getV(point.x+2, point.y  )
    -  (int)getImage().getV(point.x+2, point.y+2);

  //calculate the angle of the gradient
  return (gradientV - gradientU).normalize();
}

Vector2d GoalCrossBarDetector::calculateGradientY(const Vector2i& point) const
{
  // no angle at the border (shouldn't happen)
  if( point.x < 2 || point.x + 3 > (int)getImage().width() ||
      point.y < 2 || point.y + 3 > (int)getImage().height() ) {
    return Vector2d();
  }

  Vector2d gradientY;
  gradientY.x =
       (int)getImage().getY(point.x-2, point.y+2)
    +2*(int)getImage().getY(point.x  , point.y+2)
    +  (int)getImage().getY(point.x+2, point.y+2)
    -  (int)getImage().getY(point.x-2, point.y-2)
    -2*(int)getImage().getY(point.x  , point.y-2)
    -  (int)getImage().getY(point.x+2, point.y-2);

  gradientY.y =
       (int)getImage().getY(point.x-2, point.y-2)
    +2*(int)getImage().getY(point.x-2, point.y  )
    +  (int)getImage().getY(point.x-2, point.y+2)
    -  (int)getImage().getY(point.x+2, point.y-2)
    -2*(int)getImage().getY(point.x+2, point.y  )
    -  (int)getImage().getY(point.x+2, point.y+2);

  return gradientY.normalize();
}
  