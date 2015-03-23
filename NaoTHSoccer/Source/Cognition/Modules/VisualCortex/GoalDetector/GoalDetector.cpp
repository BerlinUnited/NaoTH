
/**
* @file GoalDetector.cpp
*
* Implementation of class GoalDetector
*
*/

#include "GoalDetector.h"

#include "Tools/CameraGeometry.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/Filter.h"
#include "Tools/ImageProcessing/MaximumScan.h"

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


GoalDetector::GoalDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:GoalDetector:markPostScans","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetector:markPosts","..", false);

  DEBUG_REQUEST_REGISTER("Vision:GoalDetector:showColorByHistogramBottom","..", false);
  DEBUG_REQUEST_REGISTER("Vision:GoalDetector:showColorByHistogramTop","..", false);

  getDebugParameterList().add(&params);
}

GoalDetector::~GoalDetector()
{
  getDebugParameterList().remove(&params);
}

void GoalDetector::execute(CameraInfo::CameraID id)
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


void GoalDetector::clusterEdgelFeatures()
{
  std::vector<GoalBarFeature> pairs;
  for(size_t scanIdOne = 0; scanIdOne + 1 < getGoalFeaturePercept().features.size(); scanIdOne++) 
  {
    const std::vector<GoalBarFeature>& scanlineOne = getGoalFeaturePercept().features[scanIdOne];
    for(size_t i = 0; i < scanlineOne.size(); i++) 
    {
      const GoalBarFeature& e1 = scanlineOne[i];

      size_t scanIdTwo = scanIdOne+1;
        for(size_t j = 0; j < getGoalFeaturePercept().features[scanIdTwo].size(); j++) {
          const GoalBarFeature& e2 = getGoalFeaturePercept().features[scanIdTwo][j];

          if(e1.sim(e2) > params.thresholdFeatureSimilarity) {
            //LINE_PX(ColorClasses::red, (int)(e1.point.x+0.5), (int)(e1.point.y+0.5), (int)(e2.point.x+0.5), (int)(e2.point.y+0.5));

            GoalBarFeature pair;
            pair.point = (e1.point + e2.point)*0.5;
            pair.direction = (e2.direction + e1.direction).normalize();
            pair.width = (e2.width + e1.width)*0.5;
            pairs.push_back(pair);
          }
        }
      //}
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

void GoalDetector::calcuateGoalPosts()
{
  for(size_t j = 0; j < clusters.size(); j++) 
  {
    Cluster& cluster = clusters[j];
      
    if(cluster.size() > params.minGoodPoints) 
    {
      Math::Line line = cluster.getLine();

      GoalPercept::GoalPost post;
      post.directionInImage = line.getDirection();
      post.basePoint = scanForEndPoint(line.getBase(), post.directionInImage);
      post.topPoint = scanForEndPoint(line.getBase(), -post.directionInImage);
      post.positionReliable = getFieldPercept().getValidField().isInside(post.basePoint);

      // NOTE: if the projection is not successfull, then post.position = (0,0)
      bool projectionOk = CameraGeometry::imagePixelToFieldCoord(
          getCameraMatrix(), getImage().cameraInfo,
          post.basePoint.x, post.basePoint.y, 0.0,
          post.position);

      post.positionReliable = post.positionReliable && projectionOk;
      post.seenWidth = cluster.getFeatureWidth();
      post.seenHeight = (post.basePoint - post.topPoint).abs();

      DEBUG_REQUEST("Vision:GoalDetector:markPosts",  
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

        LINE_PX(col, beginL.x, beginL.y, beginR.x, beginR.y);
        LINE_PX(col, beginL.x, beginL.y, endL.x, endL.y);
        LINE_PX(col, beginR.x, beginR.y, endR.x, endR.y);
        LINE_PX(col, endL.x, endL.y, endR.x, endR.y);
      );
      getGoalPercept().add(post);
    }
  }
}

Vector2i GoalDetector::scanForEndPoint(const Vector2i& start, const Vector2d& direction) const
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

    DEBUG_REQUEST("Vision:GoalDetector:markPostScans", 
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

  DEBUG_REQUEST("Vision:GoalDetector:markPostScans",
    CIRCLE_PX(ColorClasses::blue, (int) pos.x, (int) pos.y, 10);
  );

  return pos;
}


void GoalDetector::debugStuff(CameraInfo::CameraID camID)
{
  cameraID = camID;
  DEBUG_REQUEST("Vision:GoalDetector:showColorByHistogramBottom",
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
  DEBUG_REQUEST("Vision:GoalDetector:showColorByHistogramTop",
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
