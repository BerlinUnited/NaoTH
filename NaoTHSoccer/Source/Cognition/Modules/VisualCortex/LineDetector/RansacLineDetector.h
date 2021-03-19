#ifndef RANSACLINEDETECTOR_H
#define RANSACLINEDETECTOR_H

#include <iostream>

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

#include "ransac_tools.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/LineGraphPercept.h"
#include "Representations/Perception/LinePercept2018.h"

#include "Ellipse.h"

BEGIN_DECLARE_MODULE(RansacLineDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
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
      PARAMETER_ANGLE_REGISTER(line.outlierThresholdAngle) = 8;
      
      PARAMETER_REGISTER(line.maxVariance) = 0.009;
      PARAMETER_REGISTER(line.max_length_for_var_check) = 800;
      PARAMETER_REGISTER(line.min_line_length) = 100;
      //PARAMETER_REGISTER(line.fit_lines_to_inliers) = false;

      //Circle
      PARAMETER_REGISTER(circle.maxIterations) = 50;
      PARAMETER_REGISTER(circle.minInliers) = 7;
      PARAMETER_REGISTER(circle.outlierThresholdDist) = 70;
      PARAMETER_ANGLE_REGISTER(circle.outlierThresholdAngle) = 8;
      
      PARAMETER_REGISTER(circle.enable) = true;
      PARAMETER_REGISTER(circle.refine) = true;

      PARAMETER_REGISTER(circle.validate) = true;
      PARAMETER_REGISTER(circle.validation_thresh) = 200;

      syncWithConfig();
    }

    struct Line {
      int maxLines;
      int maxIterations;
      int minInliers;
      double outlierThresholdDist;
      double outlierThresholdAngle;

      double maxVariance;
      double max_length_for_var_check;
      double min_line_length;

      //bool fit_lines_to_inliers;
    } line;

    struct Circle {
      int maxIterations;
      int minInliers;
      double outlierThresholdDist;
      double outlierThresholdAngle;
      
      bool enable;
      bool refine;

      bool validate;
      double validation_thresh;
    } circle;

  } params;

  // index vector on remaining outlier edgels
  std::vector<size_t> outliers;


private: // detectors
  ransac::RansacLine lineRansac;
  ransac::RansacCircle circleRansac;

  int ransacEllipse(Ellipse& result);

private: // helper methods

  /**
    Get random items without replacement from a vector.
    Beware: This changes the order of the items.

    @param vec vector to choose random item from.
    @param ith choose the ith random item without replacement.
    Ex: For 3 random items without replacement call
    the function 3 times with ith=1, ith=2 and ith=3.
    @return random item of the vector without replacement.
  */
  size_t choose_random_from(std::vector<size_t> &vec, int ith)  const {
    int max = static_cast<int>(vec.size())-1;
    int random_pos = Math::random(ith, max);
    std::swap(vec[random_pos], vec[ith-1]);
    return vec[ith-1];
  }
};

#endif // RANSACLINEDETECTOR_H
