#include "RansacLineDetector.h"

RansacLineDetector::RansacLineDetector()
{
  // initialize some stuff here
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_edgels_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_lines_field", "", false);

  getDebugParameterList().add(&params);
}

RansacLineDetector::~RansacLineDetector()
{
  getDebugParameterList().remove(&params);
}

void RansacLineDetector::execute()
{
  getLinePercept().reset();


  DEBUG_REQUEST("Vision:RansacLineDetector:draw_edgels_field",
    FIELD_DRAWING_CONTEXT;

    for(const Edgel& e: getLineGraphPercept().edgels)
    {
      PEN("FF0000",2);
      CIRCLE(e.point.x, e.point.y, 25);
      PEN("000000",0.1);
      LINE(e.point.x, e.point.y, e.point.x + e.direction.x*100.0, e.point.y + e.direction.y*100.0);
    }
  );


  // copy the edgels
  // todo: can this be optimized?
  //outliers.assign(getLineGraphPercept().edgels.begin(), getLineGraphPercept().edgels.end());
  outliers.resize(getLineGraphPercept().edgels.size());
  for(size_t i = 0; i < getLineGraphPercept().edgels.size(); ++i) {
    outliers[i] = i;
  }
  
  for(int i = 0; i < 11; ++i)
  {
    Math::LineSegment result;
    if(ransac(result) > 0) 
    {
      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePercept().lines.push_back(fieldLine);
    } 
    else {
      break;
    }
  }
  

  DEBUG_REQUEST("Vision:RansacLineDetector:draw_lines_field",
    FIELD_DRAWING_CONTEXT;

    for(const LinePercept::FieldLineSegment& line: getLinePercept().lines)
    {
      PEN("999999", 50);
      LINE(
        line.lineOnField.begin().x, line.lineOnField.begin().y,
        line.lineOnField.end().x, line.lineOnField.end().y);
    }
  );

}

int RansacLineDetector::ransac(Math::LineSegment& result)
{
  if(outliers.size() <= 2) {
    return 0;
  }

  Math::Line bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  for(int i = 0; i < params.iterations; ++i)
  {
    //pick two random points
    int i0 = Math::random((int)outliers.size());
    int i1 = Math::random((int)outliers.size());

    if(i0 == i1) {
      continue;
    }

    const Edgel& a = getLineGraphPercept().edgels[outliers[i0]];
    const Edgel& b = getLineGraphPercept().edgels[outliers[i1]];

    //double x = a.sim(b);
    if(a.sim(b) < params.directionSimilarity) {
      continue;
    }

    // check the orientation
    // TODO

    Math::Line model(a.point, b.point-a.point);

    double inlierError = 0;
    int inlier = 0;
    for(size_t i: outliers) 
    {
      const Edgel& e = getLineGraphPercept().edgels[i];
      double d = model.minDistance(e.point);

      // inlier
      if(d < params.outlierThreshold && sim(model, e) > params.directionSimilarity) {
        ++inlier;
        inlierError += d;
      }
    }


    if(inlier >= params.inlierMin && (inlier > bestInlier || (inlier == bestInlier && inlierError < bestInlierError))) {
      bestModel = model;
      bestInlier = inlier;
      bestInlierError = inlierError;
    }
  }


  // update the outliers
  // todo: make it faster
  std::vector<size_t> newOutliers;
  newOutliers.reserve(outliers.size() - bestInlier + 1);
  double minT = 0;
  double maxT = 0;

  for(size_t i: outliers) 
  {
    const Edgel& e = getLineGraphPercept().edgels[i];
    double d = bestModel.minDistance(e.point);

    if(d < params.outlierThreshold && sim(bestModel, e) > params.directionSimilarity) {
      double t = bestModel.project(e.point);
      minT = std::min(t, minT);
      maxT = std::max(t, maxT);
    } else {
      newOutliers.push_back(i);
    }
  }
  outliers = newOutliers;

  // return results
  result = Math::LineSegment(bestModel.point(minT), bestModel.point(maxT));
  return bestInlier;
}


