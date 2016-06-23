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

#include "Representations/Perception/CameraMatrix.h"
//#include "Representations/Perception/GoalPercept.h"
//#include "Representations/Infrastructure/Image.h"
//#include "Representations/Infrastructure/JointData.h"
//#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/FieldInfo.h"
//#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/FrameInfo.h"
//#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
//#include "Representations/Perception/LineGraphPercept.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/LineGraphPercept.h"

// motion stuff
#include "Representations/Modeling/KinematicChain.h"

#include "Tools/DoubleCamHelpers.h"
#include "Tools/LinesTable.h"
#include "Tools/CameraGeometry.h"
#include <Tools/NaoInfo.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugDrawings3D.h>
#include <Tools/Debug/DebugImageDrawings.h>
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"

#include <Tools/Math/Minimizer.h>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CamMatErrorFunction)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugModify)

  REQUIRE(FieldInfo)
//  REQUIRE(FrameInfo)

//  REQUIRE(LineGraphPercept)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

//  REQUIRE(KinematicChain)

  PROVIDE(CameraMatrixOffset)

  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
END_DECLARE_MODULE(CamMatErrorFunction)

BEGIN_DECLARE_MODULE(CameraMatrixCorrectorV2)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugModify)

  REQUIRE(FrameInfo)

  REQUIRE(LineGraphPercept)
  REQUIRE(KinematicChain)
  REQUIRE(SensorJointData)

  PROVIDE(CameraMatrixOffset)
END_DECLARE_MODULE(CameraMatrixCorrectorV2)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////
/// \brief The CameraMatrixCorrectorV2 class

// HACK: shouldn't be a module, should be a service... wait and see what the future holds ;)
class CamMatErrorFunction : public CamMatErrorFunctionBase
{
public:
    struct CalibrationDataSample{
        //KinematicChain   kinematicChain;
        Pose3D headPose; //work around, can't copy kinematic chain
        LineGraphPercept lineGraphPercept;
    };

    typedef std::vector<CalibrationDataSample> CalibrationData;
    CalibrationData calibrationData;

private:

    DOUBLE_CAM_REQUIRE(CamMatErrorFunction,CameraMatrix);
    DOUBLE_CAM_REQUIRE(CamMatErrorFunction,CameraInfo);
    DOUBLE_CAM_REQUIRE(CamMatErrorFunction,ScanLineEdgelPercept);

    const std::vector<EdgelD>& getEdgelsInImage(CalibrationData::const_iterator& cd_iter) const {
      if(cameraID == CameraInfo::Top) {
        return cd_iter->lineGraphPercept.edgelsInImageTop;
      } else {
        return cd_iter->lineGraphPercept.edgelsInImage;
      }
    }

public:
    CamMatErrorFunction()
        : cameraID(CameraInfo::Bottom)
    {
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:draw_projected_edgels", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunction:debug_drawings:draw_matching_global",  "", false);

        calibrationData.resize(11);
    }

    void execute(){}

    CameraInfo::CameraID cameraID;

    double operator()(Eigen::Matrix<double, 1, 6> parameter){

        double total_sum  = 0;
        double sample_cnt = 0;

        Vector3d offsetHead(parameter(0), parameter(1),parameter(2));
        Vector3d offsetCam(parameter(3), parameter(4),parameter(5));

        for(CalibrationData::const_iterator cd_iter = calibrationData.begin(); cd_iter != calibrationData.end(); ++cd_iter){

            CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromHeadPose(
                        //cd_iter->kinematicChain,
                        cd_iter->headPose,
                        NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                        NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                        getCameraMatrixOffset().correctionOffsets[cameraID].head_rot + offsetHead,
                        getCameraMatrixOffset().correctionOffsets[cameraID].cam_rot + offsetCam
                        );

            std::vector<Vector2d> edgelProjections;
            edgelProjections.resize(getEdgelsInImage(cd_iter).size());

            // project edgels pairs to field
            for(size_t i = 0; i < getEdgelsInImage(cd_iter).size(); i++)
            {
                const EdgelT<double>& edgelOne = getEdgelsInImage(cd_iter)[i];

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

            sample_cnt += (double) edgelProjections.size();

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

                total_sum += (seen_point_global - line_point_global.position).abs();

                DEBUG_REQUEST("CamMatErrorFunction:debug_drawings:draw_matching_global",
                    FIELD_DRAWING_CONTEXT;
                    PEN("000000", 10);

                    CIRCLE(seen_point_global.x, seen_point_global.y, 20);
                    //CIRCLE(line_point_global.position.x, line_point_global.position.y, 20);
                    LINE(line_point_global.position.x,line_point_global.position.y,seen_point_global.x, seen_point_global.y);
                );
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

  void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }

private:

  int last_index;

  CameraInfo::CameraID cameraID;

  typedef double (CameraMatrixCorrectorV2::*ErrorFunction)(double, double);

  GaussNewtonMinimizer<1, 6, CamMatErrorFunction> gn_minimizer;

  void calibrate();
  void reset_calibration();

  ModuleCreator<CamMatErrorFunction>* theCamMatErrorFunction;

  DOUBLE_CAM_PROVIDE(CameraMatrixCorrectorV2,DebugImageDrawings);
};

#endif //_CameraMatrixCorrectorV2_h_
