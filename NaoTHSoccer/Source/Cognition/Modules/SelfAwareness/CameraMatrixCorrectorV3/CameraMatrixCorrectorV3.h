/**
* @file CameraMatrixCorrectorV3.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/

#ifndef _CameraMatrixCorrectorV3_h_
#define _CameraMatrixCorrectorV3_h_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/CameraInfo.h>
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
#include "Representations/Perception/LineGraphPercept.h"

#include "Representations/Modeling/KinematicChain.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"

#include "Representations/Infrastructure/SoundData.h"

#include "Tools/DoubleCamHelpers.h"
#include <Tools/NaoInfo.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Representations/Infrastructure/FrameInfo.h"

#include <Tools/Math/Optimizer.h>
#include "CamMatErrorFunctionV3.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CameraMatrixCorrectorV3)
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
  REQUIRE(FieldInfo)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(CameraMatrixOffset)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(CameraMatrixCorrectorV3)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class CameraMatrixCorrectorV3: public CameraMatrixCorrectorV3Base
{
public:
  CameraMatrixCorrectorV3();
  ~CameraMatrixCorrectorV3();

  void execute();

  typedef Eigen::Matrix<double, 11, 1> Parameter;

private:

  std::vector<Vector2d> target_points;
  std::vector<Vector2d>::const_iterator current_target;

  Parameter cam_mat_offsets;
  BoundedVariable<Parameter> bounds;

  CamMatErrorFunctionV3 theCamMatErrorFunctionV3;

  Optimizer::GaussNewtonMinimizer<CamMatErrorFunctionV3, Parameter>* minimizer;

  Optimizer::GaussNewtonMinimizer<CamMatErrorFunctionV3, Parameter>         gn_minimizer;
  Optimizer::LevenbergMarquardtMinimizer<CamMatErrorFunctionV3, Parameter>  lm_minimizer;
  Optimizer::LevenbergMarquardtMinimizer2<CamMatErrorFunctionV3, Parameter> lm2_minimizer;

  bool calibrate();
  void reset_calibration();
  bool collectingData();
  void sampling();
  void doItAutomatically();

  void writeToRepresentation();
  void readFromRepresentation();

  // for automatic calibration
  bool auto_cleared_data, auto_collected, auto_calibrated;
  bool play_collecting, play_calibrating, play_calibrated;
  RingBufferWithSum<double, 50> derrors;
  double last_error;
  FrameInfo last_frame_info;

  // for reading and writing calibration data
  bool read_calibration_data, written_calibration_data;

  class CMCParameters: public ParameterList
  {
      public:
          CMCParameters() : ParameterList("CameraMatrixCorrectorV3")
          {
              PARAMETER_ANGLE_REGISTER(lower.body_x)    = -1;
              PARAMETER_ANGLE_REGISTER(lower.body_y)    = -1;
              PARAMETER_ANGLE_REGISTER(lower.head_x)    = -1;
              PARAMETER_ANGLE_REGISTER(lower.head_y)    = -1;
              PARAMETER_ANGLE_REGISTER(lower.head_z)    = -1;
              PARAMETER_ANGLE_REGISTER(lower.cam_x)     = -1;
              PARAMETER_ANGLE_REGISTER(lower.cam_y)     = -1;
              PARAMETER_ANGLE_REGISTER(lower.cam_z)     = -1;
              PARAMETER_ANGLE_REGISTER(lower.cam_top_x) = -1;
              PARAMETER_ANGLE_REGISTER(lower.cam_top_y) = -1;
              PARAMETER_ANGLE_REGISTER(lower.cam_top_z) = -1;

              PARAMETER_ANGLE_REGISTER(upper.body_x)    = 1;
              PARAMETER_ANGLE_REGISTER(upper.body_y)    = 1;
              PARAMETER_ANGLE_REGISTER(upper.head_x)    = 1;
              PARAMETER_ANGLE_REGISTER(upper.head_y)    = 1;
              PARAMETER_ANGLE_REGISTER(upper.head_z)    = 1;
              PARAMETER_ANGLE_REGISTER(upper.cam_x)     = 1;
              PARAMETER_ANGLE_REGISTER(upper.cam_y)     = 1;
              PARAMETER_ANGLE_REGISTER(upper.cam_z)     = 1;
              PARAMETER_ANGLE_REGISTER(upper.cam_top_x) = 1;
              PARAMETER_ANGLE_REGISTER(upper.cam_top_y) = 1;
              PARAMETER_ANGLE_REGISTER(upper.cam_top_z) = 1;

              PARAMETER_REGISTER(use_bounded_variable) = false;

              syncWithConfig();
          }

          struct {
              double body_x;
              double body_y;

              double head_x;
              double head_y;
              double head_z;

              double cam_x;
              double cam_y;
              double cam_z;

              double cam_top_x;
              double cam_top_y;
              double cam_top_z;
          } lower, upper;

          bool use_bounded_variable;
  } cmc_params;
};

#endif //_CameraMatrixCorrectorV3_h_
