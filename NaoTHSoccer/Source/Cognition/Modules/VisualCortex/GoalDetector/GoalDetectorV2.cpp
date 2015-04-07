
/**
* @file GoalDetectorV2.cpp
*
* Implementation of class GoalDetectorV2
*
*/

#include "GoalDetectorV2.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/Filter.h"
#include "Tools/ImageProcessing/MaximumScan.h"

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


GoalDetectorV2::GoalDetectorV2()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:markPostScans","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:markPosts","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:markValidPosts","..", true);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:markInvalidPosts","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:markPostCenterLine","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:markPostCenterBackProjected","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:markPostBackProjected","..", false);

  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:showColorByHistogramBottom","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetectorV2:showColorByHistogramTop","..", false);

  getDebugParameterList().add(&params);
}

GoalDetectorV2::~GoalDetectorV2()
{
  getDebugParameterList().remove(&params);
}

void GoalDetectorV2::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getGoalPercept().reset();

  // calculate the clusters
  clusters.clear();
  clusterEdgelFeatures();

  // calculate the goal posts based on the clusters
  calcuateGoalPosts();
  

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

  // Where is your god now?!
  if(getGoalPercept().getNumberOfSeenPosts() > 2) {
    // what do do here?
  }

  // TODO: check this
  getGoalPercept().horizonScan = true;
}//end execute


void GoalDetectorV2::clusterEdgelFeatures()
{
  std::vector<GoalBarFeature> pairs;
  for(size_t scanIdOne = 0; scanIdOne + 1 < getGoalFeaturePercept().features.size(); scanIdOne++) 
  {
    const std::vector<GoalBarFeature>& scanlineOne = getGoalFeaturePercept().features[scanIdOne];
    for(size_t i = 0; i < scanlineOne.size(); i++) 
    {
      const GoalBarFeature& e1 = scanlineOne[i];
      size_t scanIdTwo = scanIdOne+1;
      for(size_t j = 0; j < getGoalFeaturePercept().features[scanIdTwo].size(); j++)
      {
        const GoalBarFeature& e2 = getGoalFeaturePercept().features[scanIdTwo][j];

        if(e1.sim(e2) > params.thresholdFeatureSimilarity) 
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

    if(max_sim > params.thresholdFeatureSimilarity) {
      clusters[cluster_idx].add(e1);
    } else {
      Cluster cluster;
      cluster.add(e1);
      clusters.push_back(cluster);
    }
  }
}

void GoalDetectorV2::calcuateGoalPosts()
{
  bool markValidPosts = false; 
  bool markInvalidPosts = false; 
  DEBUG_REQUEST("Vision:GoalDetectorV2:markValidPosts",  
    markValidPosts = true;   
  );
  DEBUG_REQUEST("Vision:GoalDetectorV2:markInvalidPosts",  
    markInvalidPosts = true;   
  );


  for(size_t j = 0; j < clusters.size(); j++) 
  {
    Cluster& cluster = clusters[j];
      
    if(cluster.size() >= params.minGoodPoints) 
    {
      Math::Line line = cluster.getLine();

      GoalPercept::GoalPost post;
      post.directionInImage = line.getDirection();
      post.basePoint = scanForEndPoint(line.getBase(), post.directionInImage);
      //HACK: inside polygon test seems to fail on that image border
      if(post.basePoint.y >= (int) getImage().height() - 1) post.basePoint.y--;
      post.topPoint = scanForEndPoint(line.getBase(), -post.directionInImage);
      post.positionReliable = getFieldPercept().getValidField().isInside(post.basePoint);
      post.seenWidth = cluster.getFeatureWidth();
      post.seenHeight = (post.basePoint - post.topPoint).abs();

      // NOTE: if the projection is not successfull, then post.position = (0,0)
      bool projectionOk = CameraGeometry::imagePixelToFieldCoord(
          getCameraMatrix(), getImage().cameraInfo,
          post.basePoint.x, post.basePoint.y, 0.0,
          post.position);

      post.positionReliable = post.positionReliable && projectionOk;

      double fieldDiagonal = sqrt(getFieldInfo().xFieldLength * getFieldInfo().xFieldLength + getFieldInfo().yFieldLength * getFieldInfo().yFieldLength);

      if(projectionOk) 
      {
        if(post.position.abs() > fieldDiagonal)
        {
          post.positionReliable = false;
        }
        else
        {
          Vector2d base = getBackProjectedBasePoint(post); 
          Vector2d top = getBackProjectedTopPoint(post); 
          //double baseDiff = (post.basePoint - base).abs();
          //double topDiff = (post.topPoint - top).abs();
          double baseWidth = getBackProjectedBaseBarWidth(post);
          double topWidth = getBackProjectedTopBarWidth(post);
          double meanProjectedWidth = (topWidth + baseWidth) * 0.5;            
          double widthRatio = meanProjectedWidth > post.seenWidth ? meanProjectedWidth / post.seenWidth : post.seenWidth / meanProjectedWidth;
          ColorClasses::Color col;
      
          DEBUG_REQUEST("Vision:GoalDetectorV2:markPostCenterBackProjected",  
            if(post.positionReliable)
            {
              col = ColorClasses::green;
            }
            else
            {
              col = ColorClasses::red;
            }

            LINE_PX(col, (int)(base.x+0.5), (int)(base.y+0.5), (int)(top.x+0.5), (int)(top.y+0.5));
          );

          DEBUG_REQUEST("Vision:GoalDetectorV2:markPostBackProjected",  
            Vector2i begin;
            Vector2i end;
            begin = base;
            end = top;
            Vector2d postNorm = Vector2d(top - base).normalize().rotateRight();
            Vector2d postTopNorm = postNorm * topWidth * 0.5;
            Vector2d postBaseNorm = postNorm * baseWidth * 0.5;

            Vector2i beginL = base - postBaseNorm;
            Vector2i beginR = base + postBaseNorm;
            Vector2i endL = top - postTopNorm;
            Vector2i endR = top + postTopNorm;
            ColorClasses::Color col = ColorClasses::skyblue;
            if(!post.positionReliable)
            {
              col = ColorClasses::orange;
            }
            else 
            {
              if(widthRatio > params.maxBarWidthRatio) col = ColorClasses::pink;
            }
            if( (markValidPosts && col == ColorClasses::skyblue) || (markInvalidPosts && col != ColorClasses::skyblue) )
            {
              LINE_PX(col, beginL.x, beginL.y, beginR.x, beginR.y);
              LINE_PX(col, beginL.x, beginL.y, endL.x, endL.y);
              LINE_PX(col, beginR.x, beginR.y, endR.x, endR.y);
              LINE_PX(col, endL.x, endL.y, endR.x, endR.y);
            }
          );

          DEBUG_REQUEST("Vision:GoalDetectorV2:markPostCenterLine",  
            Vector2i begin = post.basePoint;
            Vector2i end = post.topPoint;
            LINE_PX(ColorClasses::blue, (int)(begin.x+0.5), (int)(begin.y+0.5), (int)(end.x+0.5), (int)(end.y+0.5));
          );

          DEBUG_REQUEST("Vision:GoalDetectorV2:markPosts",  
            Vector2i begin;
            Vector2i end;
            begin = post.basePoint;
            end = post.topPoint;
            Vector2d postNorm = Vector2d(post.topPoint - post.basePoint).normalize().rotateRight() * post.seenWidth * 0.5;

            Vector2i beginL = post.basePoint - postNorm;
            Vector2i beginR = post.basePoint + postNorm;
            Vector2i endL = post.topPoint - postNorm;
            Vector2i endR = post.topPoint + postNorm;
            ColorClasses::Color col = ColorClasses::green;
            if(!post.positionReliable) col = ColorClasses::red;
            if(widthRatio > params.maxBarWidthRatio) col = ColorClasses::blue;
            if( (markValidPosts && col == ColorClasses::green) || (markInvalidPosts && col != ColorClasses::green) )
            {
              LINE_PX(col, beginL.x, beginL.y, beginR.x, beginR.y);
              LINE_PX(col, beginL.x, beginL.y, endL.x, endL.y);
              LINE_PX(col, beginR.x, beginR.y, endR.x, endR.y);
              LINE_PX(col, endL.x, endL.y, endR.x, endR.y);
            }
          );
        
          post.positionReliable &= widthRatio < params.maxBarWidthRatio;
        }
      }
      getGoalPercept().add(post);
    }
  }
}

Vector2i GoalDetectorV2::scanForEndPoint(const Vector2i& start, const Vector2d& direction) const
{
  Vector2i pos(start);
  Pixel pixel;
  BresenhamLineScan footPointScanner(pos, direction, getImage().cameraInfo);

  // initialize the scanner
  Vector2i peak_point(pos);
  MaximumScan<Vector2i,double> maxScan(peak_point, params.thresholdGradient);
  Filter<Diff5x1, Vector2i, double, 5> filter;

  while(footPointScanner.getNextWithCheck(pos))
  {
    IMG_GET(pos.x, pos.y, pixel);
    int pixValue = params.detectWhiteGoals ? pixel.y : (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));
    filter.add(pos, pixValue);

    //collect some values for statisics of colors
    getGoalPostHistograms().increaseChannelValue(pixel);

    DEBUG_REQUEST("Vision:GoalDetectorV2:markPostScans", 
      if(filter.ready()) {
        POINT_PX(ColorClasses::pink, filter.point().x, filter.point().y);
      }
    );

    if(filter.ready())
    {
      double absValue = fabs(filter.value());
      if( maxScan.add(filter.point(), absValue) || pixValue < params.threshold) 
      {
        break; 
      }
    }
  }

  DEBUG_REQUEST("Vision:GoalDetectorV2:markPostScans",
    CIRCLE_PX(ColorClasses::blue, (int) pos.x, (int) pos.y, 10);
  );

  return pos;
}


void GoalDetectorV2::debugStuff(CameraInfo::CameraID camID)
{
  cameraID = camID;
  DEBUG_REQUEST("Vision:GoalDetectorV2:showColorByHistogramBottom",
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
  DEBUG_REQUEST("Vision:GoalDetectorV2:showColorByHistogramTop",
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

Vector2d GoalDetectorV2::getBackProjectedTopPoint(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  Vector3d pt1(p1.x, p1.y, getFieldInfo().goalHeight);

  Vector2i topInImage1;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 
  return topInImage1;
}

Vector2d GoalDetectorV2::getBackProjectedBasePoint(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  Vector3d pt1(p1.x, p1.y, 0.0);

  Vector2i topInImage1;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 
  return topInImage1;
}

int GoalDetectorV2::getBackProjectedTopBarWidth(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  double halfWidth = getFieldInfo().goalpostRadius;
  Vector3d pt1(p1.x, p1.y - halfWidth, getFieldInfo().goalHeight);
  Vector3d pt2(p1.x, p1.y + halfWidth, getFieldInfo().goalHeight);

  Vector2i topInImage1;
  Vector2i topInImage2;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt2,topInImage2);

  return (topInImage1 - topInImage2).abs();
}

int GoalDetectorV2::getBackProjectedBaseBarWidth(const GoalPercept::GoalPost& post)
{
  Vector2d p1 = post.position;
  double halfWidth = getFieldInfo().goalpostRadius;
  Vector3d pt1(p1.x, p1.y - halfWidth, 0.0);
  Vector3d pt2(p1.x, p1.y + halfWidth, 0.0);

  Vector2i topInImage1;
  Vector2i topInImage2;
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt1,topInImage1); 
  CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo, pt2,topInImage2);

  return (topInImage1 - topInImage2).abs();
}

int GoalDetectorV2::getBackProjectedPostHeight(const GoalPercept::GoalPost& post)
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


