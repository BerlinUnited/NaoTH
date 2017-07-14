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

      if(e.point.x > 0 || e.point.y > 0) {
        CIRCLE(e.point.x, e.point.y, 25);
      }
      PEN("000000",0.1);
      LINE(e.point.x, e.point.y, e.point.x + e.direction.x*100.0, e.point.y + e.direction.y*100.0);
    }
    // draw coedgels
    /*
    for(const CoEdgels& co: getLineGraphPercept().coEdgels)
    {
      PEN("0000FF",2);

      if(co.left.point.x > 0 || co.left.point.y > 0) {
        CIRCLE(co.left.point.x, co.left.point.y, 25);
      }

      if(co.right.point.x > 0 || co.right.point.y > 0) {
        CIRCLE(co.right.point.x, co.right.point.y, 25);
      }
    }
    */
  );


  // copy the edgels
  // todo: can this be optimized?
  //outliers.assign(getLineGraphPercept().edgels.begin(), getLineGraphPercept().edgels.end());
  outliers.resize(getLineGraphPercept().edgels.size());

  for(size_t i = 0; i < getLineGraphPercept().edgels.size(); ++i) {
    outliers[i] = i;
  }

  bool foundLines = false;
  int bestInlierCirc = 0;

  Ellipse circResult;

  for(int i = 0; i < 11; ++i)
  {
    Math::LineSegment result;
    if(ransac(result) > 0)
    {
      foundLines = true;
      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePercept().lines.push_back(fieldLine);
    } 
    else {
      // Check for circle
      if (params.fit_circle) {
        bestInlierCirc = ransacEllipse(circResult);
      }
      break;
    }
  }

  DEBUG_REQUEST("Vision:RansacLineDetector:draw_lines_field",
    FIELD_DRAWING_CONTEXT;

    if (foundLines) {
      for(const LinePercept::FieldLineSegment& line: getLinePercept().lines)
      {
        PEN("999999", 50);
        LINE(
          line.lineOnField.begin().x, line.lineOnField.begin().y,
          line.lineOnField.end().x, line.lineOnField.end().y);
      }
    }

    if (bestInlierCirc > 0) {
      double c[2];
      circResult.getCenter(c);

      double a[2];
      circResult.axesLength(a);

      PEN("009900", 50);

      CIRCLE(c[0], c[1], 30);
      OVAL_ROTATED(c[0], c[1], a[0], a[1], circResult.rotationAngle());

      PEN("0000AA", 20);
      for(int i=0; i<circResult.x_toFit.size(); i++) {
        CIRCLE(circResult.x_toFit[i], circResult.y_toFit[i], 20);
      }
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

  if(bestInlier > 2) {
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
  }

  return bestInlier;
}

int RansacLineDetector::ransacEllipse(Ellipse& result)
{

  if(outliers.size() <= 5) {
    return 0;
  }

  Ellipse bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  for(int i = 0; i < params.circle_iterations; ++i)
  {
    // create model
    Ellipse ellipse;

    std::vector<double> x, y;
    x.reserve(5);
    y.reserve(5);
    //double x[5], y[5];
    for(int t=0; t<5; t++) {
      size_t r = swap_random(outliers, (int) outliers.size()-(t+1));
      const Edgel& e = getLineGraphPercept().edgels[r];

      x.push_back(e.point.x);
      y.push_back(e.point.y);
    }

    ellipse.fitPoints(x,y);

    // check model
    double inlierError = 0;
    int inlier = 0;

    for(size_t i: outliers)
    {
      const Edgel& e = getLineGraphPercept().edgels[i];
      double d = ellipse.error_to(e.point.x, e.point.y);

      if(d <= params.circle_outlierThreshold) {
        ++inlier;
        inlierError += d;
        continue;
      }
    }
    if(inlier >= params.circle_inlierMin && (inlier > bestInlier || (inlier == bestInlier && inlierError < bestInlierError))) {
      bestModel = ellipse;
      bestInlier = inlier;
      bestInlierError = inlierError;
    }
  }

  if (bestInlier > 5) {
    // update the outliers
    // todo: make it faster
    std::vector<size_t> newOutliers;
    newOutliers.reserve(outliers.size() - bestInlier + 1);

    std::vector<double> inliers_x, inliers_y;
    inliers_x.reserve(bestInlier);
    inliers_y.reserve(bestInlier);

    for(size_t i: outliers)
    {
      const Edgel& e = getLineGraphPercept().edgels[i];
      double d = bestModel.error_to(e.point.x, e.point.y);

      if(d > params.circle_outlierThreshold) {
        newOutliers.push_back(i);
      } else {
        inliers_x.push_back(e.point.x);
        inliers_y.push_back(e.point.y);
      }
    }
    outliers = newOutliers;

    // return results
    bestModel.fitPoints(inliers_x, inliers_y);
    result = bestModel;
  }

  return bestInlier;

}
