/**
* @file CameraMatrixCorrectorV2.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraMatrixProvider
*/

#ifndef _CameraMatrixCorrectorV2_h_
#define _CameraMatrixCorrectorV2_h_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
#include "Representations/Perception/LineGraphPercept.h"

#include "Representations/Modeling/KinematicChain.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"

#include "Tools/DoubleCamHelpers.h"
#include "Tools/CameraGeometry.h"
#include <Tools/NaoInfo.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include <Tools/Math/Minimizer.h>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CamMatErrorFunction)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugModify)

  REQUIRE(FieldInfo)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrixOffset)
END_DECLARE_MODULE(CamMatErrorFunction)

BEGIN_DECLARE_MODULE(CameraMatrixCorrectorV2)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugModify)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)
  REQUIRE(FrameInfo)

  // data needed for calibration
  REQUIRE(LineGraphPercept)
  REQUIRE(KinematicChain)
  REQUIRE(SensorJointData)
  REQUIRE(InertialModel)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(CameraMatrixOffset)
END_DECLARE_MODULE(CameraMatrixCorrectorV2)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////
/// \brief The CameraMatrixCorrectorV2 class

// HACK: shouldn't be a module, should be a service... wait and see what the future holds ;)
class CamMatErrorFunction : public CamMatErrorFunctionBase
{
public:
    struct CalibrationDataSample {
        CalibrationDataSample(const Pose3D& chestPose, const LineGraphPercept& lineGraphPercept, const InertialModel& inertialModel, double headYaw  , double headPitch)
                             :chestPose(chestPose)   , lineGraphPercept(lineGraphPercept)      , inertialModel(inertialModel)      , headYaw(headYaw), headPitch(headPitch)
        {}

        //KinematicChain   kinematicChain;
        Pose3D chestPose; //work around, can't copy kinematic chain
        LineGraphPercept lineGraphPercept;
        InertialModel    inertialModel;
        double headYaw;
        double headPitch;
    };

    typedef std::vector<CalibrationDataSample> CalibrationData;

private:
    //CameraInfo::CameraID cameraID;
    CalibrationData calibrationData;
    size_t numberOfResudials;

    const CameraInfo& getCameraInfo(int cameraID) const {
      if(cameraID == CameraInfo::Top) {
        return CamMatErrorFunctionBase::getCameraInfoTop();
      } else {
        return CamMatErrorFunctionBase::getCameraInfo();
      }
    }

    const std::vector<EdgelD>& getEdgelsInImage(std::vector<CalibrationDataSample>::const_iterator& sample, int cameraID) const {
      if(cameraID == CameraInfo::Top) {
        return sample->lineGraphPercept.edgelsInImageTop;
      } else {
        return sample->lineGraphPercept.edgelsInImage;
      }
    }

public:
    CamMatErrorFunction(){
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:only_bottom", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:only_top", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:draw_projected_edgels", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:draw_matching_global",  "", false);
    }

    void execute(){}

    // additional position parameter
    Vector3d globalPosition; //x, y, z_angle

    size_t getNumberOfResudials() const {
        return numberOfResudials;
    }

    void add(const CalibrationDataSample& c_data_sample){
        int numNewResudials = 2 - c_data_sample.lineGraphPercept.edgelsInImage.empty() - c_data_sample.lineGraphPercept.edgelsInImageTop.empty();
        if (numNewResudials > 0){
            calibrationData.push_back(c_data_sample);
            numberOfResudials += numNewResudials;
        }
    }

    void clear(){
        calibrationData.clear();
        numberOfResudials = 0;
    }

    void plot_CalibrationData(){
        bool only_bottom = false;
        DEBUG_REQUEST("CamMatErrorFunction:debug_drawings:only_bottom",
                      only_bottom = true;
        );

        bool only_top = false;
        DEBUG_REQUEST("CamMatErrorFunction:debug_drawings:only_top",
                      only_top = true;
        );

        if(only_bottom && only_top){
            actual_plotting(CameraInfo::Bottom);
            actual_plotting(CameraInfo::Top);
        } else if (only_bottom) {
            actual_plotting(CameraInfo::Bottom);
        } else if (only_top) {
            actual_plotting(CameraInfo::Top);
        } else {
            actual_plotting(CameraInfo::Bottom);
            actual_plotting(CameraInfo::Top);
        }
    }

    void actual_plotting(CameraInfo::CameraID cameraID){
        for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample){
                CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromChestPose(
                            sample->chestPose,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                            getCameraMatrixOffset().body_rot,
                            getCameraMatrixOffset().head_rot,
                            getCameraMatrixOffset().cam_rot[cameraID],
                            sample->headYaw,
                            sample->headPitch,
                            sample->inertialModel.orientation
                            );

                tmpCM.translation += Vector3d(globalPosition.x, globalPosition.y,0); // move around on field
                //tmpCM.rotation    *= RotationMatrix::getRotationZ(globalPosition.z); // and rotate first around the global z axis
                tmpCM.rotation = RotationMatrix::getRotationZ(globalPosition.z) * tmpCM.rotation; // why? shouldn't it be the other way around?

                std::vector<Vector2d> edgelProjections;
                edgelProjections.resize(getEdgelsInImage(sample,cameraID).size());

                // draw projected edgels to field
                for(size_t i = 0; i < getEdgelsInImage(sample,cameraID).size(); i++)
                {
                    const EdgelT<double>& edgelOne = getEdgelsInImage(sample,cameraID)[i];

                    CameraGeometry::imagePixelToFieldCoord(
                                tmpCM, getCameraInfo(cameraID),
                                edgelOne.point.x,
                                edgelOne.point.y,
                                0.0,
                                edgelProjections[i]);

                    DEBUG_REQUEST("CamMatErrorFunction:debug_drawings:draw_projected_edgels",
                            FIELD_DRAWING_CONTEXT;
                            PEN("000000", 10);
                            CIRCLE(edgelProjections[i].x, edgelProjections[i].y, 20);
                    );
                }

                DEBUG_REQUEST("CamMatErrorFunction:debug_drawings:draw_projected_edgels",
                        FIELD_DRAWING_CONTEXT;
                        PEN("FF0000", 10);
                        ROBOT(globalPosition.x, globalPosition.y, globalPosition.z);
                );

                DEBUG_REQUEST("CamMatErrorFunction:debug_drawings:draw_matching_global",
                // determine distance to nearst field line and the total aberration
                    for(std::vector<Vector2d>::const_iterator iter = edgelProjections.begin(); iter != edgelProjections.end(); ++iter){

                        const Vector2d& seen_point_relative = *iter;

                        Pose2D   robotPose;
                        Vector2d seen_point_global = robotPose * seen_point_relative;

                        LinesTable::NamedPoint line_point_global = getFieldInfo().fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);

                        // there is no such line
                        if(line_point_global.id == -1) {
                            continue;
                        }

                        FIELD_DRAWING_CONTEXT;
                        PEN("000000", 10);

                        CIRCLE(seen_point_global.x, seen_point_global.y, 20);
                        LINE(line_point_global.position.x,line_point_global.position.y,seen_point_global.x, seen_point_global.y);
                    }

                    PEN("FF0000", 10);
                    ROBOT(globalPosition.x, globalPosition.y, globalPosition.z);
                );
        }
    }

    Eigen::VectorXd operator()(const Eigen::Matrix<double, 14, 1>& parameter) const {
        Eigen::VectorXd r(numberOfResudials);

        Vector2d offsetBody(parameter(0),parameter(1));
        Vector3d offsetHead(parameter(2), parameter(3),parameter(4));
        Vector3d offsetCam[CameraInfo::numOfCamera];
        offsetCam[CameraInfo::Top]    = Vector3d(parameter(5), parameter(6),parameter(7));
        offsetCam[CameraInfo::Bottom] = Vector3d(parameter(8), parameter(9),parameter(10));
        double global_x_offset = parameter(11);
        double global_y_offset = parameter(12);
        double global_z_angle_offset = parameter(13);

        size_t idx = 0;
        size_t empty = 0;
        for(int cameraID = 0; cameraID < CameraInfo::numOfCamera; cameraID++){
            for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample){
                if (getEdgelsInImage(sample,cameraID).empty()) {
                    ++empty;
                    continue;
                }

                CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromChestPose(
                            //sample->kinematicChain,
                            sample->chestPose,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                            getCameraMatrixOffset().body_rot + offsetBody,
                            getCameraMatrixOffset().head_rot + offsetHead,
                            getCameraMatrixOffset().cam_rot[cameraID]  + offsetCam[cameraID],
                            sample->headYaw,
                            sample->headPitch,
                            sample->inertialModel.orientation
                            );

                tmpCM.translation += Vector3d(globalPosition.x + global_x_offset, globalPosition.y + global_y_offset,0); // move around on field
                //tmpCM.rotation    *= RotationMatrix::getRotationZ(globalPosition.z + global_z_angle_offset); // and rotate first around the global z axis
                tmpCM.rotation = RotationMatrix::getRotationZ(globalPosition.z + global_z_angle_offset) * tmpCM.rotation; // why? shouldn't it be the other way around?

                std::vector<Vector2d> edgelProjections;
                edgelProjections.resize(getEdgelsInImage(sample,cameraID).size());

                // project edgels pairs to field
                for(size_t i = 0; i < getEdgelsInImage(sample,cameraID).size(); i++)
                {
                    const EdgelT<double>& edgelOne = getEdgelsInImage(sample,cameraID)[i];

                    CameraGeometry::imagePixelToFieldCoord(
                                tmpCM, getCameraInfo(cameraID),
                                edgelOne.point.x,
                                edgelOne.point.y,
                                0.0,
                                edgelProjections[i]);
                }

                double total_sum = 0;
                // determine distance to nearst field line and the total aberration
                for(std::vector<Vector2d>::const_iterator seen_point_relative = edgelProjections.begin(); seen_point_relative != edgelProjections.end(); ++seen_point_relative){
                    Pose2D   robotPose;
                    Vector2d seen_point_global = robotPose * (*seen_point_relative);

                    LinesTable::NamedPoint line_point_global = getFieldInfo().fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);

                    // there is no such line or point
                    if(line_point_global.id == -1 /*&& corner.id == -1*/) {
                        continue;
                    }

                    double dist = (seen_point_global - line_point_global.position).abs();

                    total_sum += dist;
                }

                r(idx) = total_sum;
                ++idx;
            }
        }

        ASSERT(empty+idx == calibrationData.size()*2)
        ASSERT(idx == numberOfResudials);
        return r;
    }

    Eigen::VectorXd operator()(const Eigen::Matrix<double, 11, 1>& parameter) const {
        Eigen::VectorXd r(numberOfResudials);

        Vector2d offsetBody(parameter(0),parameter(1));
        Vector3d offsetHead(parameter(2), parameter(3),parameter(4));
        Vector3d offsetCam[CameraInfo::numOfCamera];
        offsetCam[CameraInfo::Top]    = Vector3d(parameter(5), parameter(6),parameter(7));
        offsetCam[CameraInfo::Bottom] = Vector3d(parameter(8), parameter(9),parameter(10));

        size_t idx = 0;
        for(int cameraID = 0; cameraID < CameraInfo::numOfCamera; cameraID++){
            for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample){
                if (getEdgelsInImage(sample,cameraID).size() == 0) {
                    continue;
                }

                CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromChestPose(
                            //sample->kinematicChain,
                            sample->chestPose,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                            getCameraMatrixOffset().body_rot + offsetBody,
                            getCameraMatrixOffset().head_rot + offsetHead,
                            getCameraMatrixOffset().cam_rot[cameraID]  + offsetCam[cameraID],
                            sample->headYaw,
                            sample->headPitch,
                            sample->inertialModel.orientation
                            );

                tmpCM.translation += Vector3d(globalPosition.x, globalPosition.y,0); // move around on field
                //tmpCM.rotation    *= RotationMatrix::getRotationZ(globalPosition.z + global_z_angle_offset); // and rotate first around the global z axis
                tmpCM.rotation = RotationMatrix::getRotationZ(globalPosition.z) * tmpCM.rotation; // why? shouldn't it be the other way around?

                std::vector<Vector2d> edgelProjections;
                edgelProjections.resize(getEdgelsInImage(sample,cameraID).size());

                // project edgels pairs to field
                for(size_t i = 0; i < getEdgelsInImage(sample,cameraID).size(); i++)
                {
                    const EdgelT<double>& edgelOne = getEdgelsInImage(sample,cameraID)[i];

                    CameraGeometry::imagePixelToFieldCoord(
                                tmpCM, getCameraInfo(cameraID),
                                edgelOne.point.x,
                                edgelOne.point.y,
                                0.0,
                                edgelProjections[i]);
                }

                double total_sum = 0;
                // determine distance to nearst field line and the total aberration
                for(std::vector<Vector2d>::const_iterator seen_point_relative = edgelProjections.begin(); seen_point_relative != edgelProjections.end(); ++seen_point_relative){
                    Pose2D   robotPose;
                    Vector2d seen_point_global = robotPose * (*seen_point_relative);

                    LinesTable::NamedPoint line_point_global = getFieldInfo().fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);

                    // there is no such line or point
                    if(line_point_global.id == -1 /*&& corner.id == -1*/) {
                        continue;
                    }

                    double dist = (seen_point_global - line_point_global.position).abs();

                    total_sum += dist;
                }

                r(idx) = total_sum;
                ++idx;
            }
        }

        ASSERT(idx == numberOfResudials);
        return r;
    }

    Eigen::VectorXd operator()(const Eigen::Matrix<double, 3, 1>& parameter) const {
        Eigen::VectorXd r(numberOfResudials);

        double global_x_offset = parameter(0);
        double global_y_offset = parameter(1);
        double global_z_angle_offset = parameter(2);

        size_t idx = 0;
        for(int cameraID = 0; cameraID < CameraInfo::numOfCamera; cameraID++){
            for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample){
                if (getEdgelsInImage(sample,cameraID).size() == 0) {
                    continue;
                }

                CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromChestPose(
                            //sample->kinematicChain,
                            sample->chestPose,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                            NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                            getCameraMatrixOffset().body_rot,
                            getCameraMatrixOffset().head_rot,
                            getCameraMatrixOffset().cam_rot[cameraID],
                            sample->headYaw,
                            sample->headPitch,
                            sample->inertialModel.orientation
                            );

                tmpCM.translation += Vector3d(globalPosition.x + global_x_offset, globalPosition.y + global_y_offset,0); // move around on field
                //tmpCM.rotation    *= RotationMatrix::getRotationZ(globalPosition.z + global_z_angle_offset); // and rotate first around the global z axis
                tmpCM.rotation = RotationMatrix::getRotationZ(globalPosition.z + global_z_angle_offset) * tmpCM.rotation; // why? shouldn't it be the other way around?

                std::vector<Vector2d> edgelProjections;
                edgelProjections.resize(getEdgelsInImage(sample,cameraID).size());

                // project edgels pairs to field
                for(size_t i = 0; i < getEdgelsInImage(sample,cameraID).size(); i++)
                {
                    const EdgelT<double>& edgelOne = getEdgelsInImage(sample,cameraID)[i];

                    CameraGeometry::imagePixelToFieldCoord(
                                tmpCM, getCameraInfo(cameraID),
                                edgelOne.point.x,
                                edgelOne.point.y,
                                0.0,
                                edgelProjections[i]);
                }

                double total_sum = 0;
                // determine distance to nearst field line and the total aberration
                for(std::vector<Vector2d>::const_iterator seen_point_relative = edgelProjections.begin(); seen_point_relative != edgelProjections.end(); ++seen_point_relative){
                    Pose2D   robotPose;
                    Vector2d seen_point_global = robotPose * (*seen_point_relative);

                    LinesTable::NamedPoint line_point_global = getFieldInfo().fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);

                    // there is no such line or point
                    if(line_point_global.id == -1 /*&& corner.id == -1*/) {
                        continue;
                    }

                    double dist = (seen_point_global - line_point_global.position).abs();

                    total_sum += dist;
                }

                r(idx) = total_sum;
                ++idx;
            }
        }

        ASSERT(idx == numberOfResudials);
        return r;
    }

};

// HACK: shouldn't be a ModuleManager but has to be because of the CamMatErrorFunction... see above
class CameraMatrixCorrectorV2: public CameraMatrixCorrectorV2Base, public ModuleManager
{
public:

  CameraMatrixCorrectorV2();
  ~CameraMatrixCorrectorV2();

  void execute();

private:

  enum {initial, look_left, look_right, look_right_down, look_left_down, look_right_up, look_left_up} head_state, last_head_state;

  int last_idx_yaw,last_idx_pitch;
  double damping_cam_offsets;
  double damping_pose;

  GaussNewtonMinimizer<CamMatErrorFunction> gn_minimizer;

  bool calibrate();
  bool calibrateOnlyPose();
  bool calibrateOnlyOffsets();
  void reset_calibration();
  bool movingHead();
  void collectingData();
  void doItAutomatically();

  ModuleCreator<CamMatErrorFunction>* theCamMatErrorFunction;

  // for automatic calibration
  bool auto_cleared_data, auto_collected, auto_calibrated;
  RingBufferWithSum<double, 200> derrors;
  double last_error;
  FrameInfo last_frame_info;

};

#endif //_CameraMatrixCorrectorV2_h_
