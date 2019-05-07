#ifndef RANSAC_TOOLS_H
#define RANSAC_TOOLS_H

#include <iostream>
#include <vector>

#include <Tools/Math/Common.h>
#include <Tools/Math/Line.h>
#include "Representations/Perception/LineGraphPercept.h"

namespace ransac {

// NOTE: we never call this function with vec.size() <= 2
Vector2i choose_random_two(const std::vector<size_t> &vec) {
  //ASSERT(vec.size() > 1);
  int size = static_cast<int>(vec.size());
  int random_pos_one = Math::random(size);
  int random_pos_two = (random_pos_one + Math::random(size-1) + 1) % size;
  return Vector2i(random_pos_one, random_pos_two);
}

class LineModel {
public:
  size_t id1;
  size_t id2;

  int inlier;
  double inlierError;

  Math::Line line;

  LineModel():
  id1(0),
  id2(0),
  inlier(0),
  inlierError(0)
  {
  }

  LineModel(size_t id1, size_t id2, const std::vector<Edgel>& edgels):
  id1(id1),
  id2(id2),
  inlier(0),
  inlierError(0)
  {
    const Edgel& edgel1 = edgels[id1];
    const Edgel& edgel2 = edgels[id2];

    line = Math::Line(edgel1.point, edgel2.point-edgel1.point);
  }

  bool betterThan(const LineModel& other) const {
    return inlier > other.inlier || (inlier == other.inlier && inlierError < other.inlierError);
  }

  Math::LineSegment getLineSegment(const std::vector<Edgel>& edgels, const std::vector<size_t>& inlier_idx) const {
    double minT = line.project(edgels[id1].point);
    double maxT = minT;

    for(size_t idx: inlier_idx) {
      const Edgel& edgel = edgels[idx];
      double t = line.project(edgel.point);
      if (t < minT) {
        minT = t;
      }
      else if (t > maxT) {
        maxT = t;
      }
    }
    return Math::LineSegment(line.point(minT), line.point(maxT));
  }

  inline double distance(const Edgel& edgel) const
  {
    return line.minDistance(edgel.point);
  }

  // calculate the simmilarity to the edgel
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
  }
};

double angle_variance(const std::vector<Edgel>& edgels, const std::vector<size_t>& inlier_idx) {
  Vector2d direction_var;

  for(size_t idx: inlier_idx) {
    direction_var += edgels[idx].direction;
  }
  direction_var /= static_cast<double>(inlier_idx.size());
  double angle_var = 1 - direction_var.abs();
  return angle_var;
}

class RansacLine {

private:
  int iterations;
  double minDirectionSimilarity;
  double outlierThresholdDist;

  std::vector<size_t> edgel_idx;

public:
  RansacLine(int iterations, double minDirectionSimilarity, double outlierThresholdDist, const std::vector<Edgel>& edgels):
  iterations(iterations),
  minDirectionSimilarity(minDirectionSimilarity),
  outlierThresholdDist(outlierThresholdDist),

  edgel_idx(edgels.size())
  {
    // create index structure to refer to the edgel list
    for(size_t i = 0; i < edgels.size(); ++i) {
      edgel_idx[i] = i;
    }
  }

  bool find_best_model(LineModel& bestModel, const std::vector<Edgel>& edgels) {
    // needs more than 2 edgels
    if(edgel_idx.size() > 2) {
      return false;
    }

    for(int i = 0; i < iterations; ++i)
    {
      // Get two random edgels to form a line
      Vector2i idx = choose_random_two(edgel_idx);
      const Edgel& a = edgels[idx[0]];
      const Edgel& b = edgels[idx[1]];

      // prior check: edgels should have simmilar direction
      if(a.sim(b) < minDirectionSimilarity) {
        continue;
      }

      LineModel model(idx[0], idx[1], edgels);

      // prior check: edgel directions should fit the direction of the line (same check as for inliers)
      if(model.similarity(a) < minDirectionSimilarity || model.similarity(b) < minDirectionSimilarity) {
        continue;
      }

      // look for inliers
      for(size_t i: edgel_idx)
      {
        const Edgel& edgel = edgels[i];
        double distance = model.distance(edgel);

        // inlier
        if(distance < outlierThresholdDist && model.similarity(edgel) > minDirectionSimilarity) {
          ++model.inlier;
          model.inlierError += distance;
        }
      }

      if(model.betterThan(bestModel)) {
        bestModel = model;
      }

      if(bestModel.inlier) {
        return true;
      }
      return false;
    }
  }// end find_best_model

  void get_inliers(const LineModel& model, const std::vector<Edgel>& edgels, std::vector<size_t>& inlier_idx, std::vector<size_t>& outlier_idx) {
    outlier_idx.reserve(edgel_idx.size() - model.inlier);
    inlier_idx.clear(); // make sure the list is empty
    inlier_idx.reserve(model.inlier);

    for(size_t idx: edgel_idx)
    {
      const Edgel& edgel = edgels[idx];
      double distance = model.distance(edgel);

      if(distance < outlierThresholdDist && model.similarity(edgel) > minDirectionSimilarity) {
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
