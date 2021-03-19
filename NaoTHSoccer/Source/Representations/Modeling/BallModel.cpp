#include "BallModel.h"

#include <Representations.pb.h>
#include <Tools/DataConversion.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

void naoth::Serializer<BallModel>::serialize(const BallModel &object, std::ostream &stream)
{

    naothmessages::BallModel msg;

    DataConversion::toMessage(object.position, *(msg.mutable_position()));
    DataConversion::toMessage(object.speed, *(msg.mutable_speed()));
    DataConversion::toMessage(object.positionPreview, *(msg.mutable_positionpreview()));
    DataConversion::toMessage(object.positionPreviewInLFoot, *(msg.mutable_positionpreviewinlfoot()));
    DataConversion::toMessage(object.positionPreviewInRFoot, *(msg.mutable_positionpreviewinrfoot()));

    msg.set_valid(object.valid);
    msg.set_knows(object.knows);

    google::protobuf::io::OstreamOutputStream buf(&stream);
    msg.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<BallModel>::deserialize(std::istream &stream, BallModel &object)
{
    naothmessages::BallModel msg;

    google::protobuf::io::IstreamInputStream buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);

    DataConversion::fromMessage(msg.position(), object.position);
    DataConversion::fromMessage(msg.speed(), object.speed);
    DataConversion::fromMessage(msg.positionpreview(), object.positionPreview);
    DataConversion::fromMessage(msg.positionpreviewinlfoot(), object.positionPreviewInLFoot);
    DataConversion::fromMessage(msg.positionpreviewinrfoot(), object.positionPreviewInRFoot);

    object.valid = msg.valid();
    object.knows = msg.knows();
}
