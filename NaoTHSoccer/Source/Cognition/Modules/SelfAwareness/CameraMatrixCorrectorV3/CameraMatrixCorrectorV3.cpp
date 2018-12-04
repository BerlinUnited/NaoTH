/**
* @file CameraMatrixCorrectorV3.cpp
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/

#include "CameraMatrixCorrectorV3.h"

#include <Messages/Representations.pb.h>
#include <fstream>
#include <google/protobuf/io/zero_copy_stream_impl.h>

//#include <array>

void CamMatErrorFunctionV3::actual_plotting(const Eigen::Matrix<double, 11, 1>& parameter, CameraInfo::CameraID cameraID) const
{
    for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample)
    {
            Vector2d offsetBody(parameter(0),parameter(1));
            Vector3d offsetHead(parameter(2), parameter(3),parameter(4));
            Vector3d offsetCam[CameraInfo::numOfCamera];
            offsetCam[CameraInfo::Top]    = Vector3d(parameter(5), parameter(6),parameter(7));
            offsetCam[CameraInfo::Bottom] = Vector3d(parameter(8), parameter(9),parameter(10));

            CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromChestPose(
                        sample->chestPose,
                        NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                        NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                        offsetBody,
                        offsetHead,
                        offsetCam[cameraID],
                        sample->headYaw,
                        sample->headPitch,
                        sample->orientation
                        );

            std::vector<Vector2d> edgelProjections;
            edgelProjections.resize(getEdgelsInImage(sample,cameraID).size());

            // draw projected edgels to field
            for(size_t i = 0; i < getEdgelsInImage(sample,cameraID).size(); i++)
            {
                const Vector2d& edgelOne = getEdgelsInImage(sample,cameraID)[i];

                CameraGeometry::imagePixelToFieldCoord(
                            tmpCM, getCameraInfo(cameraID),
                            edgelOne.x,
                            edgelOne.y,
                            0.0,
                            edgelProjections[i]);

                DEBUG_REQUEST("CamMatErrorFunctionV3:debug_drawings:draw_projected_edgels",
                        FIELD_DRAWING_CONTEXT;
                        PEN("000000", 10);
                        CIRCLE(edgelProjections[i].x, edgelProjections[i].y, 20);
                );
            }

            DEBUG_REQUEST("CamMatErrorFunctionV3:debug_drawings:draw_projected_edgels",
                    FIELD_DRAWING_CONTEXT;
                    PEN("FF0000", 10);
                    ROBOT(0,0,0);
            );

            DEBUG_REQUEST("CamMatErrorFunctionV3:debug_drawings:draw_matching_global",
            // determine distance to nearst field line and the total aberration
                for(std::vector<Vector2d>::const_iterator iter = edgelProjections.begin(); iter != edgelProjections.end(); ++iter)
                {

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
                ROBOT(0,0,0);
            );
    }
}

Eigen::VectorXd CamMatErrorFunctionV3::operator()(const Eigen::Matrix<double, 11, 1>& parameter) const
{
    Eigen::VectorXd r(numberOfResudials);

    Vector2d offsetBody(parameter(0),parameter(1));
    Vector3d offsetHead(parameter(2), parameter(3),parameter(4));
    Vector3d offsetCam[CameraInfo::numOfCamera];
    offsetCam[CameraInfo::Top]    = Vector3d(parameter(5), parameter(6),parameter(7));
    offsetCam[CameraInfo::Bottom] = Vector3d(parameter(8), parameter(9),parameter(10));

    size_t idx = 0;
    size_t empty = 0;
    for(int cameraID = 0; cameraID < CameraInfo::numOfCamera; cameraID++)
    {
        for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample)
        {
            if (getEdgelsInImage(sample,cameraID).empty()) {
                ++empty;
                continue;
            }

            CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrixFromChestPose(
                        //sample->kinematicChain,
                        sample->chestPose,
                        NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
                        NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
                        offsetBody,
                        offsetHead,
                        offsetCam[cameraID],
                        sample->headYaw,
                        sample->headPitch,
                        sample->orientation
                        );

            std::vector<Vector2d> edgelProjections;
            edgelProjections.resize(getEdgelsInImage(sample,cameraID).size());

            // project edgels pairs to field
            for(size_t i = 0; i < getEdgelsInImage(sample,cameraID).size(); i++)
            {
                const Vector2d& edgelOne = getEdgelsInImage(sample,cameraID)[i];

                CameraGeometry::imagePixelToFieldCoord(
                            tmpCM, getCameraInfo(cameraID),
                            edgelOne.x,
                            edgelOne.y,
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

            r(idx) = -total_sum; // the resudial is target - actual, i.e. 0 - total_sum
            ++idx;
        }
    }

    ASSERT(empty+idx == calibrationData.size()*2);
    ASSERT(idx == numberOfResudials);
    return r;
}

void CamMatErrorFunctionV3::write_calibration_data_to_file(){
    std::ofstream out;
    out.open("/tmp/calibration_data", std::ofstream::binary);

    naothmessages::CalibrationDataCMC message;

    message.set_numberofresudials(numberOfResudials);
    for(CalibrationData::const_iterator iter = calibrationData.begin(); iter != calibrationData.end(); ++iter){
        naothmessages::CalibrationDataCMC::CalibrationDataSampleV3 *sample = message.add_calibrationdata();

        DataConversion::toMessage(iter->chestPose, *sample->mutable_chestpose());

        for(std::vector<Vector2d>::const_iterator iter2 = iter->edgelsInImage.begin(); iter2 != iter->edgelsInImage.end(); ++iter2){
            naothmessages::DoubleVector2 *edgel = sample->add_edgelsinimage();
            DataConversion::toMessage(*iter2,*edgel);
        }

        for(std::vector<Vector2d>::const_iterator iter2 = iter->edgelsInImageTop.begin(); iter2 != iter->edgelsInImageTop.end(); ++iter2){
            naothmessages::DoubleVector2 *edgel = sample->add_edgelsinimagetop();
            DataConversion::toMessage(*iter2,*edgel);
        }

        DataConversion::toMessage(iter->orientation, *sample->mutable_orientation());
        sample->set_headpitch(iter->headPitch);
        sample->set_headyaw(iter->headYaw);
    }

    message.SerializeToOstream(&out);

    out.close();
}

void CamMatErrorFunctionV3::read_calibration_data_from_file(){
    std::ifstream in;
    in.open("/tmp/calibration_data", std::ifstream::binary);

    naothmessages::CalibrationDataCMC message;
    message.ParseFromIstream(&in);

    size_t size = static_cast<size_t>(message.calibrationdata_size());
    calibrationData.resize(size);
    for(size_t i = 0; i < size; ++i){
       naothmessages::CalibrationDataCMC::CalibrationDataSampleV3 *msg_sample = message.mutable_calibrationdata(static_cast<int>(i));
       CalibrationDataSample& sample = calibrationData[i];

       DataConversion::fromMessage(*(msg_sample->mutable_chestpose()), sample.chestPose);

       sample.edgelsInImage.resize(static_cast<size_t>(msg_sample->edgelsinimage_size()));
       for(int i = 0; i < msg_sample->edgelsinimage_size(); ++i){
           DataConversion::fromMessage(msg_sample->edgelsinimage(i), sample.edgelsInImage[static_cast<size_t>(i)]);
       }

       sample.edgelsInImageTop.resize(static_cast<size_t>(msg_sample->edgelsinimagetop_size()));
       for(int i = 0; i < msg_sample->edgelsinimagetop_size(); ++i){
           DataConversion::fromMessage(msg_sample->edgelsinimagetop(i), sample.edgelsInImageTop[static_cast<size_t>(i)]);
       }

       DataConversion::fromMessage(*(msg_sample->mutable_orientation()), sample.orientation);
       sample.headPitch = msg_sample->headpitch();
       sample.headYaw   = msg_sample->headyaw();
    }

    in.close();
}

CameraMatrixCorrectorV3::CameraMatrixCorrectorV3()
{
  getDebugParameterList().add(&getCameraMatrixOffset());

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:reset_calibration", "set the calibration offsets of the CM to 0", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:reset_minimizer", "reset lm parameters to initial values", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:collect_calibration_data", "collect the data for calibration", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:clear_calibration_data", "clears the data used for calibration", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:enable_CamMatErrorFunctionV3_drawings", "needed to be activated for error function drawings", true);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:automatic_mode","try to do automatic calibration", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:calibrate","",false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:optimizer:use_GN",  "use Gauss-Newton based optimizer during calibration", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:optimizer:use_LM",  "use Levenberg-Marquardt based optimizer during calibration (default)", true);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:optimizer:use_LM2", "use Levenberg-Marquardt based optimizer (smoothed update) during calibration", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:read_calibration_data_from_file", "", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:write_calibration_data_to_file",  "", false);

  theCamMatErrorFunctionV3 = registerModule<CamMatErrorFunctionV3>("CamMatErrorFunctionV3", true);
  read_calibration_data = written_calibration_data = false;

  auto_cleared_data = auto_collected = auto_calibrated = false;
  play_calibrated = play_calibrating = play_collecting = true;
  last_error = 0;

  minimizer = &lm_minimizer;

  // sampling coordinates
  std::array<double,4> pitchs {-20, -10 , 0, 10};
  std::vector<double> yaws;
  for (int i = -88; i <= 88; i=i+8) {
      yaws.push_back(i);
  }

  for(double pitch : pitchs){
      for(double yaw : yaws){
          target_points.push_back(Vector2d(yaw, pitch));
      }
  }

  current_target = target_points.begin();

  readFromRepresentation();
}

CameraMatrixCorrectorV3::~CameraMatrixCorrectorV3()
{
    getDebugParameterList().remove(&getCameraMatrixOffset());
}

void CameraMatrixCorrectorV3::execute()
{
  DEBUG_REQUEST("CameraMatrixV3:read_calibration_data_from_file",
    if(!read_calibration_data) {
      theCamMatErrorFunctionV3->getModuleT()->read_calibration_data_from_file();
      read_calibration_data = true;
    }
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:read_calibration_data_from_file",
    read_calibration_data = false;
  );

  DEBUG_REQUEST("CameraMatrixV3:write_calibration_data_to_file",
    if(!written_calibration_data) {
      theCamMatErrorFunctionV3->getModuleT()->write_calibration_data_to_file();
      written_calibration_data = true;
    }
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:write_calibration_data_to_file",
    written_calibration_data = false;
  );

  DEBUG_REQUEST("CameraMatrixV3:optimizer:use_GN",
    minimizer = &gn_minimizer;
  );

  DEBUG_REQUEST("CameraMatrixV3:optimizer:use_LM",
    minimizer = &lm_minimizer;
  );

  DEBUG_REQUEST("CameraMatrixV3:optimizer:use_LM2",
    minimizer = &lm2_minimizer;
  );

  if (getSoundPlayData().soundFile == "finished.wav"
      || getSoundPlayData().soundFile == "collectingdata.wav"
      || getSoundPlayData().soundFile == "calibrating.wav")
  {
    getSoundPlayData().mute = true;
    getSoundPlayData().soundFile = "";
  }

  // sit down if auto calibrated
  if(auto_calibrated){
    getMotionRequest().id = motion::sit;
    getMotionRequest().disable_relaxed_stand = false;
  } else {
    getMotionRequest().id = motion::stand;
    getMotionRequest().disable_relaxed_stand = true;
  }

  // enable debug drawings in manual and auto mode
  DEBUG_REQUEST("CameraMatrixV3:enable_CamMatErrorFunctionV3_drawings",
      theCamMatErrorFunctionV3->getModuleT()->plot_CalibrationData(cam_mat_offsets);
  );

  bool use_automatic_mode = false;
  DEBUG_REQUEST("CameraMatrixV3:automatic_mode",
    use_automatic_mode = true;
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:automatic_mode",
    // Note: wanted behavior because we want to save the "best" solution so far
    if(!auto_calibrated){
        writeToRepresentation();
        getCameraMatrixOffset().saveToConfig();
    }

    // reset to intial
    getHeadMotionRequest().id = HeadMotionRequest::hold;
    current_target = target_points.begin();

    auto_cleared_data = auto_collected = auto_calibrated = false;
    play_calibrated = play_calibrating = play_collecting = true;
    derrors.clear();
    last_error = 0;
    minimizer->reset();
  );

  DEBUG_REQUEST("CameraMatrixV3:reset_minimizer",
        minimizer->reset();
  );

  if(use_automatic_mode){
      if(!auto_calibrated){
          doItAutomatically();
      }
  } else { // manual mode
      DEBUG_REQUEST("CameraMatrixV3:clear_calibration_data",
        (theCamMatErrorFunctionV3->getModuleT())->clear();
      );

      DEBUG_REQUEST("CameraMatrixV3:collect_calibration_data",
        collectingData();
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:collect_calibration_data",
        // reset to initial
        getHeadMotionRequest().id = HeadMotionRequest::hold;
        current_target = target_points.begin();
      );

      DEBUG_REQUEST("CameraMatrixV3:calibrate",
          calibrate();
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:calibrate",
          writeToRepresentation();
          getCameraMatrixOffset().saveToConfig();
      );

      DEBUG_REQUEST("CameraMatrixV3:reset_calibration",
            reset_calibration();
            minimizer->reset();
            derrors.clear();
            last_error = 0;
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:reset_calibration",
          writeToRepresentation();
          getCameraMatrixOffset().saveToConfig();
      );
  }

  last_frame_info = getFrameInfo();
}

void CameraMatrixCorrectorV3::reset_calibration()
{
  getCameraMatrixOffset().body_rot = Vector2d();
  getCameraMatrixOffset().head_rot = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] = Vector3d();

  cam_mat_offsets = Eigen::Matrix<double, 11, 1>::Zero();
}

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV3::calibrate()
{
  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  Eigen::Matrix<double, 11, 1> epsilon = Eigen::Matrix<double, 11, 1>::Constant(1e-4);

  Parameter offset;
  bool valid = minimizer->minimizeOneStep(*(theCamMatErrorFunctionV3->getModuleT()), cam_mat_offsets, epsilon, offset);

  if(valid) {
      cam_mat_offsets += offset;
  }

  double de_dt = (minimizer->error - last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors.add(de_dt);
  }
  last_error = minimizer->error;

  PLOT("CameraMatrixV3:error", minimizer->error);
  PLOT("CameraMatrixV3:de/dt", de_dt);
  PLOT("CameraMatrixV3:avg_derror", derrors.getAverage());

  return ((  (derrors.getAverage() > -50) // average error decreases by less than this per second
           && derrors.isFull())
           || minimizer->step_failed());          // and we have a full history
}

// returns true, if the trajectory is starting again from beginning
bool CameraMatrixCorrectorV3::collectingData()
{
    if(current_target == target_points.end())
        return true;

    getHeadMotionRequest().id = HeadMotionRequest::goto_angle;
    getHeadMotionRequest().targetJointPosition.x = Math::fromDegrees(current_target->x);
    getHeadMotionRequest().targetJointPosition.y = Math::fromDegrees(current_target->y);
    getHeadMotionRequest().velocity = 20;
    MODIFY("CameraMatrixV3:collecting_velocity", getHeadMotionRequest().velocity);

    double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();
    double current_yaw   = Math::toDegrees(getSensorJointData().position[JointData::HeadYaw]);
    double current_pitch = Math::toDegrees(getSensorJointData().position[JointData::HeadPitch]);

    static double last_yaw(current_yaw);
    static double last_pitch(current_pitch);

    double vel_yaw   = (last_yaw   - current_yaw)/dt;
    double vel_pitch = (last_pitch - current_pitch)/dt;

    last_yaw   = current_yaw;
    last_pitch = current_pitch;

    // state transitions and triggering sampling
    bool target_reached =     fabs(current_yaw   - current_target->x) < 3
                           && fabs(current_pitch - current_target->y) < 3
                           && fabs(vel_yaw)   < 0.2
                           && fabs(vel_pitch) < 0.2;

    if(target_reached && current_target != target_points.end()){
        sampling();
        current_target++;
    }

    PLOT("CameraMatrixV3:vel_yaw",vel_yaw);
    PLOT("CameraMatrixV3:vel_pitch", vel_pitch);
    PLOT("CameraMatrixV3:target_reached", target_reached);

    return current_target == target_points.end();
}

void CameraMatrixCorrectorV3::sampling()
{
    LineGraphPercept lineGraphPercept(getLineGraphPercept());

    // TODO: ignore upper image if the center is above the horizon
    // HACK: ignore it if headpitch is smaller than -15
    //if(head_state == look_right_up || head_state == look_left_up
    //   || last_head_state == look_right_up || last_head_state == look_left_up){
    if(Math::toDegrees(getSensorJointData().position[JointData::HeadPitch]) < -15) {
        lineGraphPercept.edgelsInImageTop.clear();
    }

    (theCamMatErrorFunctionV3->getModuleT())->add(CamMatErrorFunctionV3::CalibrationDataSample(getKinematicChain().theLinks[KinematicChain::Torso].M,
                                                                lineGraphPercept,
                                                                getInertialModel().orientation,
                                                                getSensorJointData().position[JointData::HeadYaw],
                                                                getSensorJointData().position[JointData::HeadPitch]
                                                               ));
}

void CameraMatrixCorrectorV3::doItAutomatically()
{
    if(!auto_cleared_data) {
        (theCamMatErrorFunctionV3->getModuleT())->clear();
        readFromRepresentation();
        auto_cleared_data = true;
    }

    if(!auto_collected){
        auto_collected = collectingData();

        if(play_collecting){
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "collectingdata.wav";
            play_collecting = false;
        }
    }

    if(auto_collected && !auto_calibrated){
        auto_calibrated = calibrate();
        if(auto_calibrated){
            derrors.clear();
            minimizer->reset();
        }

        if(play_calibrating){
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "calibrating.wav";
            play_calibrating = false;
        }
    }

    if(auto_calibrated){
        writeToRepresentation();
        getCameraMatrixOffset().saveToConfig();

        if(play_calibrated) {
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "finished.wav";
            play_calibrated = false;
        }
    }
}

void CameraMatrixCorrectorV3::writeToRepresentation()
{
  getCameraMatrixOffset().body_rot += Vector2d(cam_mat_offsets(0),cam_mat_offsets(1));
  getCameraMatrixOffset().head_rot += Vector3d(cam_mat_offsets(2),cam_mat_offsets(3),cam_mat_offsets(4));

  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    += Vector3d(cam_mat_offsets(5),cam_mat_offsets(6),cam_mat_offsets(7));
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] += Vector3d(cam_mat_offsets(8),cam_mat_offsets(9),cam_mat_offsets(10));
}

void CameraMatrixCorrectorV3::readFromRepresentation(){
  cam_mat_offsets << getCameraMatrixOffset().body_rot.x,
                     getCameraMatrixOffset().body_rot.y,
                     getCameraMatrixOffset().head_rot.x,
                     getCameraMatrixOffset().head_rot.y,
                     getCameraMatrixOffset().head_rot.z,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Top].x,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Top].y,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Top].z,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Bottom].x,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Bottom].y,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Bottom].z;
}
