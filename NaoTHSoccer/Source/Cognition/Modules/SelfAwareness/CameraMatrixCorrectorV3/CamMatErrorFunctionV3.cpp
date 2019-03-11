#include "CamMatErrorFunctionV3.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <fstream>
#include "Tools/DataConversion.h"

void CamMatErrorFunctionV3::actual_plotting(const Parameter &p, naoth::CameraInfo::CameraID cameraID)
{
    Eigen::Matrix<double, 11, 1> parameter;
    if(bounds != nullptr){
        parameter = bounds->unbound(p);
    } else {
        parameter = p;
    }

    Vector2d offsetBody(parameter(0),parameter(1));
    Vector3d offsetHead(parameter(2), parameter(3),parameter(4));
    Vector3d offsetCam[naoth::CameraInfo::numOfCamera];
    offsetCam[naoth::CameraInfo::Top]    = Vector3d(parameter(5), parameter(6),parameter(7));
    offsetCam[naoth::CameraInfo::Bottom] = Vector3d(parameter(8), parameter(9),parameter(10));

    for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample)
    {
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

                    LinesTable::NamedPoint line_point_global = theFieldInfo.fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);

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

Eigen::VectorXd CamMatErrorFunctionV3::operator()(const Parameter& p) const
{
    Eigen::VectorXd r(numberOfResudials);

    Eigen::Matrix<double, 11, 1> parameter;
    if(bounds != nullptr){
        parameter = bounds->unbound(p);
    } else {
        parameter = p;
    }

    Vector2d offsetBody(parameter(0),parameter(1));
    Vector3d offsetHead(parameter(2), parameter(3),parameter(4));
    Vector3d offsetCam[naoth::CameraInfo::numOfCamera];
    offsetCam[naoth::CameraInfo::Top]    = Vector3d(parameter(5), parameter(6),parameter(7));
    offsetCam[naoth::CameraInfo::Bottom] = Vector3d(parameter(8), parameter(9),parameter(10));

    size_t idx = 0;
    size_t empty = 0;
    for(int cameraID = 0; cameraID < naoth::CameraInfo::numOfCamera; cameraID++)
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

                LinesTable::NamedPoint line_point_global = theFieldInfo.fieldLinesTable.get_closest_point(seen_point_global, LinesTable::all_lines);

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

        naoth::DataConversion::toMessage(iter->chestPose, *sample->mutable_chestpose());

        for(std::vector<Vector2d>::const_iterator iter2 = iter->edgelsInImage.begin(); iter2 != iter->edgelsInImage.end(); ++iter2){
            naothmessages::DoubleVector2 *edgel = sample->add_edgelsinimage();
            naoth::DataConversion::toMessage(*iter2,*edgel);
        }

        for(std::vector<Vector2d>::const_iterator iter2 = iter->edgelsInImageTop.begin(); iter2 != iter->edgelsInImageTop.end(); ++iter2){
            naothmessages::DoubleVector2 *edgel = sample->add_edgelsinimagetop();
            naoth::DataConversion::toMessage(*iter2,*edgel);
        }

        naoth::DataConversion::toMessage(iter->orientation, *sample->mutable_orientation());
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

       naoth::DataConversion::fromMessage(*(msg_sample->mutable_chestpose()), sample.chestPose);

       sample.edgelsInImage.resize(static_cast<size_t>(msg_sample->edgelsinimage_size()));
       for(int i = 0; i < msg_sample->edgelsinimage_size(); ++i){
           naoth::DataConversion::fromMessage(msg_sample->edgelsinimage(i), sample.edgelsInImage[static_cast<size_t>(i)]);
       }

       sample.edgelsInImageTop.resize(static_cast<size_t>(msg_sample->edgelsinimagetop_size()));
       for(int i = 0; i < msg_sample->edgelsinimagetop_size(); ++i){
           naoth::DataConversion::fromMessage(msg_sample->edgelsinimagetop(i), sample.edgelsInImageTop[static_cast<size_t>(i)]);
       }

       naoth::DataConversion::fromMessage(*(msg_sample->mutable_orientation()), sample.orientation);
       sample.headPitch = msg_sample->headpitch();
       sample.headYaw   = msg_sample->headyaw();
    }

    in.close();
}
