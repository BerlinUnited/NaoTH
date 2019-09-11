#ifndef RANSAC_TOOLS_H
#define RANSAC_TOOLS_H

#include <iostream>
#include <vector>

#include <Tools/Math/Common.h>
#include <Tools/Math/Line.h>
#include "Representations/Perception/LineGraphPercept.h"
#include "Tools/Math/Geometry.h"

namespace ransac {

Vector2i choose_random_two(const std::vector<size_t> &vec);

double angle_variance(const std::vector<Edgel>& edgels,
                      const std::vector<size_t>& inlier_idx);


class LineModel {
public:
  int inlier;
  double inlierError;

  Math::Line line;

  LineModel() : inlier(0), inlierError(0) {}

  LineModel(const Edgel& edgel1, const Edgel& edgel2) :
    inlier(0),
    inlierError(0)
  {
    line = Math::Line(edgel1.point, edgel2.point-edgel1.point);
  }


  bool betterThan(const LineModel& other) const;

  Math::LineSegment getLineSegment(const std::vector<Edgel>& edgels,
                                   const std::vector<size_t>& inlier_idx) const;

  inline double distance(const Edgel& edgel) const {
    return line.minDistance(edgel.point);
  }

  /*
  // calculate the similarity to the edgel
  // returns a value [0,1], 0 - not simmilar, 1 - very simmilar
  inline double similarity(const Edgel& edgel) const
  {
    double s = 0.0;
    if(line.getDirection()*edgel.direction > 0) {
      Vector2d v(edgel.point - line.getBase());
      v.rotateRight().normalize();
      s = 1.0-0.5*(fabs(line.getDirection()*v) + fabs(edgel.direction*v));
    }
    return s;
  }*/

  inline double angle_diff(const Edgel& edgel) const
  {
    double a = std::fabs(line.getDirection().angleTo(edgel.direction));
    return std::min(a, Math::pi - a);
  }
};


class RansacLine {

public:
  int iterations;
  double outlierThresholdAngle;
  double outlierThresholdDist;

public:
  RansacLine(int iterations, double outlierThresholdAngle, double outlierThresholdDist);

  void setParameters(int iterations, double outlierThresholdAngle,
                     double outlierThresholdDist);

  bool find_best_model(LineModel& bestModel, const std::vector<Edgel>& edgels,
                       const std::vector<size_t>& edgel_idx) const;

  void get_inliers(const LineModel& model, const std::vector<Edgel>& edgels,
                   const std::vector<size_t>& edgel_idx,
                   std::vector<size_t>& inlier_idx,
                   std::vector<size_t>& outlier_idx) const;
};


class CircleModel {
public:
  int inlier;
  double inlierError;

  double radius;
  Vector2d circle_mean;

  // constructors
  CircleModel() : inlier(0), inlierError(0), radius(0) {}

  CircleModel(double radius) : inlier(0), inlierError(0), radius(radius) {}

  CircleModel(const Vector2d& circle_mean, double radius) 
    : inlier(0), inlierError(0), radius(radius), circle_mean(circle_mean)
  {}

  inline bool estimateCircle(const Edgel& a, const Edgel& b);

  inline double angle_diff(const Edgel& edgel) const;

  inline bool isInlier(const Edgel& edgel, double& distError,
                       double maxAngleError, double outlierThresholdDist) const;

  bool betterThan(const CircleModel& other) const;

  bool validate(const std::vector<Edgel>& edgels,
                    const std::vector<size_t>& inlier_idx, double threshold) const;

  Vector2d refine(const std::vector<Edgel>& edgels,
                  const std::vector<size_t>& inlier_idx) const;
};


class RansacCircle {
public:
  int iterations;
  double outlierThresholdAngle;
  double outlierThresholdDist;

  double radius;

public:
  RansacCircle(int iterations,
               double outlierThresholdAngle, double outlierThresholdDist,
               double radius);

  void setParameters(int iterations,
                     double outlierThresholdAngle, double outlierThresholdDist,
                     double radius);

  bool find_best_model(CircleModel& best_model,
                       const std::vector<Edgel>& edgels,
                       const std::vector<size_t>& edgel_idx) const;

  void get_inliers(const CircleModel& model, const std::vector<Edgel>& edgels,
                   const std::vector<size_t>& edgel_idx,
                   std::vector<size_t>& inlier_idx,
                   std::vector<size_t>& outlier_idx) const;
};

}// end namespace

#endif // RANSAC_TOOLS_H
