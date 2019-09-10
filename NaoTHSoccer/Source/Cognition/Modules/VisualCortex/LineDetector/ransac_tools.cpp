#include "ransac_tools.h"

using namespace ransac;

/**
    Returns two unique random indices of a given vector.

    @param vec std vector with at least 2 elements.
    @return Vector2i holding the indices
*/
Vector2i ransac::choose_random_two(const std::vector<size_t> &vec)
{
  //ASSERT(vec.size() > 1);
  int size = static_cast<int>(vec.size());
  int random_pos_one = Math::random(size);
  int random_pos_two = (random_pos_one + Math::random(size-1) + 1) % size;
  return Vector2i(random_pos_one, random_pos_two);
}


double ransac::angle_variance(const std::vector<Edgel>& edgels,
                              const std::vector<size_t>& inlier_idx)
{
  Vector2d direction_var;

  for(size_t idx: inlier_idx) {
    direction_var += edgels[idx].direction;
  }
  direction_var /= static_cast<double>(inlier_idx.size());
  double angle_var = 1 - direction_var.abs();
  return angle_var;
}


bool LineModel::betterThan(const LineModel& other) const
{
  return inlier > other.inlier ||
        (inlier == other.inlier && inlierError < other.inlierError);
}

Math::LineSegment LineModel::getLineSegment(
    const std::vector<Edgel>& edgels,
    const std::vector<size_t>& inlier_idx) const
{
  // exception if not at least one inlier
  size_t id0 = inlier_idx.at(0);
  double minT = line.project(edgels[id0].point);
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

RansacLine::RansacLine(int iterations,
                       double outlierThresholdAngle,
                       double outlierThresholdDist):
iterations(iterations),
outlierThresholdAngle(outlierThresholdAngle),
outlierThresholdDist(outlierThresholdDist)
{
}

void RansacLine::setParameters(int iterations,
                               double outlierThresholdAngle,
                               double outlierThresholdDist)
{
  this->iterations = iterations;
  this->outlierThresholdAngle = outlierThresholdAngle;
  this->outlierThresholdDist = outlierThresholdDist;
}

bool RansacLine::find_best_model(LineModel& bestModel,
                                 const std::vector<Edgel>& edgels,
                                 const std::vector<size_t>& edgel_idx) const
{
  // needs more than 2 edgels
  if(edgel_idx.size() <= 2) {
    return false;
  }

  for(int i = 0; i < iterations; ++i)
  {
    // Get two random edgels to form a line
    Vector2i idx = choose_random_two(edgel_idx);
    const Edgel& a = edgels[edgel_idx[idx[0]]];
    const Edgel& b = edgels[edgel_idx[idx[1]]];

    LineModel model(a, b);

    // prior check: edgel directions should fit the
    // direction of the line (same check as for inliers)
    if(model.angle_diff(a) > outlierThresholdAngle ||
       model.angle_diff(b) > outlierThresholdAngle) {
      continue;
    }

    // look for inliers
    for(size_t i: edgel_idx)
    {
      const Edgel& edgel = edgels[i];
      double distance = model.distance(edgel);

      // inlier
      if(distance <= outlierThresholdDist &&
         model.angle_diff(edgel) <= outlierThresholdAngle)
      {
        model.inlier++;
        model.inlierError += distance;
      }
    }

    ASSERT(model.inlier >= 2);

    if(model.betterThan(bestModel)) {
      bestModel = model;
    }
  }

  // FIXME: this check needs to be checked
  //        bestModel is passed as an argument and is not necessary 0
  //        also, this check can be performed by the caller, which makes it perhaps unnecessary here
  if(bestModel.inlier > 0) {
    return true;
  }
  return false;
}// end find_best_model

void RansacLine::get_inliers(const LineModel& model,
                             const std::vector<Edgel>& edgels,
                             const std::vector<size_t>& edgel_idx,
                             std::vector<size_t>& inlier_idx,
                             std::vector<size_t>& outlier_idx) const
{
  outlier_idx.reserve(edgel_idx.size() - model.inlier);
  inlier_idx.clear(); // make sure the list is empty
  inlier_idx.reserve(model.inlier);

  for(size_t idx: edgel_idx)
  {
    const Edgel& edgel = edgels[idx];
    double distance = model.distance(edgel);

    if(distance <= outlierThresholdDist &&
       model.angle_diff(edgel) <= outlierThresholdAngle)
    {
      inlier_idx.push_back(idx);
    } else {
      outlier_idx.push_back(idx);
    }
  }
}// end get_inliers


inline bool CircleModel::estimateCircle(const Edgel& a, const Edgel& b)
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
  const Vector2d direction = (c - lab.Line::projection(c)).normalize();

  Vector2d between((a.point + b.point)/2);
  // NOTE: we allways make sure half_distance > radius,
  // but just to be sure take fabs ;)
  double between_dist = sqrt(fabs(Math::sqr(radius) - Math::sqr(half_distance)));

  circle_mean = between + direction*between_dist;

  //assert(std::isfinite(center.x) && std::isfinite(center.y));
  return true;
}

inline double CircleModel::angle_diff(const Edgel& edgel) const
{
  double a = fabs(Vector2d(edgel.point - circle_mean).rotateRight().angleTo(edgel.direction));
  return std::min(a, Math::pi - a);
}

inline bool CircleModel::isInlier(const Edgel& edgel, double& distError,
                                  double maxAngleError,
                                  double outlierThresholdDist) const
{
  distError = std::fabs(radius - (circle_mean - edgel.point).abs());
  return distError <= outlierThresholdDist && angle_diff(edgel) <= maxAngleError;
}

bool CircleModel::betterThan(const CircleModel& other) const
{
  return inlier > other.inlier || (inlier == other.inlier && inlierError < other.inlierError);
}

bool CircleModel::validate(const std::vector<Edgel>& edgels,
                           const std::vector<size_t>& inlier_idx,
                           double threshold) const
{
  std::vector<Vector2d> points(inlier_idx.size());

  for(size_t i=0; i<inlier_idx.size(); ++i) {
    points[i] = edgels[inlier_idx[i]].point;
  }

  Vector2d center;
  double radius;
  if(Geometry::calculateCircle(points, center, radius)) {
    return std::fabs(radius - this->radius) <= threshold;
  }
  return false;
}

Vector2d CircleModel::refine(const std::vector<Edgel>& edgels,
                             const std::vector<size_t>& inlier_idx) const
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


RansacCircle::RansacCircle(int iterations, double outlierThresholdAngle,
                           double outlierThresholdDist, double radius):
  iterations(iterations),
  outlierThresholdAngle(outlierThresholdAngle),
  outlierThresholdDist(outlierThresholdDist),
  radius(radius)
{}

void RansacCircle::setParameters(int iterations,
                                 double outlierThresholdAngle,
                                 double outlierThresholdDist, double radius)
{
  this->iterations = iterations;
  this->outlierThresholdAngle = outlierThresholdAngle;
  this->outlierThresholdDist = outlierThresholdDist;
  this->radius = radius;
}

bool RansacCircle::find_best_model(CircleModel& best_model,
                                   const std::vector<Edgel>& edgels,
                                   const std::vector<size_t>& edgel_idx) const
{
  if(edgel_idx.size() <= 2) {
    return false;
  }

  best_model.radius = radius;

  for(int i = 0; i < iterations; ++i)
  {
    Vector2i idx = choose_random_two(edgel_idx);

    const Edgel& a = edgels[edgel_idx[idx[0]]];
    const Edgel& b = edgels[edgel_idx[idx[1]]];

    CircleModel model(radius);

    double distError = 0.0;

    if(!model.estimateCircle(a,b) ||
       !model.isInlier(a, distError, outlierThresholdAngle, outlierThresholdDist) ||
       !model.isInlier(b, distError, outlierThresholdAngle, outlierThresholdDist))
    {
      continue;
    }

    for(size_t i: edgel_idx)
    {
      distError = 0.0;
      if(model.isInlier(edgels[i], distError,
                        outlierThresholdAngle, outlierThresholdDist))
      {
        model.inlier++;
        model.inlierError += distError;
      }
    }

    ASSERT(model.inlier >= 2);

    if(model.betterThan(best_model)) {
      best_model = model;
    }
  }
  return true;
}

void RansacCircle::get_inliers(const CircleModel& model,
                               const std::vector<Edgel>& edgels,
                               const std::vector<size_t>& edgel_idx,
                               std::vector<size_t>& inlier_idx,
                               std::vector<size_t>& outlier_idx) const
{
  outlier_idx.reserve(edgel_idx.size() - model.inlier);
  inlier_idx.clear(); // make sure the list is empty
  inlier_idx.reserve(model.inlier);

  for(size_t idx: edgel_idx)
  {
    double distError = 0.0;
    // NOTE: Less strict outlierThresholdAngle then acquiring inliers
    if(model.isInlier(edgels[idx], distError,
                      3.0*outlierThresholdAngle, outlierThresholdDist))
    {
      inlier_idx.push_back(idx);
    } else {
      outlier_idx.push_back(idx);
    }
  }
}
