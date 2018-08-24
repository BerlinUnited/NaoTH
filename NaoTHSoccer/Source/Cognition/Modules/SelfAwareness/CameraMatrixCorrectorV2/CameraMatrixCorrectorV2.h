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

#include "Tools/DoubleCamHelpers.h"
#include "Tools/CameraGeometry.h"
#include <Tools/NaoInfo.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"

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

  // data needed for calibration
  REQUIRE(LineGraphPercept)
  REQUIRE(KinematicChain)
  REQUIRE(SensorJointData)
  REQUIRE(InertialModel)

  PROVIDE(HeadMotionRequest)
  PROVIDE(CameraMatrixOffset)
END_DECLARE_MODULE(CameraMatrixCorrectorV2)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////
/// \brief The CameraMatrixCorrectorV2 class

// HACK: shouldn't be a module, should be a service... wait and see what the future holds ;)
class CamMatErrorFunction : public CamMatErrorFunctionBase
{
public:
    struct CalibrationDataSample {
        CalibrationDataSample(
          const Pose3D& chestPose,
          const LineGraphPercept& lineGraphPercept,
          const InertialModel& inertialModel,
          double headYaw,
          double headPitch) 
          : chestPose(chestPose), lineGraphPercept(lineGraphPercept), inertialModel(inertialModel), headYaw(headYaw), headPitch(headPitch)
        {

        }

        //KinematicChain   kinematicChain;
        Pose3D chestPose; //work around, can't copy kinematic chain
        LineGraphPercept lineGraphPercept;
        InertialModel    inertialModel;
        double headYaw;
        double headPitch;
    };

    typedef std::map< std::pair<int, int>, std::vector<CalibrationDataSample> > CalibrationData;
    CalibrationData calibrationData;

private:

    CameraInfo::CameraID cameraID;

    DOUBLE_CAM_REQUIRE(CamMatErrorFunction,CameraInfo);

    const std::vector<EdgelD>& getEdgelsInImage(std::vector<CalibrationDataSample>::const_iterator& sample) const {
      if(cameraID == CameraInfo::Top) {
        return sample->lineGraphPercept.edgelsInImageTop;
      } else {
        return sample->lineGraphPercept.edgelsInImage;
      }
    }

public:
    CamMatErrorFunction()
        : cameraID(CameraInfo::Bottom)
    {
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:only_bottom", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:only_top", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:draw_projected_edgels", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:draw_matching_global",  "", false);
    }

    void execute(){}

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

    void actual_plotting(CameraInfo::CameraID i){
        cameraID = i;

        for(CalibrationData::const_iterator cd_iter = calibrationData.begin(); cd_iter != calibrationData.end(); ++cd_iter){
            for(std::vector<CalibrationDataSample>::const_iterator sample = cd_iter->second.begin(); sample != cd_iter->second.end(); ++sample) {
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

                //tmpCM.translation += Vector3d(0, getFieldInfo().yPosRightSideline, 0); // move around on field
                //tmpCM.rotation = RotationMatrix::getRotationZ(Math::fromDegrees(90)) * tmpCM.rotation;

                std::vector<Vector2d> edgelProjections;
                edgelProjections.resize(getEdgelsInImage(sample).size());

                // draw projected edgels to field
                for(size_t i = 0; i < getEdgelsInImage(sample).size(); i++)
                {
                    const EdgelT<double>& edgelOne = getEdgelsInImage(sample)[i];

                    CameraGeometry::imagePixelToFieldCoord(
                                tmpCM, getCameraInfo(),
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
                );
            }
        }
    }

    double operator()(Eigen::Matrix<double, 1, 11> parameter){

        double total_sum  = 0;
        double sample_cnt = 0;

        Vector2d offsetBody(parameter(0),parameter(1));
        Vector3d offsetHead(parameter(2), parameter(3),parameter(4));
        Vector3d offsetCam[CameraInfo::numOfCamera];
        offsetCam[CameraInfo::Top]    = Vector3d(parameter(5), parameter(6),parameter(7));
        offsetCam[CameraInfo::Bottom] = Vector3d(parameter(8), parameter(9),parameter(10));

        for(int i = 0; i < CameraInfo::numOfCamera; i++){
            cameraID = static_cast<CameraInfo::CameraID>(i);
            for(CalibrationData::const_iterator cd_iter = calibrationData.begin(); cd_iter != calibrationData.end(); ++cd_iter){
                for(std::vector<CalibrationDataSample>::const_iterator sample = cd_iter->second.begin(); sample != cd_iter->second.end(); ++sample) {
                    CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromChestPose(
                                //cd_iter->kinematicChain,
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

                    //tmpCM.translation += Vector3d(0, getFieldInfo().yPosRightSideline, 0); // move around on field
                    //tmpCM.rotation = RotationMatrix::getRotationZ(Math::fromDegrees(90)) * tmpCM.rotation;

                    std::vector<Vector2d> edgelProjections;
                    edgelProjections.resize(getEdgelsInImage(sample).size());

                    // project edgels pairs to field
                    for(size_t i = 0; i < getEdgelsInImage(sample).size(); i++)
                    {
                        const EdgelT<double>& edgelOne = getEdgelsInImage(sample)[i];

                        CameraGeometry::imagePixelToFieldCoord(
                                    tmpCM, getCameraInfo(),
                                    edgelOne.point.x,
                                    edgelOne.point.y,
                                    0.0,
                                    edgelProjections[i]);
                    }

                    sample_cnt += (double) edgelProjections.size();

                    // determine distance to nearst field line and the total aberration
                    for(std::vector<Vector2d>::const_iterator iter = edgelProjections.begin(); iter != edgelProjections.end(); ++iter){

                        const Vector2d& seen_point_relative = *iter;

                        Pose2D   robotPose;
                        Vector2d seen_point_global = robotPose * seen_point_relative;

                        LinesTable::NamedPoint line_point_global = getFieldInfo().fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);
                        //LinesTable::NamedPoint corner = getFieldInfo().fieldLinesTable.get_closest_corner_point(seen_point_global);

                        // there is no such line or point
                        if(line_point_global.id == -1 /*&& corner.id == -1*/) {
                            continue;
                        }

                        double dist           = (seen_point_global - line_point_global.position).abs();//(line_point_global.id == -1) ? std::numeric_limits<double>::infinity() : (seen_point_global - line_point_global.position).abs();
                        //double dist_corner  = (corner.id == -1)            ? std::numeric_limits<double>::infinity() : (seen_point_global - corner.position).abs();

                        //if(dist >= dist_corner) {
                        //    dist = dist_corner;
                        //}

                        total_sum += dist;
                    }
                }
            }
        }
        return total_sum;//sample_cnt; //just a try
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
  double damping;

  typedef double (CameraMatrixCorrectorV2::*ErrorFunction)(double, double);

  GaussNewtonMinimizer<1, 11, CamMatErrorFunction> gn_minimizer;

  void calibrate();
  void reset_calibration();

  ModuleCreator<CamMatErrorFunction>* theCamMatErrorFunction;

};

#endif //_CameraMatrixCorrectorV2_h_
