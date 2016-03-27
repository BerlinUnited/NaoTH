
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
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:draw_crossbar_candidates", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:draw_best_crossbar_candidate", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:draw_crossbar_endpoints", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:markPeaks", "mark found maximum u-v peaks in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:markGradients", "mark found gradients in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:markEstimationGradients", "mark found gradients while erstimating the bar direction in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:markWeakGradients", "mark found weak gradients in image", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalCrossBarDetector:showTestBackProjection", "mark backprojected post center line in image", false);

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
  clusters.clear();
  goalCandidates.clear();

  std::vector<int> seenReliablePosts;

  for(int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
  {
    const GoalPercept::GoalPost& post = getGoalPercept().getPost(i);
    if(post.positionReliable)
    {
      seenReliablePosts.push_back(i);
    }
  }

  if(seenReliablePosts.size() == 1)
  {
    checkSinglePost(getGoalPercept().getPost(seenReliablePosts[0]));
  }
  else if(seenReliablePosts.size() == 2)
  {
    const GoalPercept::GoalPost& post0 = getGoalPercept().getPost(seenReliablePosts[0]);
    const GoalPercept::GoalPost& post1 = getGoalPercept().getPost(seenReliablePosts[1]);
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
      checkSinglePost(post0);
      checkSinglePost(post1);
    }
  } 
  else if(seenReliablePosts.size() > 2)
  {
    std::vector<Goal> possibleGoals;
    for(size_t p0 = 0; p0 < seenReliablePosts.size() - 1; p0++)
    {
      for(size_t p1 = p0 + 1; p1 < seenReliablePosts.size(); p1++)
      {
        const GoalPercept::GoalPost& post0 = getGoalPercept().getPost(seenReliablePosts[p0]);
        const GoalPercept::GoalPost& post1 = getGoalPercept().getPost(seenReliablePosts[p1]);
        if(checkProjectedPostDistance(post0, post1))
        {
          Goal goal;
          goal.post0 = seenReliablePosts[p0];
          goal.post1 = seenReliablePosts[p1];
          possibleGoals.push_back(goal);
        }
      }
    }
    std::vector<int> usedPosts;
    if(possibleGoals.size() > 0)
    {
      for(size_t i = 0; i < possibleGoals.size(); i++)
      {
        const GoalPercept::GoalPost& post0 = getGoalPercept().getPost(possibleGoals[i].post0);
        const GoalPercept::GoalPost& post1 = getGoalPercept().getPost(possibleGoals[i].post1);
        //scan from post most left in image
        if(post1.topPoint.x < post0.topPoint.x)
        {
          checkBothPosts(post1, post0);
        }
        else
        {
          checkBothPosts(post0, post1);
        }
        usedPosts.push_back(possibleGoals[i].post0);
        usedPosts.push_back(possibleGoals[i].post1);
      }
    }
    for(size_t p = 0; p < seenReliablePosts.size(); p++)
    {
      bool isUsed = false;
      for(size_t i = 0; i < usedPosts.size(); i++)
      {
        if(usedPosts[i] == (int)p) 
        {
          isUsed = true;
          break;
        }
      }
      if(!isUsed)
      {
        checkSinglePost(getGoalPercept().getPost(seenReliablePosts[p]));
      }
    }
  }
  calcuateCrossBars();

  return true;
}//end execute

void GoalCrossBarDetector::checkSinglePost(const GoalPercept::GoalPost& post)
{
  DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_crossbar_endpoints",
    Vector2d topPoint = getBackProjectedTopPoint(post);
    int width = getBackProjectedTopBarWidth(post) / 2;
    CIRCLE_PX(ColorClasses::skyblue, (int) topPoint.x, (int) topPoint.y + width, width);
  );
  Vector2i start;
  Vector2d direction;
  bool barFound = estimateCrossBarDirection(post, start, direction);
  //double angle = direction.angle();
  if(barFound)
  {
    double backProjectedBarWidth = getBackProjectedTopBarWidth(post);
    double backProjectedPostHeight = getBackProjectedPostHeight(post);

    double meanBarWidth = (backProjectedBarWidth + post.seenWidth) * 0.5;

    double estimatedCrossBarLength = getBackProjectedGoalWidth(post, direction);
    CrossBar crossBar(direction, estimatedCrossBarLength);

    Vector2i end = start + direction * estimatedCrossBarLength;
    scanAlongCrossBar(start, end, Vector2d(0, -1),  meanBarWidth);
    clusterEdgelFeatures();
    GoalCandidate goalCandidate(post, crossBar);
    goalCandidates.push_back(goalCandidate);
  }
}

void GoalCrossBarDetector::checkBothPosts(const GoalPercept::GoalPost& postLeft, const GoalPercept::GoalPost& postRight)
{
  double backProjectedBarWidthLeft = getBackProjectedTopBarWidth(postLeft);
  double backProjectedBarWidthRight = getBackProjectedTopBarWidth(postRight);

  Vector2d meanPostDirection = (postLeft.directionInImage + postRight.directionInImage).normalize(); 
  double meanPostWidth =  0.25 * (backProjectedBarWidthLeft + backProjectedBarWidthRight + postLeft.seenWidth + postRight.seenWidth);

  //Vector2d offset =  meanPostDirection * meanPostWidth * 0.5;

  Vector2i start = getBackProjectedTopPoint(postLeft);;
  Vector2i end = getBackProjectedTopPoint(postRight);;

  DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_crossbar_endpoints",
      int widthLeft = getBackProjectedTopBarWidth(postLeft) / 2;
      int widthRight = getBackProjectedTopBarWidth(postRight) / 2;
      CIRCLE_PX(ColorClasses::blue, start.x, start.y, widthLeft);
      CIRCLE_PX(ColorClasses::blue, end.x, end.y, widthRight);
  );

  scanAlongCrossBar(start, end, Vector2d(0, -1), meanPostWidth);
  //scanAlongCrossBar(start, end, meanPostDirection, meanPostWidth);
  clusterEdgelFeatures();
      
  Vector2d bar(start - end); 
  CrossBar crossBar(bar.normalize(), bar.abs());
  GoalCandidate goalCandidate(postLeft, postRight, crossBar);
  goalCandidates.push_back(goalCandidate);
}

bool GoalCrossBarDetector::checkProjectedPostDistance(const GoalPercept::GoalPost& post0, const GoalPercept::GoalPost& post1)
{
  Vector2d p0 = post0.position;
  Vector2d p1 = post1.position;

  double projectedGoalWidth = (p1 - p0).abs();
  double goalWidth = getFieldInfo().goalWidth;
  double widthRatio = goalWidth > projectedGoalWidth ? goalWidth / projectedGoalWidth : projectedGoalWidth/ goalWidth;

  return (post0.positionReliable || post1.positionReliable) && widthRatio < parameters.maxGoalWidthRatio;
}

Vector2d GoalCrossBarDetector::getBackProjectedTopPoint(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  Vector3d pt1(p1.x, p1.y, getFieldInfo().goalHeight);

  Vector2i topInImage1;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1, topInImage1); 
  return topInImage1;
}

int GoalCrossBarDetector::getBackProjectedTopBarWidth(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  double halfWidth = getFieldInfo().goalpostRadius;
  Vector3d pt1(p1.x, p1.y - halfWidth, getFieldInfo().goalHeight);
  Vector3d pt2(p1.x, p1.y + halfWidth, getFieldInfo().goalHeight);

  Vector2i topInImage1;
  Vector2i topInImage2;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1, topInImage1); 
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt2, topInImage2);

  return (topInImage1 - topInImage2).abs();
}

int GoalCrossBarDetector::getBackProjectedGoalWidth(const GoalPercept::GoalPost& post, const Vector2d& direction)
{
  Vector2d pImg(post.basePoint + direction * 20);
  Vector2d pField;
  if(!CameraGeometry::imagePixelToFieldCoord(
		  getCameraMatrix(), 
		  getImage().cameraInfo,
		  pImg.x, 
		  pImg.y, 
		  0,
		  pField))
  {
    return -1;
  }
  Vector2d p1 = post.position;
  Vector2d p2 = p1 + (pField - p1).normalize() * getFieldInfo().goalWidth;
  

  Vector3d pt1(p1.x, p1.y, getFieldInfo().goalHeight);
  Vector3d pt2(p2.x, p2.y, getFieldInfo().goalHeight);

  Vector2i topInImage1;
  Vector2i topInImage2;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1, topInImage1); 
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt2, topInImage2);

  return (topInImage1 - topInImage2).abs();
}

int GoalCrossBarDetector::getBackProjectedPostHeight(const GoalPercept::GoalPost& post)
{
  Vector2d p = post.position;
  Vector3d pb(p.x, p.y, 0.0);
  Vector3d pt(p.x, p.y, getFieldInfo().goalHeight);

  Vector2i pbImg;
  Vector2i ptImg;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pb, pbImg); 
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt, ptImg);

  return (pbImg - ptImg).abs();
}

bool GoalCrossBarDetector::estimateCrossBarDirection(const GoalPercept::GoalPost& post, Vector2i& start, Vector2d& direction)
{
  direction = post.directionInImage; 
  direction.rotateRight();

  start = getBackProjectedTopPoint(post);

  Vector2d scanOffset = post.directionInImage * post.seenWidth * 4.0;
 
  //Vector2i pointLeft(post.topPoint - direction * post.seenWidth * 1.5);
  //Vector2i pointRight(post.topPoint + direction * post.seenWidth * 1.5);
  Vector2d pointLeft(start - direction * post.seenWidth * 1.5);
  Vector2d pointRight(start + direction * post.seenWidth * 1.5);

  Vector2i downStart(pointLeft - scanOffset);
  Vector2i downEnd(pointLeft + scanOffset);

  scanDown(0, pointLeft - scanOffset, pointLeft + scanOffset, post.seenWidth);
  scanDown(1, pointRight - scanOffset, pointRight + scanOffset, post.seenWidth);

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
    if(ii == 0)
    {
      //if scan direction is left, flip direction
      direction = -direction;
    }
    //calculate a second test feature for better scan direction estimation
    Vector2i nextPoint(start + direction * post.seenWidth * 3.0);
    scanDown(2, nextPoint - scanOffset, nextPoint + scanOffset, post.seenWidth);

    DEBUG_REQUEST("Vision:GoalCrossBarDetector:markEstimationGradients",
      LINE_PX(ColorClasses::black, (int) features[ii][jj].point.x, (int) features[ii][jj].point.y, (int) (features[ii][jj].point.x + 5 * direction.x), (int) (features[ii][jj].point.y + 5 * direction.y));
    );
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

    if(featureFound)
    {
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markEstimationGradients",
        LINE_PX(ColorClasses::black, (int) features[2][jjj].point.x, (int) features[2][jjj].point.y, (int) (features[2][jjj].point.x + 5 * direction.x), (int) (features[2][jjj].point.y + 5 * direction.y));
      );
      if(ii == 0)
      {
        //if left scan flip feature direction
        direction -= features[2][jjj].direction;
      }
      else
      {
        direction += features[2][jjj].direction;
      }
      //features[2].clear();
    }

    if(ii == 0)
    {
      //if left scan, clear right test features
      features[1].clear();
    }
    else
    {
      //if right scan, clear left test features
      features[0].clear();
    }

    //angle of cross bar should not be less than 90° therefor we can restrict the search direction to 90° if its too small
    if (direction.y > 0)
    {
      direction.y = 0.0;
    }
    direction.normalize();
  }

  return barFound;
}

void GoalCrossBarDetector::scanAlongCrossBar(const Vector2i& start, const Vector2i& end, const Vector2d& direction, double barWidth)
{
  BresenhamLineScan barScan(start, end);

  Vector2i pos(start);
  Vector2d scanOffset = direction * barWidth * 3.0;
  
  //size_t dist = parameters.scanlinesDistance;
  size_t dist = static_cast<size_t> (1.5 * barWidth);

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

    if(actDist % dist == dist / 2)
    {
      Vector2i downStart(pos + scanOffset);
      Vector2i downEnd(pos - scanOffset);

      size_t id = actDist / dist;
      //if(id >= lastCrossBarScanLineId)
      {
        scanDown(id, downStart, downEnd, barWidth);
      }
    }
  }
}//end ScanAlongCrossBar 

size_t GoalCrossBarDetector::scanDown(size_t id, const Vector2i& downStart, const Vector2i& downEnd, double barWidth)
{
  if(parameters.useColorFeatures)
  {
    return scanDownColor(id, downStart, downEnd, barWidth);
  }
  else
  {
    return scanDownDiff(id, downStart, downEnd, barWidth);
  }
}

size_t GoalCrossBarDetector::scanDownColor(size_t id, const Vector2i& downStart, const Vector2i& downEnd, double barWidth)
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
      if
      (
        fabs(gradientBegin*gradientEnd) > parameters.thresholdFeatureGradient && 
        featureWidth < parameters.maxFeatureWidth &&
        featureWidth >= barWidth * parameters.barWidthSimilarity
      ) 
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

size_t GoalCrossBarDetector::scanDownDiff(size_t id, const Vector2i& downStart, const Vector2i& downEnd, double /*barWidth*/)
{
  lastCrossBarScanLineId = id;
  if(features.size() <= id)
  {
    features.push_back(std::vector<GoalBarFeature>());
  }

  BresenhamLineScan downScan(downStart, downEnd);
  Vector2i pos(downStart);

  Filter<Diff5x1, Vector2i, double, 5> filter;

  bool edgeFound = false;

  Pixel pixel;

  EdgelD lastEdgel;

  Vector2i lastPos;

  // initialize the scanner
  Vector2i peak_point(pos);
  Vector2i peak_point_weak(pos);
  Vector2i peak_point_max(pos);
  Vector2i peak_point_min(pos);
  Vector2i peak_point_weak_max(pos);
  Vector2i peak_point_weak_min(pos);
  MaximumScan<Vector2i,double> maxScan(peak_point_max, parameters.thresholdGradient);
  MaximumScan<Vector2i,double> minScan(peak_point_min, parameters.thresholdGradient);
  MaximumScan<Vector2i,double> maxWeakScan(peak_point_weak_max, parameters.thresholdWeakGradient);
  MaximumScan<Vector2i,double> minWeakScan(peak_point_weak_min, parameters.thresholdWeakGradient);
    
  bool weakPeakFound = false;
  bool filterSwitchReady = false;
  bool artificialEndPointSet = false;

  std::vector<EdgelD> weakJumpPoints;
  weakJumpPoints.reserve(4);

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

    if(pixValue > parameters.threshold)
    {
      //add pixValue to filter if above the threshold
      filter.add(pos, pixValue);
    }
    else
    {
      //add half of parameters.threshold as base level to filter if below or equal to the threshold
      filter.add(pos, parameters.threshold);
    }

    if(!filter.ready()) {
        filterSwitchReady = true;
      continue;
    }
    if(pixValue > parameters.threshold && p > downScan.numberOfPixels - 3)
    {
      if(!artificialEndPointSet)
      {
        filterSwitchReady = true;
        artificialEndPointSet = true;
      }
    }

    bool peakFound = false;
    if(parameters.useArtificialPoints && pixValue > parameters.threshold && filterSwitchReady)
    {
      peakFound = true;
      peak_point = filter.point();
    }

    //double absValue = fabs(filter.value());

    //if(maxScan.add(filter.point(), absValue))

    if(maxScan.add(filter.point(), filter.value()))
    {
      peakFound = true;
      peak_point = peak_point_max;
    }
    else if(minScan.add(filter.point(), -filter.value()))
    {
      peakFound = true;
      peak_point = peak_point_min;
    }

    bool edgeValid = false;
    if(peakFound)
    {
      Vector2d leftRight = (downEnd - downStart).normalize();
      leftRight.rotateRight();

      Vector2d gradient;

      if(parameters.useArtificialPoints && filterSwitchReady)
      {
        // assume edge direction along the normal of the horizon for the artificially created jump points
        // artificial jump points are created, if the images left side is very bright (i.e. the post is only partially seen)
        gradient = leftRight;
        filterSwitchReady = false;
      }
      else
      {
        gradient = parameters.detectWhiteGoals ? calculateGradientY(peak_point) : calculateGradientUV(peak_point);
      }
      //Vector2d gradient = parameters.detectWhiteGoals ? calculateGradientY(peak_point) : calculateGradientUV(peak_point);
      EdgelD newEdgel;

      if(gradient.x < 0)
      {
        gradient *= -1;
        newEdgel.type = EdgelD::positive; 
      }
      else
      {
        newEdgel.type = EdgelD::negative; 
      }

      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markGradients",
        LINE_PX(ColorClasses::black, peak_point.x, peak_point.y, peak_point.x + (int)(10*gradient.x+0.5), peak_point.y + (int)(10*gradient.y+0.5));
      );
      newEdgel.point = peak_point;
      newEdgel.direction = gradient;

      ColorClasses::Color col;
      ColorClasses::Color peakCol = ColorClasses::red;
      if(newEdgel.type == EdgelD::negative) 
      {
        col = ColorClasses::pink;
      }
      else
      {
        col = ColorClasses::orange;
        peakCol = ColorClasses::pink;
      }

      //since gradient is normalized we can just check abs(gradient.x) to sort out all not upright edges
      if(fabs(fabs(gradient.x) - fabs(leftRight.x)) < 0.2)
      {
        DEBUG_REQUEST("Vision:GoalCrossBarDetector:markGradients",
          LINE_PX(col, (int)newEdgel.point.x, (int)newEdgel.point.y, (int)newEdgel.point.x + (int)(10*newEdgel.direction.x+0.5), (int)newEdgel.point.y + (int)(10*newEdgel.direction.y+0.5));
          POINT_PX(ColorClasses::blue, (int)newEdgel.point.x, (int)newEdgel.point.y );
        );

        if(edgeFound)
        { 
          double sim = lastEdgel.sim2(newEdgel);
          if(sim > parameters.thresholdFeatureGradient)
          {
            GoalBarFeature feature;
            feature.point = Vector2d(lastEdgel.point + newEdgel.point) * 0.5;
          
            getImage().get((int)  feature.point.x, (int)  feature.point.y, pixel);
            int pixValue =  parameters.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));
            if(pixValue > parameters.threshold)
            {
              double featureWidth = (newEdgel.point - lastEdgel.point).abs();
              GoalBarFeature crossBarFeature;
              crossBarFeature.point = peak_point;
              crossBarFeature.direction = (lastEdgel.direction + newEdgel.direction).normalize();;
              crossBarFeature.width = featureWidth;
              
              crossBarFeature.begin.point = lastEdgel.point;
              crossBarFeature.begin.direction = lastEdgel.direction;
              crossBarFeature.begin.type = lastEdgel.type;
                
              crossBarFeature.end.point = newEdgel.point;
              crossBarFeature.end.direction = newEdgel.direction;
              crossBarFeature.end.type = newEdgel.type;

              features[id].push_back(crossBarFeature);
                
              if(parameters.useWeakJumpsToo && weakJumpPoints.size() > 0)
              {
                for(size_t w = 0; w < weakJumpPoints.size(); w++)
                {
                  //add a additional feature from lastedgel to weak edgel
                  if((newEdgel.point - weakJumpPoints[w].point).abs2() > 2)
                  {
                    GoalBarFeature featureLast2Weak;
                    featureLast2Weak.point = Vector2d(lastEdgel.point + weakJumpPoints[w].point) * 0.5;
                    double featureWidth = (weakJumpPoints[w].point - lastEdgel.point).abs();
                    featureLast2Weak.begin.point = lastEdgel.point;
                    featureLast2Weak.begin.direction = lastEdgel.direction;
                    featureLast2Weak.begin.type = lastEdgel.type;
                
                    featureLast2Weak.end.point = weakJumpPoints[w].point;
                    featureLast2Weak.end.direction = weakJumpPoints[w].direction;
                    featureLast2Weak.end.type = weakJumpPoints[w].type;

                    featureLast2Weak.direction = (lastEdgel.direction + weakJumpPoints[w].direction).normalize();
                    featureLast2Weak.width = featureWidth;
                    features[id].push_back(featureLast2Weak);

                    //feature.weakJumps.push_back(weakJumpPoints[w]);
                    DEBUG_REQUEST("Vision:GoalCrossBarDetector:markWeakGradients",
                      LINE_PX(ColorClasses::red, (int)weakJumpPoints[w].point.x - (int)(4*weakJumpPoints[w].direction.x+0.5), (int)weakJumpPoints[w].point.y - (int)(4*weakJumpPoints[w].direction.y+0.5), (int)weakJumpPoints[w].point.x + (int)(4*weakJumpPoints[w].direction.x+0.5), (int)weakJumpPoints[w].point.y + (int)(4*weakJumpPoints[w].direction.y+0.5));
                    );
                  }
                  //add a additional feature from between weak edgels
                  if(w > 0 && (weakJumpPoints[w - 1].point - weakJumpPoints[w].point).abs2() > 2)
                  {
                    //add a feature from lastedgel to weak edgel
                    GoalBarFeature featureWeak2Weak;
                    featureWeak2Weak.point = Vector2d(weakJumpPoints[w - 1].point + weakJumpPoints[w].point) * 0.5;
                    double featureWidth = (weakJumpPoints[w].point - weakJumpPoints[w - 1].point).abs();
                    featureWeak2Weak.begin.point = weakJumpPoints[w - 1].point;
                    featureWeak2Weak.begin.direction = weakJumpPoints[w - 1].direction;
                    featureWeak2Weak.begin.type = weakJumpPoints[w - 1].type;
                
                    featureWeak2Weak.end.point = weakJumpPoints[w].point;
                    featureWeak2Weak.end.direction = weakJumpPoints[w].direction;
                    featureWeak2Weak.end.type = weakJumpPoints[w].type;

                    featureWeak2Weak.direction = (weakJumpPoints[w - 1].direction + weakJumpPoints[w].direction).normalize();
                    featureWeak2Weak.width = featureWidth;
                    features[id].push_back(featureWeak2Weak);
                  }
                }
                for(size_t w = 0; w < weakJumpPoints.size(); w++)
                {
                  if((newEdgel.point - weakJumpPoints[w].point).abs2() > 2)
                  {
                    GoalBarFeature featureWeak2New;
                    featureWeak2New.point = Vector2d(weakJumpPoints[w].point + newEdgel.point) * 0.5;
                    double featureWidth = (newEdgel.point - weakJumpPoints[w].point).abs();
                    featureWeak2New.begin.point = weakJumpPoints[w].point;
                    featureWeak2New.begin.direction = weakJumpPoints[w].direction;
                    featureWeak2New.begin.type = weakJumpPoints[w].type;
                
                    featureWeak2New.end.point = newEdgel.point;
                    featureWeak2New.end.direction = newEdgel.direction;
                    featureWeak2New.end.type = newEdgel.type;

                    featureWeak2New.direction = (weakJumpPoints[w].direction + newEdgel.direction).normalize();
                    featureWeak2New.width = featureWidth;
                    features[id].push_back(featureWeak2New);
                  }
                }
              }

              DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks",
                LINE_PX(ColorClasses::blue, (int)lastEdgel.point.x, (int)lastEdgel.point.y, (int)newEdgel.point.x,  (int)newEdgel.point.y);
              );
              DEBUG_REQUEST("Vision:GoalCrossBarDetector:markGradients",
                LINE_PX(ColorClasses::green, (int) feature.point.x, (int) feature.point.y, (int) feature.point.x + (int)(10* feature.direction.x+0.5), (int) feature.point.y + (int)(10* feature.direction.y+0.5));
                POINT_PX(ColorClasses::blue, (int) feature.point.x, (int) feature.point.y );
              );
            }
          }
        }
        edgeFound = true;
      }
      DEBUG_REQUEST("Vision:GoalCrossBarDetector:markPeaks",
        POINT_PX(peakCol, peak_point.x, peak_point.y );
      );
      lastEdgel.type = newEdgel.type;
      lastEdgel.direction = newEdgel.direction;
      lastEdgel.point = newEdgel.point;
      weakJumpPoints.clear();
    }

    if(parameters.useWeakJumpsToo)
    {
      if(!peakFound)
      {
        if(maxWeakScan.add(filter.point(), filter.value()))
        {
          weakPeakFound = true;
          peak_point_weak = peak_point_weak_max;
        }
        else if(minWeakScan.add(filter.point(), -filter.value()))
        {
          weakPeakFound = true;
          peak_point_weak = peak_point_weak_min;
        }
      }

      if(!edgeValid && weakPeakFound)
      {
        //Vector2d upright = getArtificialHorizon().getDirection();
        //upright.rotateRight();
        Vector2d gradientWeak = parameters.detectWhiteGoals ? calculateGradientY(peak_point_weak) : calculateGradientUV(peak_point_weak);
        if(/*fabs(fabs(gradientWeak.y) - fabs(upright.y)) < 0.2 &&*/ (lastEdgel.point - peak_point_weak).abs2() > 2)
        {
          EdgelD weakEdgel;

          if(gradientWeak.x < 0)
          {
            gradientWeak *= -1;
            weakEdgel.type = EdgelD::positive; 
          }
          else
          {
            weakEdgel.type = EdgelD::negative; 
          }
          weakEdgel.point = peak_point_weak;
          weakEdgel.direction = gradientWeak;
          weakJumpPoints.push_back(weakEdgel);
        }
      }
    }
    weakPeakFound = false;

  }//end while
  return features[id].size();
}

void GoalCrossBarDetector::clusterEdgelFeatures()
{
  std::vector<GoalBarFeature> pairs;
  for(size_t scanIdOne = 0; scanIdOne + 1 < features.size(); scanIdOne++) 
  {
    const std::vector<GoalBarFeature>& scanlineOne = features[scanIdOne];
    for(size_t i = 0; i < scanlineOne.size(); i++) 
    {
      const GoalBarFeature& e1 = scanlineOne[i];

      size_t scanIdTwo = scanIdOne+1;
      for(size_t j = 0; j < features[scanIdTwo].size(); j++) 
      {
        const GoalBarFeature& e2 = features[scanIdTwo][j];

        if(e1.sim(e2) > parameters.thresholdFeatureSimilarity) 
        {
          GoalBarFeature pair;
          pair.point = (e1.point + e2.point)*0.5;
          pair.direction = (e2.direction + e1.direction).normalize();
          pair.width = (e2.width + e1.width)*0.5;
          pairs.push_back(pair);
        }
      }
    }
  }

  for(size_t i = 0; i < pairs.size(); i++) 
  {
    const GoalBarFeature& e1 = pairs[i];

    size_t cluster_idx = 0;
    double max_sim = -1;

    for(size_t j = 0; j < clusters.size(); j++) 
    {
      Cluster& cluster = clusters[j];
      double s = cluster.sim(e1);
      if(s > max_sim) {
        max_sim = s;
        cluster_idx = j;
      }
    }

    if(max_sim > parameters.thresholdFeatureSimilarity) {
      clusters[cluster_idx].add(e1);
    } else {
      Cluster cluster;
      cluster.add(e1);
      clusters.push_back(cluster);
    }
  }
}

void GoalCrossBarDetector::calcuateCrossBars()
{
  CrossBarCandidates bestCandidates;

  for(size_t j = 0; j < clusters.size(); j++) 
  {
    Cluster& cluster = clusters[j];
      
    if(cluster.size() > parameters.minGoodPoints) 
    {
      Math::Line line = cluster.getLine();

      for(size_t i = 0; i < goalCandidates.size(); i++)
      {
        Vector2d begin;
        Vector2d end;

        if(goalCandidates[i].postCount == 1)
        {
          Math::Line post(goalCandidates[i].goalPost.basePoint, goalCandidates[i].goalPost.directionInImage);
          double d = line.intersection(post);
          begin = line.point(d);

          if(goalCandidates[i].crossBar.direction.x < 0)
          {
            end = begin - line.getDirection() * goalCandidates[i].crossBar.length;
          }
          else
          {
            end = begin + line.getDirection() * goalCandidates[i].crossBar.length;
          }

          Vector2d topPoint = getBackProjectedTopPoint(goalCandidates[i].goalPost);
          double width = getBackProjectedTopBarWidth(goalCandidates[i].goalPost) * parameters.maxGoalWidthRatio;
          bool isNearTopPoint = fabs(topPoint.x - begin.x) <= width && fabs(topPoint.y - begin.y) <= 2.0 * width;

          if(isNearTopPoint)
          {
            double distance = (topPoint - begin).abs();
            if(!bestCandidates.exists())
            {
              bestCandidates.ids.push(i);
              bestCandidates.distances.push(distance);
              bestCandidates.begins.push(begin);
              bestCandidates.ends.push(end);
            }
            else
            {
              if(distance < bestCandidates.distances.front())
              {
                bestCandidates.ids.push(i);
                bestCandidates.distances.push(distance);
                bestCandidates.begins.push(begin);
                bestCandidates.ends.push(end);
              }
            }
          }

          DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_crossbar_candidates",  
            if(isNearTopPoint)
            {
              width *= 0.5;
              CIRCLE_PX(ColorClasses::green, (int) begin.x, (int) (begin.y), (int) width);
            }
            else
            {
              width *= 0.5;
              CIRCLE_PX(ColorClasses::red, (int) begin.x, (int) (begin.y), (int) width);
            }
          );

        }
        else if(goalCandidates[i].postCount == 2)
        {
          Math::Line postLeft(goalCandidates[i].goalPostLeft.basePoint, goalCandidates[i].goalPostLeft.directionInImage);
          Math::Line postRight(goalCandidates[i].goalPostRight.basePoint, goalCandidates[i].goalPostRight.directionInImage);

          double d1 = line.intersection(postLeft);
          double d2 = line.intersection(postRight);

          begin = line.point(d1);
          end = line.point(d2);

          Vector2d topLeftPoint = getBackProjectedTopPoint(goalCandidates[i].goalPostLeft);
          Vector2d topRightPoint = getBackProjectedTopPoint(goalCandidates[i].goalPostRight);
          double widthLeft = getBackProjectedTopBarWidth(goalCandidates[i].goalPostLeft) * parameters.maxGoalWidthRatio;
          double widthRight = getBackProjectedTopBarWidth(goalCandidates[i].goalPostRight) * parameters.maxGoalWidthRatio;
          bool isNearTopLeftPoint = fabs(topLeftPoint.x - begin.x) <= widthLeft && fabs(topLeftPoint.y - begin.y) <= 2.0 * widthLeft;
          bool isNearTopRightPoint = fabs(topRightPoint.x - begin.x) <= widthRight && fabs(topRightPoint.y - begin.y) <= 2.0 * widthRight;

          if(isNearTopLeftPoint || isNearTopRightPoint)
          {
            double distanceLeft = (topLeftPoint - begin).abs();
            double distanceRight = (topRightPoint - end).abs();
            double distance = distanceLeft < distanceRight ? distanceLeft : distanceRight;
            if(!bestCandidates.exists())
            {
              bestCandidates.ids.push(i);
              bestCandidates.distances.push(distance);
              bestCandidates.begins.push(begin);
              bestCandidates.ends.push(end);
            }
            else
            {
              if(distance < bestCandidates.distances.front())
              {
                bestCandidates.ids.push(i);
                bestCandidates.distances.push(distance);
                bestCandidates.begins.push(begin);
                bestCandidates.ends.push(end);
              }
            }
          }

          DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_crossbar_candidates",  
            if(fabs(topLeftPoint.x - begin.x) <= widthLeft && fabs(topLeftPoint.y - begin.y) <= 2.0 * widthLeft)
            {
              widthLeft *= 0.5;
              CIRCLE_PX(ColorClasses::green, (int) begin.x, (int) (begin.y), (int) widthLeft);
            }
            else
            {
              widthLeft *= 0.5;
              CIRCLE_PX(ColorClasses::red, (int) begin.x, (int) (begin.y), (int) widthLeft);
            }
            if(fabs(topRightPoint.x - end.x) <= widthRight && fabs(topRightPoint.y - end.y) <= 2.0 * widthRight)
            {
              widthRight *= 0.5;
              CIRCLE_PX(ColorClasses::green, (int) end.x, (int) (end.y), (int) widthRight);
            }
            else
            {
              widthRight *= 0.5;
              CIRCLE_PX(ColorClasses::red, (int) end.x, (int) (end.y), (int) widthRight);
            }
          );

        }       
        //Vector2d begin, end, end2;
        //begin = line.getBase();
        //end = line.getBase() + line.getDirection()*50;
        //end2 = line.getBase() - line.getDirection()*50;
        DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_crossbar_candidates",  
          LINE_PX(ColorClasses::pink, (int)(begin.x+0.5), (int)(begin.y+0.5), (int)(end.x+0.5), (int)(end.y+0.5));
          //LINE_PX(ColorClasses::pink, (int)(begin.x+0.5), (int)(begin.y+0.5), (int)(end2.x+0.5), (int)(end2.y+0.5));
        );
      }
      //GoalPercept::GoalPost post;
      //post.directionInImage = line.getDirection();
      //post.basePoint = scanForEndPoint(line.getBase(), post.directionInImage);
      //post.topPoint = scanForEndPoint(line.getBase(), -post.directionInImage);
      //post.positionReliable = getFieldPercept().getValidField().isInside(post.basePoint);

      //// NOTE: if the projection is not successfull, then post.position = (0,0)
      //bool projectionOk = CameraGeometry::imagePixelToFieldCoord(
      //    getCameraMatrix(), getImage().cameraInfo,
      //    post.basePoint.x, post.basePoint.y, 0.0,
      //    post.position);

      //post.positionReliable = post.positionReliable && projectionOk;
      //post.seenWidth = cluster.getFeatureWidth();
      //post.seenHeight = (post.basePoint - post.topPoint).abs();

      //getGoalPercept().add(post);
    }
  }

  DEBUG_REQUEST("Vision:GoalCrossBarDetector:draw_best_crossbar_candidate",  
    if(bestCandidates.exists())
    {
      const Vector2d& b = bestCandidates.begins.front();
      const Vector2d& e = bestCandidates.ends.front();

      LINE_PX(ColorClasses::green, (int) b.x, (int) (b.y), (int) e.x, (int) (e.y));
      CIRCLE_PX(ColorClasses::green, (int) b.x, (int) (b.y), 10);
      CIRCLE_PX(ColorClasses::green, (int) e.x, (int) (e.y), 10);
    }
  );

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
      //LINE_PX(ColorClasses::red, topInImage1.x, topInImage1.y, topInImage2.x, topInImage2.y);
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
