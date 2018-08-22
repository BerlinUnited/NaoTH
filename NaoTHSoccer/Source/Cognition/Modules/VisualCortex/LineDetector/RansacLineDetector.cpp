#include "RansacLineDetector.h"

#include "Tools/Math/Geometry.h"

RansacLineDetector::RansacLineDetector()
{
  // initialize some stuff here
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_edgels_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_lines_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:fit_and_draw_ellipse_field", "", false);

  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_circle_field", "", false);

  getDebugParameterList().add(&params);
}

RansacLineDetector::~RansacLineDetector()
{
  getDebugParameterList().remove(&params);
}

void RansacLineDetector::execute()
{
  getLinePercept().reset();
  getRansacCirclePercept().reset();
  getLinePercept().middleCircleWasSeen = false;
  outliers.resize(getLineGraphPercept().edgelsOnField.size());
  getLinePercept().edgelLineIDs.resize(getLineGraphPercept().edgelsOnField.size());
  // copy the edgels
  for(size_t i = 0; i < getLineGraphPercept().edgelsOnField.size(); ++i) {
    outliers[i] = i;
    getLinePercept().edgelLineIDs[i] = -1;
  }
  std::vector<size_t> inliers;
  for(int i = 0; i < params.maxLines; ++i)
  {
    inliers.clear();
    Math::LineSegment result;
    size_t start_edgel, end_edgel;
    if(ransac(result, inliers, start_edgel, end_edgel))
    {
      if(params.fit_lines_to_inliers) {
        std::vector<Vector2d> points;
        points.reserve(inliers.size());
        for(size_t i : inliers) {
          const Edgel& e = getLineGraphPercept().edgelsOnField[i];
          points.push_back(e.point);
        }

        double slope, intercept;
        simpleLinearRegression(points, slope, intercept);

        Vector2d base(0, intercept);
        Vector2d direction = base - Vector2d(1, slope + intercept);
        Math::Line regression_line(base, direction);

        Math::LineSegment regression_result(
          regression_line.projection(getLineGraphPercept().edgelsOnField[start_edgel].point),
          regression_line.projection(getLineGraphPercept().edgelsOnField[end_edgel].point)
        );

        result = regression_result;
      }

      LinePercept::FieldLineSegment fieldLine;
      fieldLine.lineOnField = result;
      getLinePercept().lines.push_back(fieldLine);

      for(size_t i : inliers) {
        getLinePercept().edgelLineIDs[i] = static_cast<int>(getLinePercept().lines.size()-1);
      }
    }
    else {
      break;
    }
  }

  std::vector<Vector2d> inlierPoints;
  // circle
  if(params.enable_circle_ransac) {
    inlierPoints.clear();
    Vector2d circResult;
    if (ransacCircle(circResult, inlierPoints)) {

      DEBUG_REQUEST("Vision:RansacLineDetector:draw_circle_field",
        FIELD_DRAWING_CONTEXT;
        PEN("000099", 5);
        for(Vector2d inlier : inlierPoints)
        {
          CIRCLE(inlier.x, inlier.y, 30);
        }
        PEN("990000", 10);
        CIRCLE(circResult.x, circResult.y, getFieldInfo().centerCircleRadius);
      );

      if(params.fit_circle_to_inliers) {
        Vector2d circleMean;
        double circleRadius;
        if (Geometry::calculateCircle(inlierPoints, circleMean, circleRadius)) {
          getRansacCirclePercept().middleCircleCenter = circleMean;
          getRansacCirclePercept().middleCircleWasSeen = true;

          DEBUG_REQUEST("Vision:RansacLineDetector:draw_circle_field",
            FIELD_DRAWING_CONTEXT;
            PEN("009900", 10);
            CIRCLE(circleMean.x, circleMean.y, circleRadius);
          );
        }
      } else {
        getRansacCirclePercept().middleCircleCenter = circResult;
        getRansacCirclePercept().middleCircleWasSeen = true;
      }
    }

  }

  DEBUG_REQUEST("Vision:RansacLineDetector:draw_edgels_field",
    FIELD_DRAWING_CONTEXT;

    for(size_t i=0; i<getLineGraphPercept().edgelsOnField.size(); i++)
    {
      const Edgel& e = getLineGraphPercept().edgelsOnField[i];

      if(getLinePercept().edgelLineIDs[i] > -1) {
        std::string color;
        switch(getLinePercept().edgelLineIDs[i]%3) {
          case 0: color = "00FF00"; break;
          case 1: color = "0000FF"; break;
          default: color = "00FFFF"; break;
        }
        PEN(color,2);
      } else {
        PEN("FF0000",2);
      }

      if(e.point.x > 0 || e.point.y > 0) {
        CIRCLE(e.point.x, e.point.y, 25);
      }
      PEN("000000",0.1);
      LINE(e.point.x, e.point.y, e.point.x + e.direction.x*100.0, e.point.y + e.direction.y*100.0);
    }

  );

  DEBUG_REQUEST("Vision:RansacLineDetector:draw_lines_field",
    FIELD_DRAWING_CONTEXT;
    for(size_t i=0; i<getLinePercept().lines.size(); i++)
    {
      std::string color;
      switch(i%3) {
        case 0: color = "00FF00"; break;
        case 1: color = "0000FF"; break;
        default: color = "00FFFF"; break;
      }
      PEN(color, 50);
      LINE(
        getLinePercept().lines[i].lineOnField.begin().x, getLinePercept().lines[i].lineOnField.begin().y,
        getLinePercept().lines[i].lineOnField.end().x, getLinePercept().lines[i].lineOnField.end().y);
    }

  );

  DEBUG_REQUEST("Vision:RansacLineDetector:fit_and_draw_ellipse_field",
    FIELD_DRAWING_CONTEXT;
    // fit ellipse
    Ellipse ellipseResult;

    int bestInlierCirc = ransacEllipse(ellipseResult);

    if (bestInlierCirc > 0) {
      {

        double c[2];
        ellipseResult.getCenter(c);

        double a[2];
        ellipseResult.axesLength(a);

        PEN("009900", 50);

        //CIRCLE(c[0], c[1], 30);
        OVAL_ROTATED(c[0], c[1], a[0], a[1], ellipseResult.rotationAngle());

        PEN("0000AA", 20);
        for(size_t i=0; i<ellipseResult.x_toFit.size(); i++) {
          CIRCLE(ellipseResult.x_toFit[i], ellipseResult.y_toFit[i], 20);
        }
      }

      {
        Vector2d c = ellipseResult.getCenter();
        Vector2d a = ellipseResult.axesLength();

        Vector2d ax(a.x,0.0);
        ax.rotate(ellipseResult.rotationAngle());

        Vector2d ay(0.0,a.y);
        ay.rotate(ellipseResult.rotationAngle());

        PEN("FF0000", 20);
        CIRCLE(c.x, c.y, 50);
        LINE(c.x, c.y, c.x+ax.x, c.y+ax.y);
        LINE(c.x, c.y, c.x+ay.x, c.y+ay.y);


        Vector2d point(500,500);
        Vector2d p = ellipseResult.test_project(point);
        PEN("FF0000", 20);
        CIRCLE(point.x, point.y, 50);
        PEN("0000FF", 20);
        CIRCLE(p.x, p.y, 50);
        PEN("000000", 10);
        LINE(point.x, point.y, p.x, p.y);
    }
    }
  );


}

bool RansacLineDetector::ransac(Math::LineSegment& result, std::vector<size_t>& inliers, size_t& start_edgel, size_t& end_edgel)
{
  if(outliers.size() <= 2) {
    return false;
  }

  Math::Line bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  for(int i = 0; i < params.iterations; ++i)
  {
    //pick two random points without replacement
    size_t i0 = choose_random_from(outliers, 1);
    size_t i1 = choose_random_from(outliers, 2);

    const Edgel& a = getLineGraphPercept().edgelsOnField[outliers[i0]];
    const Edgel& b = getLineGraphPercept().edgelsOnField[outliers[i1]];

    if(a.sim(b) < params.directionSimilarity) {
      continue;
    }

    Math::Line model(a.point, b.point-a.point);

    double inlierError = 0;
    int inlier = 0;
    for(size_t i: outliers)
    {
      const Edgel& e = getLineGraphPercept().edgelsOnField[i];
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

  if(bestInlier >= params.inlierMin)
  {
    std::vector<size_t> newOutliers;
    newOutliers.reserve(outliers.size() - bestInlier + 1);
    inliers.reserve(bestInlier);

    double minT = bestModel.project(getLineGraphPercept().edgelsOnField[0].point);
    double maxT = minT;
    start_edgel = end_edgel = 0;

    Vector2d direction_var;

    for(size_t i: outliers)
    {
      const Edgel& e = getLineGraphPercept().edgelsOnField[i];
      double d = bestModel.minDistance(e.point);

      if(d < params.outlierThreshold && sim(bestModel, e) > params.directionSimilarity) {
        inliers.push_back(i);

        double t = bestModel.project(e.point);
        if (t < minT) {
          minT = t;
          start_edgel = i;
        }
        if (t > maxT) {
          maxT = t;
          end_edgel = i;
        }
        double ang = e.direction.angle();
        direction_var.x += cos(ang);
        direction_var.y += sin(ang);
      } else {
        newOutliers.push_back(i);
      }
    }

    direction_var /= static_cast<int>(inliers.size());
    double angle_var = 1 - sqrt(Math::sqr(direction_var.x) + Math::sqr(direction_var.y));

    result = Math::LineSegment(bestModel.point(minT), bestModel.point(maxT));
    double line_length = result.getLength();

    if (line_length > params.min_line_length
        && (line_length > params.length_of_var_check || angle_var <= params.maxVariance)) {

      outliers = newOutliers;
      return true;
    }

  }
  return false;
}

bool RansacLineDetector::ransacCircle(Vector2d& result, std::vector<Vector2d>& inlierPoints) {
  if(outliers.size() <= 2) {
    return false;
  }

  double radius = getFieldInfo().centerCircleRadius;

  Vector2d bestModel;

  int bestInlier = 0;
  double bestInlierError = 0;

  for(int i = 0; i < params.circle_iterations; ++i)
  {
    //pick two random points without replacement
    size_t i0 = choose_random_from(outliers, 1);
    size_t i1 = choose_random_from(outliers, 2);

    const Edgel& a = getLineGraphPercept().edgelsOnField[outliers[i0]];
    const Edgel& b = getLineGraphPercept().edgelsOnField[outliers[i1]];

    // create model
    std::vector<Vector2d> models;
    models.reserve(2);

    if (a.point.x == b.point.x && a.point.y == b.point.y) {
      continue;
    }
    Vector2d between((a.point.x + b.point.x)/2, (a.point.y + b.point.y)/2);
    double distance = (a.point - b.point).abs();
    double half_distance = distance/2;

    if(half_distance > radius) continue;
    if(half_distance == radius) {
      models.push_back(between);
    } else {
      Vector2d model1;
      Vector2d model2;

      Vector2d direction(b.point-a.point);
      direction /= distance;

      double between_dist = sqrt(Math::sqr(radius) - Math::sqr(half_distance));

      model1.x = between.x - between_dist * direction.y;
      model1.y = between.y + between_dist * direction.x;

      model2.x = between.x + between_dist * direction.y;
      model2.y = between.y - between_dist * direction.x;

      models.push_back(model1);
      models.push_back(model2);
    }

    for (Vector2d model : models) {
      Vector2d direction_var;
      double inlierError = 0;
      int inlier = 0;

      for(size_t i: outliers)
      {
        const Edgel& e = getLineGraphPercept().edgelsOnField[i];
        double offset = std::fabs(radius - (model - e.point).abs());

        // inlier
        if(offset <= params.circle_outlierThreshold &&
          Math::toDegrees(angle_diff(model, e)) <= params.circle_max_angle_diff) {
          double ang = e.direction.angle();
          direction_var.x += cos(ang);
          direction_var.y += sin(ang);

          ++inlier;
          inlierError += offset;
        }
      }
      if(inlier >= params.circle_inlierMin) {
        direction_var /= inlier;
        double angle_variance = 1 - sqrt(Math::sqr(direction_var.x) + Math::sqr(direction_var.y));

        if((inlier > bestInlier || (inlier == bestInlier && inlierError < bestInlierError))
          && angle_variance >= params.circle_angle_variance) {
          bestModel = model;
          bestInlier = inlier;
          bestInlierError = inlierError;
        }
      }
    }
  }

  if (bestInlier >= params.circle_inlierMin) {
    std::vector<size_t> newOutliers;
    newOutliers.reserve(outliers.size() - bestInlier + 1);
    inlierPoints.reserve(bestInlier);

    // update the outliers
    for(size_t i: outliers)
    {
      const Edgel& e = getLineGraphPercept().edgelsOnField[i];
      double offset = std::fabs(radius - (bestModel - e.point).abs());

      if(offset > params.circle_outlierThreshold || Math::toDegrees(angle_diff(bestModel, e)) > params.circle_max_angle_diff) {
        newOutliers.push_back(i);
      } else {
        inlierPoints.push_back(e.point);
      }
    }
    outliers = newOutliers;

    result = bestModel;
    return true;
  }
  return false;
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
    for(int t=1; t<=5; t++) {
      size_t r = choose_random_from(outliers, t);
      const Edgel& e = getLineGraphPercept().edgelsOnField[r];

      x.push_back(e.point.x);
      y.push_back(e.point.y);
    }

    ellipse.fitPoints(x,y);

    // check model
    double inlierError = 0;
    int inlier = 0;

    for(size_t i: outliers)
    {
      const Edgel& e = getLineGraphPercept().edgelsOnField[i];
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
      const Edgel& e = getLineGraphPercept().edgelsOnField[i];
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

void RansacLineDetector::simpleLinearRegression(std::vector<Vector2d> &points, double& slope, double& intercept) {
  int n = (int) points.size();

  if(n == 1) {
    slope = 0.0;
    intercept = points[0].y;
    return;
  }

  Vector2d avg;
  for(Vector2d point : points) {
    avg += point;
  }
  avg /= n;

  double top = 0.;
  double bot = 0.;
  double x_diff;
  for(Vector2d point : points) {
    x_diff = point.x - avg.x;
    top += x_diff * (point.y - avg.y);
    bot += x_diff * x_diff;
  }

  slope = top / bot;
  intercept = avg.y - slope * avg.x;

  return;
}
