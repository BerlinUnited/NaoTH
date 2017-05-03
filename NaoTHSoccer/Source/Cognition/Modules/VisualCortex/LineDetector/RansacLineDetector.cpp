#include "RansacLineDetector.h"

RansacLineDetector::RansacLineDetector()
{
  // initialize some stuff here
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:edgel_pairs", "", false);
}

RansacLineDetector::~RansacLineDetector()
{
  // clean some stuff here
}

void RansacLineDetector::execute()
{
  // do some stuff here

  DEBUG_REQUEST("Vision:RansacLineDetector:edgel_pairs",
    CANVAS("ImageTop");

    for(size_t i = 0; i < getScanLineEdgelPerceptTop().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPerceptTop().pairs[i];
      const Edgel& edgelOne = getScanLineEdgelPerceptTop().edgels[pair.begin];
      const Edgel& edgelTwo = getScanLineEdgelPerceptTop().edgels[pair.end];

      PEN("000000",0.1);
      //LINE_PX(ColorClasses::red, edgelOne.center.x, edgelOne.center.y, edgelTwo.center.x, edgelTwo.center.y);
      LINE(edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
      PEN("FF0000",0.1);
      CIRCLE( edgelOne.point.x, edgelOne.point.y, 3);
      PEN("0000FF",0.1);
      CIRCLE( edgelTwo.point.x, edgelTwo.point.y, 2);
    }
  );


  // copy the edgels
  // todo: can this be optimized?
  //outliers.assign(getLineGraphPercept().edgels.begin(), getLineGraphPercept().edgels.end());
  outliers.resize(getLineGraphPercept().edgels.size());
  for(size_t i = 0; i < getLineGraphPercept().edgels.size(); ++i) {
    outliers[i] = i;
  }
  
  for(int i = 0; i < 10; ++i)
  {
    Math::Line result;
    if(ransac(result) > 0) 
    {
      Vector2d a = result.point(-10000);
      Vector2d b = result.point(10000);
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 30);
      LINE(a.x,a.y,b.x,b.y);
    } 
    else {
      break;
    }
  }
  
}

int RansacLineDetector::ransac(Math::Line& result)
{
  int iterations = 20;
  double outlierThreshold = 100;
  int inlierMin = 10;

  if(outliers.size() <= 2) {
    return 0;
  }

  Math::Line bestModel;
  int bestInlier = 0;

  for(int i = 0; i < iterations; ++i)
  {
    //pick two random points
    int i0 = Math::random((int)outliers.size());
    int i1 = Math::random((int)outliers.size());

    if(i0 == i1) {
      continue;
    }

    const Edgel& a = getLineGraphPercept().edgels[outliers[i0]];
    const Edgel& b = getLineGraphPercept().edgels[outliers[i1]];

    // check the orientation
    // TODO

    Math::Line model(a.point, b.point-a.point);

    int inlier = 0;
    for(size_t i: outliers) 
    {
      const Edgel& e = getLineGraphPercept().edgels[i];
      double d = model.minDistance(e.point);

      // inlier
      if(d < outlierThreshold) {
        ++inlier;
      }
    }


    if(inlier > inlierMin && inlier > bestInlier) {
      bestModel = model;
      bestInlier = inlier;
    }
  }

  // update the outliers
  // todo: make it faster
  std::vector<size_t> newOutliers;
  newOutliers.reserve(outliers.size() - bestInlier + 1);

  for(size_t i: outliers) 
  {
    const Edgel& e = getLineGraphPercept().edgels[i];
    double d = bestModel.minDistance(e.point);
    if(d >= outlierThreshold) {
        newOutliers.push_back(i);
      }
  }
  outliers = newOutliers;

  // return results
  result = bestModel;
  return bestInlier;
}


