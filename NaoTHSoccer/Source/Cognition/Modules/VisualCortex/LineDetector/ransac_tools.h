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

class RansacCircle {
public:
  int iterations;
  double outlierThresholdAngle;
  double outlierThresholdDist;

private:
  std::vector<size_t> edgel_idx;

public:
  RansacCircle(int iterations, double outlierThresholdAngle, double outlierThresholdDist);
  RansacCircle(std::vector<size_t>& edgel_idx, int iterations, double outlierThresholdAngle, double outlierThresholdDist);

  void setParameters(int iterations, double minDirectionSimilarity, double outlierThresholdDist);

  void set_edgel_idx(std::vector<size_t>& edgel_idx) {
    this->edgel_idx = edgel_idx;
  }
};

}// end namespace

#endif // RANSAC_TOOLS_H
