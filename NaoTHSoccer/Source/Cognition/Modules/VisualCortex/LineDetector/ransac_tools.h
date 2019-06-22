#ifndef RANSAC_TOOLS_H
#define RANSAC_TOOLS_H

#include <iostream>
#include <vector>

#include <Tools/Math/Common.h>
#include <Tools/Math/Line.h>
#include "Representations/Perception/LineGraphPercept.h"

namespace ransac {

Vector2i choose_random_two(const std::vector<size_t> &vec);

double angle_variance(const std::vector<Edgel>& edgels,
                      const std::vector<size_t>& inlier_idx);


class LineModel {
public:
  size_t id1;
  size_t id2;

  int inlier;
  double inlierError;

  Math::Line line;

  LineModel() : id1(0), id2(0), inlier(0), inlierError(0) {}

  // FIXME: why not pass the const Edgel& edgel1 and const Edgel& edgel2 directly, like in estimate circle?
  //        Nobody seems to be using the id1 and id2
  //LineModel(const Edgel& e1, const Edgel& e2) : inlier(0), inlierError(0), line(e1.point, e2.point - e1.point) {}

  LineModel(size_t id1, size_t id2, const std::vector<Edgel>& edgels) :
    id1(id1),
    id2(id2),
    inlier(0),
    inlierError(0)
  {
    const Edgel& edgel1 = edgels[id1];
    const Edgel& edgel2 = edgels[id2];

    line = Math::Line(edgel1.point, edgel2.point-edgel1.point);
  }


  bool betterThan(const LineModel& other) const;

  Math::LineSegment getLineSegment(const std::vector<Edgel>& edgels,
                                   const std::vector<size_t>& inlier_idx) const;

  inline double distance(const Edgel& edgel) const {
    return line.minDistance(edgel.point);
  }

  inline double similarity(const Edgel& edgel) const;
};


class RansacLine {

public:
  int iterations;
  double minDirectionSimilarity;
  double outlierThresholdDist;

private:
  // FIXME: why do edgel_idx need to be a member, but std::vector<Edgel>& edgels not?
  std::vector<size_t> edgel_idx;

public:
  RansacLine(int iterations,
             double minDirectionSimilarity, double outlierThresholdDist);
  RansacLine(std::vector<size_t>& edgel_idx, int iterations,
             double minDirectionSimilarity, double outlierThresholdDist);

  void setParameters(int iterations, double minDirectionSimilarity,
                     double outlierThresholdDist);

  bool find_best_model(LineModel& bestModel,
                       const std::vector<Edgel>& edgels) const;

  void get_inliers(const LineModel& model,
                   const std::vector<Edgel>& edgels,
                   std::vector<size_t>& inlier_idx,
                   std::vector<size_t>& outlier_idx) const;

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

  Vector2d refine(const std::vector<Edgel>& edgels,
                  const std::vector<size_t>& inlier_idx) const;
};


class RansacCircle {
public:
  int iterations;
  double outlierThresholdAngle;
  double outlierThresholdDist;

  double radius;

private:
  // FIXME: why do edgel_idx need to be a member, but std::vector<Edgel>& edgels not?
  std::vector<size_t> edgel_idx;

public:
  RansacCircle(int iterations,
               double outlierThresholdAngle, double outlierThresholdDist,
               double radius);
  RansacCircle(std::vector<size_t>& edgel_idx, int iterations,
               double outlierThresholdAngle, double outlierThresholdDist,
               double radius);

  void setParameters(int iterations,
                     double outlierThresholdAngle, double outlierThresholdDist,
                     double radius);

  bool find_best_model(CircleModel& best_model,
                       const std::vector<Edgel>& edgels) const;

  void get_inliers(const CircleModel& model, const std::vector<Edgel>& edgels,
                   std::vector<size_t>& inlier_idx,
                   std::vector<size_t>& outlier_idx) const;

  void set_edgel_idx(std::vector<size_t>& edgel_idx) {
    this->edgel_idx = edgel_idx;
  }
};

}// end namespace

#endif // RANSAC_TOOLS_H
