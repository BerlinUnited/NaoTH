/**
 * @file ScanLineEdgelPercept.cpp
 * 
 * Definition of class ScanLineEdgelPercept
 */ 

#include "ScanLineEdgelPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;


void Serializer<ScanLineEdgelPercept>::serialize(const ScanLineEdgelPercept& representation, std::ostream& stream)
{
  naothmessages::ScanLineEdgelPercept percept_msg;

  for(unsigned int i = 0; i < representation.endPoints.size(); i++)
  {
    const ScanLineEdgelPercept::EndPoint& point = representation.endPoints[i];
    naothmessages::ScanLineEndPoint& point_msg = *(percept_msg.add_endpoints());

    // posInImage
    point_msg.mutable_posinimage()->set_x(point.posInImage.x);
    point_msg.mutable_posinimage()->set_y(point.posInImage.y);

    // posOnField
    point_msg.mutable_posonfield()->set_x(point.posOnField.x);
    point_msg.mutable_posonfield()->set_y(point.posOnField.y);

    // color
    point_msg.set_color((naothmessages::Color) point.color);

    // ScanLineID
    point_msg.set_scanlineid(point.ScanLineID);
  }//end for

  // serialize the message
  google::protobuf::io::OstreamOutputStream buf(&stream);
  percept_msg.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<ScanLineEdgelPercept>::deserialize(std::istream& stream, ScanLineEdgelPercept& representation)
{
  // deserialize the message
  naothmessages::ScanLineEdgelPercept percept_msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  percept_msg.ParseFromZeroCopyStream(&buf);


  for(int i = 0; i < percept_msg.endpoints_size(); i++)
  {
    ScanLineEdgelPercept::EndPoint point;
    const naothmessages::ScanLineEndPoint& point_msg = percept_msg.endpoints(i);

    // posInImage
    point.posInImage.x = point_msg.posinimage().x();
    point.posInImage.y = point_msg.posinimage().y();
    

    // posOnField
    point.posOnField.x = point_msg.posonfield().x();
    point.posOnField.y = point_msg.posonfield().y();

    // color
    point.color = (ColorClasses::Color) point_msg.color();

    // ScanLineID
    point.ScanLineID = point_msg.scanlineid();

    representation.endPoints.push_back(point);
  }//end for
}//end deserialize