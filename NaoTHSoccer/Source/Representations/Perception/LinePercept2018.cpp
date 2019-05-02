#include "LinePercept2018.h"

#include <Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

void naoth::Serializer<ShortLinePercept>::serialize(const ShortLinePercept &object, std::ostream &stream)
{

    naothmessages::LinePercept2018 all;

    for (Math::LineSegment const &seg : object.fieldLineSegments)
    {
        naothmessages::LineSegment *segProto = all.add_fieldlinesegments();

        DataConversion::toMessage(seg.getBase(), *(segProto->mutable_base()));
        DataConversion::toMessage(seg.getDirection(), *(segProto->mutable_direction()));

        segProto->set_length(seg.getLength());
    }

    google::protobuf::io::OstreamOutputStream buf(&stream);
    all.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<ShortLinePercept>::deserialize(std::istream &stream, ShortLinePercept &object)
{
    naothmessages::LinePercept2018 msg;

    google::protobuf::io::IstreamInputStream buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);

    for (int i = 0; i < msg.fieldlinesegments_size(); i++)
    {
        Vector2d base;
        Vector2d direction;

        DataConversion::fromMessage(msg.fieldlinesegments(i).base(), base);
        DataConversion::fromMessage(msg.fieldlinesegments(i).direction(), direction);

        object.fieldLineSegments.push_back(Math::LineSegment(base, direction, msg.fieldlinesegments(i).length()));
    }
}

void naoth::Serializer<RansacLinePercept>::serialize(const RansacLinePercept &object, std::ostream &stream)
{

    naothmessages::LinePercept2018 all;

    for (Math::LineSegment const &seg : object.fieldLineSegments)
    {
        naothmessages::LineSegment *segProto = all.add_fieldlinesegments();

        DataConversion::toMessage(seg.getBase(), *(segProto->mutable_base()));
        DataConversion::toMessage(seg.getDirection(), *(segProto->mutable_direction()));

        segProto->set_length(seg.getLength());
    }

    google::protobuf::io::OstreamOutputStream buf(&stream);
    all.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<RansacLinePercept>::deserialize(std::istream &stream, RansacLinePercept &object)
{
    naothmessages::LinePercept2018 msg;

    google::protobuf::io::IstreamInputStream buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);

    for (int i = 0; i < msg.fieldlinesegments_size(); i++)
    {
        Vector2d base;
        Vector2d direction;

        DataConversion::fromMessage(msg.fieldlinesegments(i).base(), base);
        DataConversion::fromMessage(msg.fieldlinesegments(i).direction(), direction);

        object.fieldLineSegments.push_back(Math::LineSegment(base, direction, msg.fieldlinesegments(i).length()));
    }
}

void naoth::Serializer<RansacCirclePercept2018>::serialize(const RansacCirclePercept2018 &object, std::ostream &stream)
{

    naothmessages::RansacCirclePercept2018 out;

    out.set_wasseen(object.wasSeen);
    DataConversion::toMessage(object.center, *(out.mutable_middlecirclecenter()));

    google::protobuf::io::OstreamOutputStream buf(&stream);
    out.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<RansacCirclePercept2018>::deserialize(std::istream &stream, RansacCirclePercept2018 &object)
{
    naothmessages::RansacCirclePercept2018 msg;

    google::protobuf::io::IstreamInputStream buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);

    object.wasSeen = msg.wasseen();
    DataConversion::fromMessage(msg.middlecirclecenter(), object.center);
}

