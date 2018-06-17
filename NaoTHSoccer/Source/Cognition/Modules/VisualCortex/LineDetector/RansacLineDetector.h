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

  PROVIDE(LinePercept)
END_DECLARE_MODULE(RansacLineDetector)

class RansacLineDetector: public RansacLineDetectorBase
{
public:
 RansacLineDetector();
 ~RansacLineDetector();

 virtual void execute();

private:
  std::vector<size_t> outliers;
  int ransac(Math::LineSegment& result, std::vector<size_t>& inliers);

  int ransacEllipse(Ellipse& result);

  int ransacCircle(Vector2d& result, std::vector<size_t>& inliers);

//private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("RansacLineDetector")
    {
      //Lines
      PARAMETER_REGISTER(iterations) = 50;
      PARAMETER_REGISTER(outlierThreshold) = 40;
      PARAMETER_REGISTER(inlierMin) = 8;
      PARAMETER_REGISTER(directionSimilarity) = 0.8;
      PARAMETER_REGISTER(maxLines) = 11;
      PARAMETER_REGISTER(maxVariance) = 40;
      PARAMETER_REGISTER(length_of_var_check) = 700;
      PARAMETER_REGISTER(min_line_length) = 300;

      //Circle
      PARAMETER_REGISTER(circle_iterations) = 20;
      PARAMETER_REGISTER(circle_outlierThreshold) = 70;
      PARAMETER_REGISTER(circle_inlierMin) = 10;

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

  //swaps v[i] with a random v[x], x in [0:i]
  size_t swap_random(std::vector<size_t> &v, int i) {
    int r = Math::random(i+1);
    std::swap(v[r], v[i]);

    return v[i];
  }
};

#endif // RANSACLINEDETECTOR_H
