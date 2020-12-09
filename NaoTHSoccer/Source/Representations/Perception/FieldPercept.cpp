#include "FieldPercept.h"

#include <Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

void naoth::Serializer<FieldPercept>::serialize(const FieldPercept &object, std::ostream &stream)
{

    naothmessages::FieldPercept all;

    naothmessages::Polygon *polyProto = all.mutable_fieldpoly();

    for (const Vector2i &point : object.getField().getPoints())
    {
        naothmessages::IntVector2 *pointProto = polyProto->add_points();

        pointProto->set_x(point.x);
        pointProto->set_y(point.y);
    }

    google::protobuf::io::OstreamOutputStream buf(&stream);
    all.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<FieldPercept>::deserialize(std::istream &stream, FieldPercept &object)
{
    naothmessages::FieldPercept msg;

    google::protobuf::io::IstreamInputStream buf(&stream);
    msg.ParseFromZeroCopyStream(&buf);

    const naothmessages::Polygon& polyProto = msg.fieldpoly();

    std::vector<Vector2i> points(polyProto.points_size());
    for (int i = 0; i < polyProto.points_size(); i++)
    {
        points[i] = Vector2i(polyProto.points(i).x(), polyProto.points(i).y());
        //std::cout << points[i].x << " " << points[i].y << std::endl;
    }

    Math::Polygon<int> fieldPoly(points);
    object.setField(fieldPoly);
}
