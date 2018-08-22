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
#include "Representations/Perception/LinePercept.h"

#include "Tools/Debug/DebugParameterList.h"

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

  PROVIDE(LinePercept)
  PROVIDE(RansacCirclePercept)
END_DECLARE_MODULE(RansacLineDetector)

class RansacLineDetector: public RansacLineDetectorBase
{
public:
 RansacLineDetector();
 ~RansacLineDetector();

 virtual void execute();

private:
  std::vector<size_t> outliers;
  bool ransac(Math::LineSegment& result);

  int ransacEllipse(Ellipse& result);

  bool ransacCircle(Vector2d& result, std::vector<Vector2d>& inlierPoints);

//private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("RansacLineDetector")
    {
      //Lines
      PARAMETER_REGISTER(iterations) = 50;
      PARAMETER_REGISTER(outlierThreshold) = 70;
      PARAMETER_REGISTER(inlierMin) = 5;
      PARAMETER_REGISTER(directionSimilarity) = 0.8;
      PARAMETER_REGISTER(maxLines) = 11;
      PARAMETER_REGISTER(maxVariance) = 0.009;
      PARAMETER_REGISTER(length_of_var_check) = 800;
      PARAMETER_REGISTER(min_line_length) = 100;

      //Circle
      PARAMETER_REGISTER(circle_iterations) = 20;
      PARAMETER_REGISTER(circle_outlierThreshold) = 70;
      PARAMETER_REGISTER(circle_inlierMin) = 7;
      PARAMETER_REGISTER(circle_angle_variance) = 0.02;
      PARAMETER_REGISTER(circle_max_angle_diff) = 8;
      PARAMETER_REGISTER(enable_circle_ransac) = true;
      PARAMETER_REGISTER(fit_circle_to_inliers) = true;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }
    int iterations;
    double outlierThreshold;
    int inlierMin;
    double directionSimilarity;
    int maxLines;

    double maxVariance;
    double length_of_var_check;
    double min_line_length;

    int circle_iterations;
    double circle_outlierThreshold;
    int circle_inlierMin;
    double circle_angle_variance;
    double circle_max_angle_diff;
    bool enable_circle_ransac;
    bool fit_circle_to_inliers;

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

    return acos(fabs(tangent_d*edgel.direction));
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
};

#endif // RANSACLINEDETECTOR_H
