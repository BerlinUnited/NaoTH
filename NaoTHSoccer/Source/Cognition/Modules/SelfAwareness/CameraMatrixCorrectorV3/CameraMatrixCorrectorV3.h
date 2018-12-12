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

#include <Tools/Math/Optimizer.h>

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
  PROVIDE(CameraMatrixOffset)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(CameraMatrixCorrectorV3)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////
/// \brief The CameraMatrixCorrectorV3 class

// TODO: generalize and move it into the optimizer name space
template<class T>
class BoundedVariable{
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        typedef Eigen::Array<double, T::RowsAtCompileTime, 1> Bound;

    private:
        Bound lower_bound;
        Bound upper_bound;

    public:
        BoundedVariable():
            lower_bound(Bound::Constant(-std::numeric_limits<double>::infinity())),
            upper_bound(Bound::Constant( std::numeric_limits<double>::infinity()))
        {
            static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");
        }

        BoundedVariable(const Bound& lower_bound, const Bound& upper_bound):
            lower_bound(lower_bound),
            upper_bound(upper_bound)
        {
            static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required");
        }

        T bound(const T& value) const {
            Bound bounded = value.array().cwiseMax(lower_bound).cwiseMin(upper_bound);

            // shift by 2*pi to avoid problems at zero in fminsearch
            // otherwise, the initial simplex is vanishingly small
            T r = (2 * Math::pi + (2 * (bounded - lower_bound) / (upper_bound - lower_bound) - 1).asin()).matrix();
            return r;
        }

        T unbound(const T& value) const {
            Bound r = (value.array().sin() + 1) / 2 * (upper_bound - lower_bound) + lower_bound;
            return (r.cwiseMin(upper_bound)).cwiseMax(lower_bound).matrix();
        }
};

// HACK: shouldn't be a ModuleManager but has to be because of the CamMatErrorFunctionV3... see above
class CameraMatrixCorrectorV3: public CameraMatrixCorrectorV3Base, public ModuleManager
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

  Optimizer::GaussNewtonMinimizer<CamMatErrorFunctionV3, Parameter>* minimizer;

  Optimizer::GaussNewtonMinimizer<CamMatErrorFunctionV3, Parameter> 	    gn_minimizer;
  Optimizer::LevenbergMarquardtMinimizer<CamMatErrorFunctionV3, Parameter>  lm_minimizer;
  Optimizer::LevenbergMarquardtMinimizer2<CamMatErrorFunctionV3, Parameter> lm2_minimizer;

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

              PARAMETER_REGISTER(use_bounded_variable) = true;

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

// HACK: shouldn't be a module, should be a service... wait and see what the future holds ;)
class CamMatErrorFunctionV3 : public CamMatErrorFunctionV3Base
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

    const BoundedVariable<CameraMatrixCorrectorV3::Parameter> *bounds;

private:
    CalibrationData calibrationData;
    unsigned int numberOfResudials;

    const CameraInfo& getCameraInfo(int cameraID) const {
      if(cameraID == CameraInfo::Top) {
        return CamMatErrorFunctionV3Base::getCameraInfoTop();
      } else {
        return CamMatErrorFunctionV3Base::getCameraInfo();
      }
    }

    const std::vector<Vector2d>& getEdgelsInImage(std::vector<CalibrationDataSample>::const_iterator& sample, int cameraID) const {
      if(cameraID == CameraInfo::Top) {
        return sample->edgelsInImageTop;
      } else {
        return sample->edgelsInImage;
      }
    }

    void actual_plotting(const Eigen::Matrix<double, 11, 1>& parameter, CameraInfo::CameraID cameraID) const;

public:
    CamMatErrorFunctionV3() :
        bounds(nullptr),
        numberOfResudials(0)
    {
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:only_bottom", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:only_top", "", false);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:draw_projected_edgels", "", true);
        DEBUG_REQUEST_REGISTER("CamMatErrorFunctionV3:debug_drawings:draw_matching_global",  "", false);
    }

    void execute(){}

    Eigen::VectorXd operator()(const Eigen::Matrix<double, 11, 1>& parameter) const;

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

    void write_calibration_data_to_file();
    void read_calibration_data_from_file();
};
#endif //_CameraMatrixCorrectorV3_h_
