/**
* @file ZMPPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#ifndef _ZMP_PLANNER_H
#define _ZMP_PLANNER_H

#include "FootStep.h"

class ZMPPlanner
{
public:
  /**
   * @brief just set the ZMP on support foot
   */
  static Vector2d simplest(const FootStep& step, double offsetX, double offsetY);

  /**
   * @brief a little better than the simplest(..) one
   */
  static Vector2d betterOne(const FootStep& step, double offsetX, double offsetY,
    double cycle, double samplesDoubleSupport, double samplesSingleSupport, double offset, double width);

  static Vector2d bezierBased(const FootStep& step, double offsetX, double offsetY,
                              double cycle, double samplesDoubleSupport, double samplesSingleSupport, double inFootScalingY, double transitionScaling);

private:
  static std::vector<Vector2d> FourPointBezier2D(const std::vector<Vector2d>& polygon, unsigned int number_of_samples){
      std::vector<Vector2d> trajectory;

      for(int i = 0; static_cast<double>(i)/static_cast<double>(number_of_samples) <= 1; ++i){
          double t = static_cast<double>(i)/static_cast<double>(number_of_samples);

          trajectory.push_back(  polygon[0] * ((1-t)*(1-t)*(1-t))
                               + polygon[1] * (3*t*(1-t)*(1-t))
                               + polygon[2] * (3*t*t*(1-t))
                               + polygon[3] * (t*t*t));
      }

      return  trajectory;
  }
};

#endif // _FOOT_STEP_PLANNER_H
