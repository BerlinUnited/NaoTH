
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
#include "Tools/ImageProcessing/Filter.h"

#define IMG_GET(x,y,p) \
  if(!getImage().isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  getImage().get(x, y, p);


NewGoalDetector::NewGoalDetector()
: 
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:NewGoalDetector:markPostScans","..", false);

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

  clusters.clear();
  clusterEdgelFeatures();

  for(size_t j = 0; j < clusters.size(); j++) 
  {
    Cluster& cluster = clusters[j];
      
    if(cluster.size() > params.minGoodPoints) 
    {
      Math::Line line = cluster.getLine();

      Vector2d begin, end;
      begin = line.getBase();
      end = line.getBase() + line.getDirection()*50;
      LINE_PX(ColorClasses::red, (int)(begin.x+0.5), (int)(begin.y+0.5), (int)(end.x+0.5), (int)(end.y+0.5));

      GoalPercept::GoalPost post;
      post.directionInImage = line.getDirection();
      post.basePoint = scanForEndPoint(line.getBase(), post.directionInImage);
      post.topPoint = scanForEndPoint(line.getBase(), -post.directionInImage);
      post.positionReliable = getFieldPercept().getValidField().isInside(post.basePoint);
    }
  }

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
  for(size_t scanIdOne = 0; scanIdOne + 1 < getGoalFeaturePercept().edgel_features.size(); scanIdOne++) 
  {
    const std::vector<EdgelT<double> >& scanlineOne = getGoalFeaturePercept().edgel_features[scanIdOne];
    for(size_t i = 0; i < scanlineOne.size(); i++) 
    {
      const EdgelT<double>& e1 = scanlineOne[i];

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

  for(size_t i = 0; i < pairs.size(); i++) 
  {
    const EdgelT<double>& e1 = pairs[i];

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

    if(max_sim > t_sim) {
      clusters[cluster_idx].add(e1);
    } else {
      Cluster cluster;
      cluster.add(e1);
      clusters.push_back(cluster);
    }
  }
}


Vector2i NewGoalDetector::scanForEndPoint(const Vector2i& start, const Vector2d& direction) const
{
  Vector2i pos(start);
  Pixel pixel;
  BresenhamLineScan footPointScanner(pos, direction, getImage().cameraInfo);

  Filter<Gauss,Vector2i,double> filter;

  while(footPointScanner.getNextWithCheck(pos))
  {
    IMG_GET(pos.x, pos.y, pixel);
    int diffVU = (int) pixel.v - (int) pixel.u;
    filter.add(pos, diffVU);

    //collect some values for statisics of colors
    getGoalPostHistograms().increaseChannelValue(pixel);

    DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markPostScans", 
      if(filter.ready()) {
        POINT_PX(ColorClasses::pink, filter.point().x, filter.point().y);
      }
    );

    if(filter.ready() && filter.value() < params.thresholdUV) {
      break; 
    }
  }

  DEBUG_REQUEST("Vision:Detectors:NewGoalDetector:markPostScans",
    CIRCLE_PX(ColorClasses::blue, (int) pos.x, (int) pos.y, 10);
  );

  return pos;
}


