#ifndef LINECAMMATERRORFUNCTIONV3_H
#define LINECAMMATERRORFUNCTIONV3_H

#include <Representations/Infrastructure/CameraInfo.h>
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/LineGraphPercept.h"
#include "Representations/Perception/LinePercept2018.h"
#include "Tools/CameraGeometry.h"

#include "Eigen/Eigen"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include "Tools/Math/Optimizer.h"

class LineCamMatErrorFunctionV3
{
public:
    struct CalibrationDataSample {
        CalibrationDataSample(){}

        CalibrationDataSample(const Pose3D& chestPose, const RansacLinePerceptImage& line_percept_image, const RansacLinePerceptImage& line_percept_image_top, const Vector2d& orientation, double headYaw, double headPitch)
                             :chestPose(chestPose), orientation(orientation), headYaw(headYaw), headPitch(headPitch)
        {
            field_lines_in_image.reserve(line_percept_image.imageFieldLineSegments.size());
            for (Math::LineSegment field_line_in_image : line_percept_image.imageFieldLineSegments) {
                field_lines_in_image.push_back(field_line_in_image);
            }
            field_lines_in_image_top.reserve(line_percept_image_top.imageFieldLineSegments.size());
            for (Math::LineSegment field_line_in_image : line_percept_image_top.imageFieldLineSegments) {
                field_lines_in_image_top.push_back(field_line_in_image);
            }
        }

        //KinematicChain   kinematicChain;
        Pose3D chestPose; //work around, can't copy kinematic chain
        std::vector<Math::LineSegment> field_lines_in_image;
        std::vector<Math::LineSegment> field_lines_in_image_top;
        Vector2d orientation;
        double headYaw;
        double headPitch;
    };

    typedef std::vector<CalibrationDataSample> CalibrationData;
    typedef Eigen::Matrix<double, 11, 1> Parameter; // TODO: can this be the same typedef like in CameraMatrixCorrectorV3

    Optimizer::BoundedVariable<Parameter> const * bounds;
    Vector2d const * global_position;
    double const * global_orientation;

private:
    CalibrationData calibrationData;
    unsigned int numberOfResudials;

    DebugRequest&        theDebugRequest;
    DebugDrawings&       theDebugDrawings;
    //DebugModify&         theDebugModify;
    const FieldInfo&     theFieldInfo;
    const naoth::CameraInfo&    theCameraInfo;
    const naoth::CameraInfoTop& theCameraInfoTop;

    DebugRequest& getDebugRequest() {
        return theDebugRequest;
    }

    DebugDrawings& getDebugDrawings() {
        return theDebugDrawings;
    }

    const naoth::CameraInfo& getCameraInfo(int cameraID) const {
      if(cameraID == naoth::CameraInfo::Top) {
        return theCameraInfoTop;
      } else {
        return theCameraInfo;
      }
    }

    const std::vector<Math::LineSegment>& getLinesInImage(std::vector<CalibrationDataSample>::const_iterator& sample, int cameraID) const {
      if(cameraID == naoth::CameraInfo::Top) {
        return sample->field_lines_in_image_top;
      } else {
        return sample->field_lines_in_image;
      }
    }

    void actual_plotting(const Parameter& parameter, naoth::CameraInfo::CameraID cameraID);

public:
    LineCamMatErrorFunctionV3(
        DebugRequest&        theDebugRequest,
        DebugDrawings&       theDebugDrawings,
        DebugModify&         /*theDebugModify*/,
        const FieldInfo&     theFieldInfo,
        const naoth::CameraInfo&    theCameraInfo,
        const naoth::CameraInfoTop& theCameraInfoTop
    ):
        bounds(nullptr),
        numberOfResudials(0),
        theDebugRequest(theDebugRequest),
        theDebugDrawings(theDebugDrawings),
        //theDebugModify(theDebugModify),
        theFieldInfo(theFieldInfo),
        theCameraInfo(theCameraInfo),
        theCameraInfoTop(theCameraInfoTop)
    {
        DEBUG_REQUEST_REGISTER("LineCamMatErrorFunctionV3:debug_drawings:only_bottom", "", false);
        DEBUG_REQUEST_REGISTER("LineCamMatErrorFunctionV3:debug_drawings:only_top", "", false);
        DEBUG_REQUEST_REGISTER("LineCamMatErrorFunctionV3:debug_drawings:draw_projected_lines", "", true);
        DEBUG_REQUEST_REGISTER("LineCamMatErrorFunctionV3:debug_drawings:draw_matching_global",  "", false);
    }

    Eigen::VectorXd operator()(const Parameter& parameter) const;

    unsigned int getNumberOfResudials() const {
        return numberOfResudials;
    }

    void add(const CalibrationDataSample& c_data_sample)
    {
        unsigned int numNewResudials =
          (c_data_sample.field_lines_in_image.empty()   ? 0 : 1) +
          (c_data_sample.field_lines_in_image_top.empty()? 0 : 1);

        if (numNewResudials > 0) {
            calibrationData.push_back(c_data_sample);
            numberOfResudials += numNewResudials;
        }
    }

    void clear(){
        calibrationData.clear();
        numberOfResudials = 0;
    }

    void plot_CalibrationData(const Parameter& parameter){
        bool only_bottom = false;
        DEBUG_REQUEST("LineCamMatErrorFunctionV3:debug_drawings:only_bottom",
                      only_bottom = true;
        );

        bool only_top = false;
        DEBUG_REQUEST("LineCamMatErrorFunctionV3:debug_drawings:only_top",
                      only_top = true;
        );

        if(only_bottom && only_top){
            actual_plotting(parameter, naoth::CameraInfo::Bottom);
            actual_plotting(parameter, naoth::CameraInfo::Top);
        } else if (only_bottom) {
            actual_plotting(parameter, naoth::CameraInfo::Bottom);
        } else if (only_top) {
            actual_plotting(parameter, naoth::CameraInfo::Top);
        } else {
            actual_plotting(parameter, naoth::CameraInfo::Bottom);
            actual_plotting(parameter, naoth::CameraInfo::Top);
        }
    }

    void write_calibration_data_to_file();
    void read_calibration_data_from_file();
};

#endif // LINECAMMATERRORFUNCTIONV3_H
