/**
* @file CameraMatrixCorrectorV2.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraMatrixProvider
*/

#ifndef _CameraMatrixCorrectorV2_h_
#define _CameraMatrixCorrectorV2_h_

#include <ModuleFramework/Module.h>

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

BEGIN_DECLARE_MODULE(CameraMatrixCorrectorV2)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugModify)
  
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  REQUIRE(KinematicChain)

  PROVIDE(CameraMatrixOffset)
END_DECLARE_MODULE(CameraMatrixCorrectorV2)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////
/// \brief The CameraMatrixCorrectorV2 class

#define DOUBLE_CAM_IN_ERROR_FUNCTION(name)      \
  const name& get##name() const {            \
    if(cameraID == CameraInfo::Top) {        \
      return the##name##Top; \
    } else {                                 \
      return the##name;      \
    }                                        \
  }

class CamMatErrorFunction
{
private:

    const CameraMatrixOffset&      camMatOffset;
    const KinematicChain&          kinematicChain;
    const ScanLineEdgelPercept&    theScanLineEdgelPercept;
    const ScanLineEdgelPerceptTop& theScanLineEdgelPerceptTop;
    const CameraInfo&              theCameraInfo;
    const CameraInfoTop&           theCameraInfoTop;
    const FieldInfo&               fieldInfo;

    DOUBLE_CAM_IN_ERROR_FUNCTION(ScanLineEdgelPercept);
    DOUBLE_CAM_IN_ERROR_FUNCTION(CameraInfo);

public:
    CamMatErrorFunction(const CameraMatrixOffset& camMatOffset, const KinematicChain& kinematicChain, const ScanLineEdgelPercept& scanLineEdgelPercept, const ScanLineEdgelPerceptTop& scanLineEdgelPerceptTop, const CameraInfo& cameraInfo, const CameraInfoTop& cameraInfoTop, const FieldInfo& fieldInfo):
        camMatOffset(camMatOffset),
        kinematicChain(kinematicChain),
        theScanLineEdgelPercept(scanLineEdgelPercept),
        theScanLineEdgelPerceptTop(scanLineEdgelPerceptTop),
        theCameraInfo(cameraInfo),
        theCameraInfoTop(cameraInfoTop),
        fieldInfo(fieldInfo)
    {}

    CameraInfo::CameraID cameraID;

    double operator()(Eigen::Matrix<double, 1, 2> parameter){
        double total_sum = 0;

        Vector2d offset(parameter(0), parameter(1));

        CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrix(
                    kinematicChain,
                    NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                    NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                    camMatOffset.correctionOffset[cameraID] + offset
                    );

        std::vector<Vector2d> edgelProjections;
        edgelProjections.resize(getScanLineEdgelPercept().pairs.size());

        // project edgels pairs to field
        for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
        {
            const EdgelT<double>& edgelOne = getScanLineEdgelPercept().pairs[i];

            CameraGeometry::imagePixelToFieldCoord(
                        tmpCM, getCameraInfo(),
                        edgelOne.point.x,
                        edgelOne.point.y,
                        0.0,
                        edgelProjections[i]);
        }

        // for all Egels in edgels
        for(std::vector<Vector2d>::const_iterator iter = edgelProjections.begin(); iter != edgelProjections.end(); ++iter){

            const Vector2d& seen_point_relative = *iter;

            Pose2D   robotPose;
            Vector2d seen_point_global = robotPose * seen_point_relative;

            LinesTable::NamedPoint line_point_global = fieldInfo.fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);

            // there is no such line
            if(line_point_global.id == -1) {
                continue;
            }

            total_sum += (seen_point_global - line_point_global.position).abs();
        }

        return total_sum;
    }
};

class CameraMatrixCorrectorV2: public CameraMatrixCorrectorV2Base
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
  CameraInfo::CameraID cameraID;

  typedef double (CameraMatrixCorrectorV2::*ErrorFunction)(double, double);

  GaussNewtonMinimizer<1, 2, CamMatErrorFunction> gn_minimizer;

  void calibrate();
  void reset_calibration();
  CamMatErrorFunction lineMatchingError;

  DOUBLE_CAM_PROVIDE(CameraMatrixCorrectorV2,DebugImageDrawings);
  DOUBLE_CAM_REQUIRE(CameraMatrixCorrectorV2,CameraMatrix);
  DOUBLE_CAM_REQUIRE(CameraMatrixCorrectorV2,ScanLineEdgelPercept);
  DOUBLE_CAM_REQUIRE(CameraMatrixCorrectorV2,CameraInfo);
};

#endif //_CameraMatrixCorrectorV2_h_
