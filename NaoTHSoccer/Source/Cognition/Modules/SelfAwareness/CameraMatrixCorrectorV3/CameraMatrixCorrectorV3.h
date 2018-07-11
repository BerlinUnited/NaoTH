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
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
#include "Representations/Perception/LineGraphPercept.h"

#include "Representations/Modeling/KinematicChain.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/Request/MotionRequest.h"

#include "Representations/Infrastructure/SoundData.h"

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

#include <Tools/Math/LevenbergMarquardtMinimizer.h>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CamMatErrorFunctionV3)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugModify)

  REQUIRE(FieldInfo)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
END_DECLARE_MODULE(CamMatErrorFunctionV3)

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

  PROVIDE(HeadMotionRequest)
  PROVIDE(MotionRequest)
  PROVIDE(CameraMatrixOffsetV3)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(CameraMatrixCorrectorV3)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////
/// \brief The CameraMatrixCorrectorV3 class

// HACK: shouldn't be a module, should be a service... wait and see what the future holds ;)
class CamMatErrorFunctionV3 : public CamMatErrorFunctionV3Base
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
    CalibrationData calibrationData;
    size_t numberOfResudials;

    const CameraInfo& getCameraInfo(int cameraID) const {
      if(cameraID == CameraInfo::Top) {
        return CamMatErrorFunctionV3Base::getCameraInfoTop();
      } else {
        return CamMatErrorFunctionV3Base::getCameraInfo();
      }
    }

    const std::vector<EdgelD>& getEdgelsInImage(std::vector<CalibrationDataSample>::const_iterator& sample, int cameraID) const {
      if(cameraID == CameraInfo::Top) {
        return sample->lineGraphPercept.edgelsInImageTop;
      } else {
        return sample->lineGraphPercept.edgelsInImage;
      }
    }

    void actual_plotting(const Eigen::Matrix<double, 11, 1>& parameter, CameraInfo::CameraID cameraID) const;

public:
    CamMatErrorFunctionV3()
      : numberOfResudials(0)
    {
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:only_bottom", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:only_top", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:draw_projected_edgels", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:draw_matching_global",  "", false);
    }

    void execute(){}

    Eigen::VectorXd operator()(const Eigen::Matrix<double, 11, 1>& parameter) const;

    size_t getNumberOfResudials() const {
        return numberOfResudials;
    }

    void add(const CalibrationDataSample& c_data_sample) 
    {
        int numNewResudials = 
          (c_data_sample.lineGraphPercept.edgelsInImage.empty()   ? 0 : 1) + 
          (c_data_sample.lineGraphPercept.edgelsInImageTop.empty()? 0 : 1);
        
        if (numNewResudials > 0) {
            calibrationData.push_back(c_data_sample);
            numberOfResudials += numNewResudials;
        }
    }

    void clear(){
        calibrationData.clear();
        numberOfResudials = 0;
    }

    void plot_CalibrationData(const Eigen::Matrix<double, 11, 1>& parameter){
        bool only_bottom = false;
        DEBUG_REQUEST("CamMatErrorFunctionV3:debug_drawings:only_bottom",
                      only_bottom = true;
        );

        bool only_top = false;
        DEBUG_REQUEST("CamMatErrorFunctionV3:debug_drawings:only_top",
                      only_top = true;
        );

        if(only_bottom && only_top){
            actual_plotting(parameter, CameraInfo::Bottom);
            actual_plotting(parameter, CameraInfo::Top);
        } else if (only_bottom) {
            actual_plotting(parameter, CameraInfo::Bottom);
        } else if (only_top) {
            actual_plotting(parameter, CameraInfo::Top);
        } else {
            actual_plotting(parameter, CameraInfo::Bottom);
            actual_plotting(parameter, CameraInfo::Top);
        }
    }
};

// HACK: shouldn't be a ModuleManager but has to be because of the CamMatErrorFunctionV3... see above
class CameraMatrixCorrectorV3: public CameraMatrixCorrectorV3Base, public ModuleManager
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  CameraMatrixCorrectorV3();
  ~CameraMatrixCorrectorV3();

  void execute();

private:

  std::vector<Vector2d> target_points;
  std::vector<Vector2d>::const_iterator current_target;

  Eigen::Matrix<double, 11, 1> cam_mat_offsets;

  LevenbergMarquardtMinimizer<CamMatErrorFunctionV3> lm_minimizer;

  bool calibrate();
  void reset_calibration();
  bool collectingData();
  void sampling();
  void doItAutomatically();

  void writeToRepresentation();
  void readFromRepresentation();

  ModuleCreator<CamMatErrorFunctionV3>* theCamMatErrorFunctionV3;

  // for automatic calibration
  bool auto_cleared_data, auto_collected, auto_calibrated;
  bool play_collecting, play_calibrating, play_calibrated;
  RingBufferWithSum<double, 50> derrors;
  double last_error;
  FrameInfo last_frame_info;
};

#endif //_CameraMatrixCorrectorV3_h_
