#ifndef CAMMATERRORFUNCTIONV3_H
#define CAMMATERRORFUNCTIONV3_H

#include <Representations/Infrastructure/CameraInfo.h>
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/LineGraphPercept.h"
#include "Tools/CameraGeometry.h"

#include "Eigen/Eigen"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include "Tools/Math/Optimizer.h"

class CamMatErrorFunctionV3
{
public:
    struct CalibrationDataSample {
        CalibrationDataSample(){}

        CalibrationDataSample(const Pose3D& chestPose, const LineGraphPercept& lineGraphPercept, const Vector2d& orientation, double headYaw  , double headPitch)
                             :chestPose(chestPose)   , orientation(orientation)      , headYaw(headYaw), headPitch(headPitch)
        {
            edgelsInImage.reserve(lineGraphPercept.edgelsInImage.size());
            for(std::vector<EdgelD>::const_iterator iter = lineGraphPercept.edgelsInImage.begin(); iter != lineGraphPercept.edgelsInImage.end(); ++iter){
                edgelsInImage.push_back(iter->point);
            }

            edgelsInImageTop.reserve(lineGraphPercept.edgelsInImageTop.size());
            for(std::vector<EdgelD>::const_iterator iter = lineGraphPercept.edgelsInImageTop.begin(); iter != lineGraphPercept.edgelsInImageTop.end(); ++iter){
                edgelsInImageTop.push_back(iter->point);
            }
        }

        //KinematicChain   kinematicChain;
        Pose3D chestPose; //work around, can't copy kinematic chain
        std::vector<Vector2d> edgelsInImage;
        std::vector<Vector2d> edgelsInImageTop;
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

    const std::vector<Vector2d>& getEdgelsInImage(std::vector<CalibrationDataSample>::const_iterator& sample, int cameraID) const {
      if(cameraID == naoth::CameraInfo::Top) {
        return sample->edgelsInImageTop;
      } else {
        return sample->edgelsInImage;
      }
    }

    void actual_plotting(const Parameter& parameter, naoth::CameraInfo::CameraID cameraID);

public:
    CamMatErrorFunctionV3(
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
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:only_bottom", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:only_top", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:draw_projected_edgels", "", true);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:draw_matching_global",  "", false);
    }

    Eigen::VectorXd operator()(const Parameter& parameter) const;

    unsigned int getNumberOfResudials() const {
        return numberOfResudials;
    }

    void add(const CalibrationDataSample& c_data_sample)
    {
        unsigned int numNewResudials =
          (c_data_sample.edgelsInImage.empty()   ? 0 : 1) +
          (c_data_sample.edgelsInImageTop.empty()? 0 : 1);

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
        DEBUG_REQUEST("CamMatErrorFunctionV3:debug_drawings:only_bottom",
                      only_bottom = true;
        );

        bool only_top = false;
        DEBUG_REQUEST("CamMatErrorFunctionV3:debug_drawings:only_top",
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

#endif // CAMMATERRORFUNCTIONV3_H
