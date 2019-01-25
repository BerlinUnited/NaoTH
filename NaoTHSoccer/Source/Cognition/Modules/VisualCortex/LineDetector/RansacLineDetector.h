#ifndef RANSACLINEDETECTOR_H
#define RANSACLINEDETECTOR_H

#include <iostream>
#include <forward_list>

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/LineGraphPercept.h"
#include "Representations/Perception/LinePercept2018.h"

#include "Ellipse.h"

BEGIN_DECLARE_MODULE(RansacLineDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(LineGraphPercept)
  REQUIRE(FieldInfo)

  PROVIDE(RansacLinePercept)
  PROVIDE(RansacCirclePercept2018)
END_DECLARE_MODULE(RansacLineDetector)

class RansacLineDetector: public RansacLineDetectorBase
{
public:
 RansacLineDetector();
 ~RansacLineDetector();

 virtual void execute();

private:
  std::vector<size_t> outliers;
  bool ransac(Math::LineSegment& result, std::vector<size_t>& inliers, size_t& start_edgel, size_t& end_edgel);

  int ransacEllipse(Ellipse& result);

  bool ransacCircle(Vector2d& result, std::vector<size_t>& inliers);

//private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("RansacLineDetector")
    {
      //Lines
      PARAMETER_REGISTER(line.maxLines) = 11;
      PARAMETER_REGISTER(line.maxIterations) = 100;
      PARAMETER_REGISTER(line.minInliers) = 12;

      PARAMETER_REGISTER(line.outlierThresholdDist) = 70;
      PARAMETER_REGISTER(line.minDirectionSimilarity) = 0.8;
      
      PARAMETER_REGISTER(line.maxVariance) = 0.009;
      PARAMETER_REGISTER(line.length_of_var_check) = 800;
      PARAMETER_REGISTER(line.min_line_length) = 100;
      PARAMETER_REGISTER(line.fit_lines_to_inliers) = true;

      //Circle
      PARAMETER_REGISTER(circle.maxIterations) = 50;
      PARAMETER_REGISTER(circle.minInliers) = 7;
      PARAMETER_REGISTER(circle.outlierThresholdDist) = 70;
      PARAMETER_ANGLE_REGISTER(circle.outlierThresholdAngle) = 8;
      
      PARAMETER_REGISTER(circle.enable) = true;
      PARAMETER_REGISTER(circle.refine) = true;

      syncWithConfig();
    }

    struct Line {
      int maxLines;
      int maxIterations;
      int minInliers;
      double outlierThresholdDist;
      double minDirectionSimilarity;

      double maxVariance;
      double length_of_var_check;
      double min_line_length;

      bool fit_lines_to_inliers;
    } line;

    struct Circle {
      int maxIterations;
      int minInliers;
      double outlierThresholdDist;
      double outlierThresholdAngle;
      
      bool enable;
      bool refine;
    } circle;

  } params;

  // calculate the simmilarity to the other edgel
  // returns a value [0,1], 0 - not simmilar, 1 - very simmilar
  inline double sim(const Math::Line& line, const Edgel& edgel) const
  {
    double s = 0.0;
    if(line.getDirection()*edgel.direction > 0) {
      Vector2d v(edgel.point - line.getBase());
      v.rotateRight().normalize();
      s = 1.0-0.5*(fabs(line.getDirection()*v) + fabs(edgel.direction*v));
    }

    return s;
  }

  inline double sim(const Vector2d& circle_mean, const Edgel& edgel) const
  {
    Vector2d tangent_d(edgel.point - circle_mean);
    tangent_d.rotateRight().normalize();

    double s = 0.0;
    if(tangent_d*edgel.direction > 0) {
      s = 1.0-0.5*(fabs(edgel.direction*tangent_d));
    }

    return s;
  }

  inline double angle_diff(const Vector2d& circle_mean, const Edgel& edgel) const
  {
    Vector2d tangent_d(edgel.point - circle_mean);
    tangent_d.rotateRight().normalize();
    double a = acos(fabs(tangent_d*edgel.direction));
    return std::min(a, Math::pi - a);
  }

  /**
    Get random items without replacement from a vector.
    Beware: This changes the order of the items.

    @param vec vector to choose random item from.
    @param ith choose the ith random item without replacement.
    Ex: For 3 random items without replacement call
    the function 3 times with ith=1, ith=2 and ith=3.
    @return random item of the vector without replacement.
  */
  size_t choose_random_from(std::vector<size_t> &vec, int ith) {
    int max = (int) vec.size()-1;
    int random_pos = Math::random(ith, max);
    std::swap(vec[random_pos], vec[ith-1]);
    return vec[ith-1];
  }

  bool estimateCircle(const Edgel& a, const Edgel& b, const double radius, Vector2d& center);
  Vector2d refineCircle(const std::vector<size_t>& inliers, const Vector2d& center);

  inline bool isCircleInlier(size_t i, const Vector2d& center, double& distError, double maxAngleError) {
    const Edgel& e = getLineGraphPercept().edgelsOnField[i];
    const double radius = getFieldInfo().centerCircleRadius;

    distError = std::fabs(radius - (center - e.point).abs());

    return distError             <= params.circle.outlierThresholdDist && 
           angle_diff(center, e) <= maxAngleError;
  }
};

#endif // RANSACLINEDETECTOR_H
