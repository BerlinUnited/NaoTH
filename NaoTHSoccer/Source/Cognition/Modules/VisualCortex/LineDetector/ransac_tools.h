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

  LineModel();

  LineModel(size_t id1, size_t id2, const std::vector<Edgel>& edgels);

  bool betterThan(const LineModel& other) const;

  Math::LineSegment getLineSegment(const std::vector<Edgel>& edgels,
                                   const std::vector<size_t>& inlier_idx) const;

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

  CircleModel();

  CircleModel(double radius);

  CircleModel(Vector2d circle_mean, double radius);

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
