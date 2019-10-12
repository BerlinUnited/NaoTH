#include "RansacLineDetector.h"

#include "Tools/Math/Geometry.h"
#include <Tools/Math/Matrix_mxn.h>
#include <numeric>

RansacLineDetector::RansacLineDetector():
lineRansac(params.line.maxIterations,
           params.line.outlierThresholdAngle,
           params.line.outlierThresholdDist),
circleRansac(params.circle.maxIterations,
             params.circle.outlierThresholdAngle,
             params.circle.outlierThresholdDist,
             getFieldInfo().centerCircleRadius)
{
  // initialize some stuff here
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_edgels_field",
                         "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_lines_field",
                         "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:fit_and_draw_ellipse_field",
                         "", false);
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:draw_circle_field",
                         "", false);
  getDebugParameterList().add(&params);
}

RansacLineDetector::~RansacLineDetector()
{
  getDebugParameterList().remove(&params);
}

void RansacLineDetector::execute()
{
  getRansacLinePercept().reset();
  getRansacCirclePercept2018().reset();

  // prepare line id index structure to assign edgels to lines
  // prepare the index arrays
  getRansacLinePercept().edgelLineIDs.resize(
        getLineGraphPercept().edgelsOnField.size());
  outliers.resize(getLineGraphPercept().edgelsOnField.size());
  for(size_t i = 0; i < getLineGraphPercept().edgelsOnField.size(); ++i) {
    outliers[i] = i;
    getRansacLinePercept().edgelLineIDs[i] = -1;
  }

  std::vector<size_t> inlier_idx;

  /*****
   * Ransac to detect the center circle in edgels
   *****/

  if(params.circle.enable)
  {
    // reset parameters
    circleRansac.setParameters(params.circle.maxIterations,
                               params.circle.outlierThresholdAngle,
                               params.circle.outlierThresholdDist,
                               getFieldInfo().centerCircleRadius);
    ransac::CircleModel model;
    if(circleRansac.find_best_model(model, getLineGraphPercept().edgelsOnField, outliers)
       && model.inlier >= params.circle.minInliers)
    {
      // get inliers and outliers of the model
      std::vector<size_t> new_outlier_idx;
      circleRansac.get_inliers(model, getLineGraphPercept().edgelsOnField,
                               outliers, inlier_idx, new_outlier_idx);      

      bool valid = !params.circle.validate;
      if (!valid) {
        // VALIDATE
        std::vector<Vector2d> points(inlier_idx.size());

        for(size_t i=0; i<inlier_idx.size(); ++i) {
          points[i] = getLineGraphPercept().edgelsOnField[inlier_idx[i]].point;
        }

        Vector2d center;
        double radius;
        if (Geometry::calculateCircle(points, center, radius)) {
          valid = !params.circle.validate || std::fabs(radius - getFieldInfo().centerCircleRadius)
                               <= params.circle.validation_thresh;

          DEBUG_REQUEST("Vision:RansacLineDetector:draw_circle_field",
            FIELD_DRAWING_CONTEXT;
            PEN("000099", 5);
            for(size_t idx : inlier_idx) {
              const Edgel& inlier = getLineGraphPercept().edgelsOnField[idx];
              CIRCLE(inlier.point.x, inlier.point.y, 30);
            }

            if(valid) {
              PEN("99000066", 50);
            } else {
              PEN("FF69B1FF", 70);
            }
            CIRCLE(model.circle_mean.x, model.circle_mean.y, model.radius);

            PEN("DEAD07FF", 50);
            CIRCLE(center.x, center.y, radius);
          );
        }
      }

      if(valid) {
        // continue with ransac on remaining outliers
        outliers = new_outlier_idx;

        if(params.circle.refine)
        {
          Vector2d newCircleMean = model.refine(
                getLineGraphPercept().edgelsOnField, inlier_idx);

          getRansacCirclePercept2018().set(newCircleMean);

          DEBUG_REQUEST("Vision:RansacLineDetector:draw_circle_field",
            FIELD_DRAWING_CONTEXT;
            PEN("00990066", 35);
            CIRCLE(newCircleMean.x, newCircleMean.y, model.radius);
          );
        } else {
          getRansacCirclePercept2018().set(model.circle_mean);
        }
      }
    }
  }// end if circle

  /*****
   * Ransac to detect lines in edgels
   *****/

  // reset parameters
  lineRansac.setParameters(params.line.maxIterations,
                           params.line.outlierThresholdAngle,
                           params.line.outlierThresholdDist);

  for(int i = 0; i < params.line.maxLines; ++i)
  {
    ransac::LineModel model;
    if(lineRansac.find_best_model(model, getLineGraphPercept().edgelsOnField, outliers) &&
       model.inlier > params.line.minInliers)
    {
      inlier_idx.clear();

      // get inliers and outliers of the model
      std::vector<size_t> new_outlier_idx;
      lineRansac.get_inliers(model, getLineGraphPercept().edgelsOnField,
                             outliers, inlier_idx, new_outlier_idx);

      // calculate edgel angle variance
      // -> edgels on the same line are expected to have a low angle variance
      double angle_variance = ransac::angle_variance(
            getLineGraphPercept().edgelsOnField, inlier_idx);

      // calculate line segment and line length from inliers
      Math::LineSegment line = model.getLineSegment(
            getLineGraphPercept().edgelsOnField, inlier_idx);
      double line_length = line.getLength();

      bool variance_small = line_length > params.line.max_length_for_var_check ||
                            angle_variance <= params.line.maxVariance;

      if (line_length > params.line.min_line_length && variance_small)
      {
        // add line to line percept and assign corresponding line to edgels
        for(size_t idx : inlier_idx) {
          getRansacLinePercept().edgelLineIDs[idx] =
              static_cast<int>(getRansacLinePercept().fieldLineSegments.size());
        }
        getRansacLinePercept().fieldLineSegments.push_back(line);

        // set search space to remaining outliers
        outliers = new_outlier_idx;
      } else {
        break; // model does not satisfy parameters
      }
    } else {
      break; // no model has been found
    }
  }

  DEBUG_REQUEST("Vision:RansacLineDetector:draw_lines_field",
    FIELD_DRAWING_CONTEXT;
    for(size_t i=0; i<getRansacLinePercept().fieldLineSegments.size(); i++)
    {
      std::string color;
      switch(i%3) {
        case 0: color = "FF000066"; break;
        case 1: color = "0000FF66"; break;
        default: color = "00FFFF66"; break;
      }
      const Math::LineSegment& line =
          getRansacLinePercept().fieldLineSegments[i];
      PEN(color, 50);
      LINE(line.begin().x, line.begin().y, line.end().x, line.end().y);
    }
  );

  DEBUG_REQUEST("Vision:RansacLineDetector:draw_edgels_field",
    FIELD_DRAWING_CONTEXT;
    for(size_t i=0; i<getLineGraphPercept().edgelsOnField.size(); i++)
    {
      const Edgel& e = getLineGraphPercept().edgelsOnField[i];

      if(getRansacLinePercept().edgelLineIDs[i] > -1) {
        std::string color;
        switch(getRansacLinePercept().edgelLineIDs[i]%3) {
        case 0: color = "FF0000"; break;
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
      LINE(e.point.x, e.point.y,
           e.point.x + e.direction.x*100.0, e.point.y + e.direction.y*100.0);
    }
  );

  // NOTE: experimental
  DEBUG_REQUEST("Vision:RansacLineDetector:fit_and_draw_ellipse_field",
    FIELD_DRAWING_CONTEXT;
    // fit ellipse
    Ellipse ellipseResult;
    int bestInlierCirc = ransacEllipse(ellipseResult);

    if (bestInlierCirc > 0)
    {
      Vector2d c = ellipseResult.getCenter();
      Vector2d a = ellipseResult.axesLength();

      Vector2d ax(a.x,0.0);
      ax.rotate(ellipseResult.rotationAngle());

      Vector2d ay(0.0,a.y);
      ay.rotate(ellipseResult.rotationAngle());

      // draw the oval and the axes
      PEN("009900", 50);
      OVAL_ROTATED(c.x, c.y, a.x, a.y, ellipseResult.rotationAngle());

      PEN("FF0000", 20);
      CIRCLE(c.x, c.y, 50);
      LINE(c.x, c.y, c.x+ax.x, c.y+ax.y);
      LINE(c.x, c.y, c.x+ay.x, c.y+ay.y);

      // draw fitted points
      PEN("0000AA", 20);
      for(size_t i=0; i<ellipseResult.x_toFit.size(); i++) {
        CIRCLE(ellipseResult.x_toFit[i], ellipseResult.y_toFit[i], 20);
      }

      // NOTE: experimental (Heinrich)
      Vector2d point(500,500);
      Vector2d p = ellipseResult.test_project(point);
      PEN("FF0000", 20);
      CIRCLE(point.x, point.y, 50);
      PEN("0000FF", 20);
      CIRCLE(p.x, p.y, 50);
      PEN("000000", 10);
      LINE(point.x, point.y, p.x, p.y);
    }
  );
}

int RansacLineDetector::ransacEllipse(Ellipse& result)
{
  if(outliers.size() <= 5) {
    return 0;
  }

  Ellipse bestModel;
  int bestInlier = 0;
  double bestInlierError = 0;

  for(int i = 0; i < params.circle.maxIterations; ++i)
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

      if(d <= params.circle.outlierThresholdDist) {
        ++inlier;
        inlierError += d;
        continue;
      }
    }
    if(inlier >= params.circle.minInliers && (inlier > bestInlier ||
                                              (inlier == bestInlier &&
                                              inlierError < bestInlierError)))
    {
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

      if(d > params.circle.outlierThresholdDist) {
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
