#ifndef RANSACLINEDETECTOR_H
#define RANSACLINEDETECTOR_H

#include <iostream>

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include <Tools/Debug/DebugImageDrawings.h>
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/CameraGeometry.h"

#include "ransac_tools.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/LineGraphPercept.h"
#include "Representations/Perception/LinePercept2018.h"
#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Perception/CameraMatrix.h>

#include "Tools/DoubleCamHelpers.h"
#include "Ellipse.h"

BEGIN_DECLARE_MODULE(RansacLineDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)

  REQUIRE(LineGraphPercept)
  REQUIRE(LineGraphPerceptTop)
  REQUIRE(FieldInfo)

  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  PROVIDE(RansacLinePercept)
  PROVIDE(RansacCirclePercept2018)

  PROVIDE(RansacLinePerceptImage)
  PROVIDE(RansacLinePerceptImageTop)
END_DECLARE_MODULE(RansacLineDetector)

class RansacLineDetector: public RansacLineDetectorBase
{
public:
RansacLineDetector();
~RansacLineDetector();


virtual void execute(CameraInfo::CameraID id, const std::vector<Edgel>& edgelsOnField,
                     RansacLinePercept& ransacLinePercept,
                     RansacCirclePercept2018& ransacCirclePercept,
                     RansacLinePerceptImage& ransacLinePerceptImage);

void execute()
{
  // local representation which are required to determine
  // the field lines in the image but won't be "exported"
  // to the blackboard
  RansacLinePercept dumpRLP;
  RansacCirclePercept2018 dumpRCP;
  RansacLinePerceptImage dumpRLPImage;

  // determine lines in images
  execute(CameraInfo::Bottom, getLineGraphPercept().edgelsOnField,
          dumpRLP, dumpRCP, getRansacLinePerceptImage());

  dumpRLP.reset();
  dumpRCP.reset();

  execute(CameraInfo::Top, getLineGraphPerceptTop().edgelsOnField,
          dumpRLP, dumpRCP, getRansacLinePerceptImageTop());

  // determine RansacLinePercept and RansacCirclePercept2018 depending
  // on both, i.e. top and bottom, camera edgles on the field. Note that
  // this might enable the robot to detect lines which start in one image
  // and end in the other image which might be benefitial for e.g. self-localization
  getRansacLinePercept().reset();
  getRansacCirclePercept2018().reset();

  std::vector<Edgel> allEdgelsOnField(getLineGraphPercept().edgelsOnField);
  allEdgelsOnField.insert(allEdgelsOnField.end(),
                          getLineGraphPerceptTop().edgelsOnField.begin(),
                          getLineGraphPerceptTop().edgelsOnField.end());
  execute(CameraInfo::numOfCamera, allEdgelsOnField,
          getRansacLinePercept(), getRansacCirclePercept2018(), dumpRLPImage);
}

private:
class Parameters: public ParameterList
{
public:
    Parameters() : ParameterList("RansacLineDetector")
    {
        PARAMETER_REGISTER(detect_lines_first) = false;

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

    bool detect_lines_first;

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

void find_middle_circle(std::vector<size_t>& inlier_idx, std::vector<Edgel> edgelsOnField, RansacCirclePercept2018& ransacCirclePercept);
void find_field_lines(std::vector<size_t>& inlier_idx, std::vector<Edgel> edgelsOnField, RansacLinePercept& ransacLinePercepts);

void project_lines_on_image(RansacLinePercept& ransacLinePercept, RansacLinePerceptImage& ransacLinePerceptImage) const;

int ransacEllipse(Ellipse& result, std::vector<Edgel> edgelsOnField);

private: // helper methods

/**
Get random items without replacement from a vector. Beware: This changes the order of the items.

@param vec vector to choose random item from.
@param ith choose the ith random item without replacement.
       Ex: For 3 random items without replacement call the function 3 times with ith=1, ith=2 and ith=3.
@return random item of the vector without replacement.
*/
size_t choose_random_from(std::vector<size_t> &vec, int ith)  const {
    int max = static_cast<int>(vec.size())-1;
    int random_pos = Math::random(ith, max);
    std::swap(vec[random_pos], vec[ith-1]);
    return vec[ith-1];
}

  CameraInfo::CameraID cameraID;
  
  DOUBLE_CAM_REQUIRE(RansacLineDetector, CameraInfo);
  DOUBLE_CAM_REQUIRE(RansacLineDetector, CameraMatrix);
};

#endif // RANSACLINEDETECTOR_H
