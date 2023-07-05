#include "LineCamMatErrorFunctionV3.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <fstream>
#include "Tools/DataConversion.h"

void LineCamMatErrorFunctionV3::actual_plotting(const Parameter &p, naoth::CameraInfo::CameraID cameraID)
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

            tmpCM.translation += Vector3d(global_position->x, global_position->y, 0); // move around on field
            tmpCM.rotation = RotationMatrix::getRotationZ(*global_orientation) * tmpCM.rotation;

            std::vector<Math::LineSegment> lineProjections;
            lineProjections.resize(getLinesInImage(sample,cameraID).size());

            // draw projected edgels to field
            for(size_t i = 0; i < getLinesInImage(sample,cameraID).size(); i++)
            {
                const Math::LineSegment& line_in_image = getLinesInImage(sample,cameraID)[i];

                Vector2d begin;
                CameraGeometry::imagePixelToFieldCoord(
                            tmpCM, getCameraInfo(cameraID),
                            line_in_image.begin().x,
                            line_in_image.begin().y,
                            0.0,
                            begin);

                Vector2d end;
                CameraGeometry::imagePixelToFieldCoord(
                            tmpCM, getCameraInfo(cameraID),
                            line_in_image.end().x,
                            line_in_image.end().y,
                            0.0,
                            end);

                Math::LineSegment line_on_field(begin, end);
                lineProjections[i] = line_on_field;

                DEBUG_REQUEST("LineCamMatErrorFunctionV3:debug_drawings:draw_projected_lines",
                        FIELD_DRAWING_CONTEXT;
                        PEN("000000", 50);
                        LINE(line_on_field.begin().x, line_on_field.begin().y, line_on_field.end().x, line_on_field.end().y);
                );
            }

            DEBUG_REQUEST("LineCamMatErrorFunctionV3:debug_drawings:draw_projected_lines",
                    FIELD_DRAWING_CONTEXT;
                    PEN("FF0000", 10);
                    ROBOT(0,0,0);
            );

            /*
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
            */
    }
}

Eigen::VectorXd LineCamMatErrorFunctionV3::operator()(const Parameter& p) const
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

    lineProjections.clear();

    for(int cameraID = 0; cameraID < naoth::CameraInfo::numOfCamera; cameraID++)
    {
        for(CalibrationData::const_iterator sample = calibrationData.begin(); sample != calibrationData.end(); ++sample)
        {
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

            tmpCM.translation += Vector3d(global_position->x, global_position->y, 0); // move around on field
            tmpCM.rotation = RotationMatrix::getRotationZ(*global_orientation) * tmpCM.rotation;

            // project lines to field
            for(size_t i = 0; i < getLinesInImage(sample,cameraID).size(); i++)
            {
                const Math::LineSegment& line_in_image = getLinesInImage(sample,cameraID)[i];

                Vector2d begin;
                CameraGeometry::imagePixelToFieldCoord(
                            tmpCM, getCameraInfo(cameraID),
                            line_in_image.begin().x,
                            line_in_image.begin().y,
                            0.0,
                            begin);

                Vector2d end;
                CameraGeometry::imagePixelToFieldCoord(
                            tmpCM, getCameraInfo(cameraID),
                            line_in_image.end().x,
                            line_in_image.end().y,
                            0.0,
                            end);

                lineProjections.emplace_back(begin, end);
            }
        }
    }

    Eigen::VectorXd r(numberOfResudials);

    double total_sum = 0.0;
    for (size_t i = 0; i < lineProjections.size(); ++i) {
      for (size_t j = i+1; j < lineProjections.size(); ++j) {
        double dot = lineProjections[i].getDirection() * lineProjections[j].getDirection();
        double dot2 = dot*dot;
        double err = dot2*(1-dot2);
        total_sum -= err;
      }
    }
    
    size_t idx = 0;
    r(idx) = -total_sum;


    //ASSERT(empty+idx == calibrationData.size()*2);
    //ASSERT(idx == numberOfResudials);
    return r;
}


void LineCamMatErrorFunctionV3::write_calibration_data_to_file(){
    /*
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
    */
}

void LineCamMatErrorFunctionV3::read_calibration_data_from_file(){
    /*
    std::ifstream in;
    in.open("/tmp/calibration_data", std::ifstream::binary);

    naothmessages::CalibrationDataCMC message;
    message.ParseFromIstream(&in);

    numberOfResudials = 0;
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
       if(sample.edgelsInImage.size() > 0){
           ++numberOfResudials;
       }

       sample.edgelsInImageTop.resize(static_cast<size_t>(msg_sample->edgelsinimagetop_size()));
       for(int i = 0; i < msg_sample->edgelsinimagetop_size(); ++i){
           naoth::DataConversion::fromMessage(msg_sample->edgelsinimagetop(i), sample.edgelsInImageTop[static_cast<size_t>(i)]);
       }
       if(sample.edgelsInImageTop.size() > 0){
           ++numberOfResudials;
       }

       naoth::DataConversion::fromMessage(*(msg_sample->mutable_orientation()), sample.orientation);
       sample.headPitch = msg_sample->headpitch();
       sample.headYaw   = msg_sample->headyaw();
    }

    in.close();
    */
}

