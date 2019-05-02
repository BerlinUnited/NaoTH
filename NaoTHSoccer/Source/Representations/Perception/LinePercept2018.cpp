#include "LinePercept2018.h"

#include <Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

void naoth::Serializer<ShortLinePercept>::serialize(const ShortLinePercept &object, std::ostream &stream)
{

    naothmessages::LinePercept2018 all;

    for (Math::LineSegment const &seg : object.fieldLineSegments)
    {
        naothmessages::LineSegment *segProto = all.add_fieldlinesegments();

        segProto->mutable_base()->set_x(seg.getBase().x);
        segProto->mutable_base()->set_y(seg.getBase().y);
        segProto->mutable_direction()->set_x(seg.getDirection().x);
        segProto->mutable_direction()->set_y(seg.getDirection().y);
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
        Vector2d base(msg.fieldlinesegments(i).base().x(), msg.fieldlinesegments(i).base().y());
        Vector2d direction(msg.fieldlinesegments(i).direction().x(), msg.fieldlinesegments(i).direction().y());

        object.fieldLineSegments.push_back(Math::LineSegment(base, direction, msg.fieldlinesegments(i).length()));
    }
}
