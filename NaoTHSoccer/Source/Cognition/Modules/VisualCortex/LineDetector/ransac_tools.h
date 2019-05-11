#ifndef RANSAC_TOOLS_H
#define RANSAC_TOOLS_H

#include <iostream>
#include <vector>

#include <Tools/Math/Common.h>
#include <Tools/Math/Line.h>
#include "Representations/Perception/LineGraphPercept.h"

namespace ransac {

// NOTE: we never call this function with vec.size() <= 2
Vector2i choose_random_two(const std::vector<size_t> &vec);

double angle_variance(const std::vector<Edgel>& edgels, const std::vector<size_t>& inlier_idx);


class LineModel {
public:
  size_t id1;
  size_t id2;

  int inlier;
  double inlierError;

  Math::Line line;

  LineModel();

  LineModel(size_t id1, size_t id2, const std::vector<Edgel>& edgels);

  bool betterThan(const LineModel& other) const;

  Math::LineSegment getLineSegment(const std::vector<Edgel>& edgels, const std::vector<size_t>& inlier_idx) const;

  inline double distance(const Edgel& edgel) const;

  inline double similarity(const Edgel& edgel) const;
};


class RansacLine {

public:
  int iterations;
  double minDirectionSimilarity;
  double outlierThresholdDist;

private:
  std::vector<size_t> edgel_idx;

public:
  RansacLine(int iterations, double minDirectionSimilarity, double outlierThresholdDist);
  RansacLine(std::vector<size_t>& edgel_idx, int iterations, double minDirectionSimilarity, double outlierThresholdDist);

  void setParameters(int iterations, double minDirectionSimilarity, double outlierThresholdDist);

  bool find_best_model(LineModel& bestModel, const std::vector<Edgel>& edgels);

  void get_inliers(const LineModel& model, const std::vector<Edgel>& edgels, std::vector<size_t>& inlier_idx, std::vector<size_t>& outlier_idx);

  void set_edgel_idx(std::vector<size_t>& edgel_idx) {
    this->edgel_idx = edgel_idx;
  }
};


class CircleModel {
public:
  int inlier;
  double inlierError;

  double radius;
  Vector2d circle_mean;

  CircleModel(): inlier(0), inlierError(0), radius(0) {
  }

  CircleModel(double radius): inlier(0), inlierError(0), radius(radius)
  {
  }

  CircleModel(Vector2d circle_mean, double radius): inlier(0), inlierError(0), radius(radius)
  {
    this->circle_mean = circle_mean;
  }

  inline bool estimateCircle(const Edgel& a, const Edgel& b)
  {
    double half_distance = (a.point - b.point).abs()/2;

    // the points need to be reasonably close to each other to form a circle
    if(half_distance > 0.9*radius) {
      return false;
    }

    Math::Line la(a.point, Vector2d(a.direction).rotateLeft());
    Math::Line lb(b.point, Vector2d(b.direction).rotateLeft());
    double t = la.intersection(lb);

    // too large error
    if(fabs(t) > radius*1.5) {
      return false;
    }

    Math::LineSegment lab(a.point, b.point);
    const Vector2d c = la.point(t);
    const Vector2d direction = (c - lab.projection(c)).normalize();

    Vector2d between((a.point + b.point)/2);
    // NOTE: we allways make sure half_distance > radius, but just to be sure take fabs ;)
    double between_dist = sqrt(fabs(Math::sqr(radius) - Math::sqr(half_distance)));

    circle_mean = between + direction*between_dist;

    //assert(std::isfinite(center.x) && std::isfinite(center.y));
    return true;
  }

  inline double angle_diff(const Edgel& edgel) const
  {
    double a = fabs(Vector2d(edgel.point - circle_mean).rotateRight().angleTo(edgel.direction));
    return std::min(a, Math::pi - a);
  }

  inline bool isInlier(const Edgel& edgel, double& distError, double maxAngleError, double outlierThresholdDist) const
  {
    distError = std::fabs(radius - (circle_mean - edgel.point).abs());
    return distError <= outlierThresholdDist && angle_diff(edgel) <= maxAngleError;
  }

  bool betterThan(const CircleModel& other) const
  {
    return inlier > other.inlier || (inlier == other.inlier && inlierError < other.inlierError);
  }

  Vector2d refine(const std::vector<Edgel>& edgels, const std::vector<size_t>& inlier_idx) const
  {
    Vector2d result;
    double n = 0;

    for(size_t idx: inlier_idx)
    {
      const Edgel& edgel = edgels[idx];

      Vector2d c = edgel.point + (circle_mean - edgel.point).normalize(radius);
      result += c;
      n++;
    }

    if(n > 0) {
      result /= n;
    }

    return result;
  }
};


class RansacCircle {
public:
  int iterations;
  double outlierThresholdAngle;
  double outlierThresholdDist;

  double radius;

private:
  std::vector<size_t> edgel_idx;

public:
  RansacCircle(int iterations, double outlierThresholdAngle, double outlierThresholdDist, double radius);
  RansacCircle(std::vector<size_t>& edgel_idx, int iterations,
               double outlierThresholdAngle, double outlierThresholdDist, double radius);

  void setParameters(int iterations, double outlierThresholdAngle, double outlierThresholdDist, double radius);

  bool find_best_model(CircleModel& best_model, const std::vector<Edgel>& edgels) {
    if(edgel_idx.size() <= 2) {
      return false;
    }

    best_model.radius = radius;

    for(int i = 0; i < iterations; ++i)
    {
      Vector2i idx = choose_random_two(edgel_idx);

      const Edgel& a = edgels[idx[0]];
      const Edgel& b = edgels[idx[1]];

      CircleModel model(radius);

      if(!model.estimateCircle(a,b) ||
         model.angle_diff(a) > outlierThresholdAngle ||
         model.angle_diff(b) > outlierThresholdAngle)
      {
        continue;
      }

      for(size_t idx: edgel_idx)
      {
        double distError = 0.0;
        if(model.isInlier(edgels[idx], distError, outlierThresholdAngle, outlierThresholdDist))
        {
          model.inlier++;
          model.inlierError += distError;
        }
      }

      if(model.betterThan(best_model)) {
        best_model = model;
      }
    }
    return true;
  }

  void get_inliers(const CircleModel& model, const std::vector<Edgel>& edgels, std::vector<size_t>& inlier_idx, std::vector<size_t>& outlier_idx) {
    outlier_idx.reserve(edgel_idx.size() - model.inlier);
    inlier_idx.clear(); // make sure the list is empty
    inlier_idx.reserve(model.inlier);

    for(size_t idx: edgel_idx)
    {
      double distError = 0.0;
      // TODO why 3.0*
      if(model.isInlier(edgels[idx], distError, 3.0*outlierThresholdAngle, outlierThresholdDist)) {
        inlier_idx.push_back(idx);
      } else {
        outlier_idx.push_back(idx);
      }
    }
  }

  void set_edgel_idx(std::vector<size_t>& edgel_idx) {
    this->edgel_idx = edgel_idx;
  }
};

}// end namespace

#endif // RANSAC_TOOLS_H
