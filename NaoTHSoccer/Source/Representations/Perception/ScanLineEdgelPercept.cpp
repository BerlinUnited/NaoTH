/**
 * @file ScanLineEdgelPercept.cpp
 * 
 * Definition of class ScanLineEdgelPercept
 */ 

#include "ScanLineEdgelPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;


void Serializer<ScanLineEdgelPercept>::serialize(const ScanLineEdgelPercept& representation, std::ostream& stream)
{
  // EVIL HACK: recreating these messages can be very expensive. For now we just make them static.
  static naothmessages::ScanLineEdgelPercept percept_msg;
  percept_msg.Clear();

  for(size_t i = 0; i < representation.endPoints.size(); i++)
  {
    const ScanLineEdgelPercept::EndPoint& point = representation.endPoints[i];
    naothmessages::ScanLineEndPoint& point_msg = *(percept_msg.add_endpoints());

    DataConversion::toMessage(point.posInImage, *point_msg.mutable_posinimage());
    DataConversion::toMessage(point.posOnField, *point_msg.mutable_posonfield());

    point_msg.set_color((naothmessages::Color) point.color);
    point_msg.set_scanlineid(point.ScanLineID);
  }//end for

  for(size_t i = 0; i < representation.edgels.size(); i++)
  {
    const Edgel& edgel = representation.edgels[i];
    naothmessages::Edgel& edgel_msg = *(percept_msg.add_edgels());

    DataConversion::toMessage(edgel.point, *edgel_msg.mutable_point());
    DataConversion::toMessage(edgel.direction, *edgel_msg.mutable_direction());
  }

  for(size_t i = 0; i < representation.pairs.size(); i++)
  {
    const ScanLineEdgelPercept::EdgelPair& pair = representation.pairs[i];
    naothmessages::EdgelPair& pair_msg = *(percept_msg.add_pairs());

    pair_msg.set_begin(pair.begin);
    pair_msg.set_end(pair.end);
    pair_msg.set_id(pair.id);
    pair_msg.set_adaptive(pair.adaptive);
  }

  // serialize the message
  google::protobuf::io::OstreamOutputStream buf(&stream);
  percept_msg.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<ScanLineEdgelPercept>::deserialize(std::istream& stream, ScanLineEdgelPercept& representation)
{
  representation.reset();

  // deserialize the message
  naothmessages::ScanLineEdgelPercept percept_msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  percept_msg.ParseFromZeroCopyStream(&buf);

  unsigned int size = static_cast<unsigned int>(percept_msg.endpoints_size());
  representation.endPoints.resize(size);
  for(unsigned int i = 0; i < size; i++)
  {
    ScanLineEdgelPercept::EndPoint& point = representation.endPoints[i];
    const naothmessages::ScanLineEndPoint& point_msg = percept_msg.endpoints(static_cast<int>(i));

    DataConversion::fromMessage(point_msg.posinimage(), point.posInImage);
    DataConversion::fromMessage(point_msg.posonfield(), point.posOnField);
    point.color = (ColorClasses::Color) point_msg.color();
    point.ScanLineID = point_msg.scanlineid();
  }

  size = static_cast<unsigned int>(percept_msg.edgels_size());
  representation.edgels.resize(size);
  for(unsigned int i = 0; i < size; i++)
  {
    Edgel& edgel = representation.edgels[i];
    const naothmessages::Edgel& edgel_msg = percept_msg.edgels(static_cast<int>(i));

    DataConversion::fromMessage(edgel_msg.point(), edgel.point);
    DataConversion::fromMessage(edgel_msg.direction(), edgel.direction);
  }

  size = static_cast<unsigned int>(percept_msg.pairs_size());
  representation.pairs.resize(size);
  for(unsigned int i = 0; i < size; i++)
  {
    const naothmessages::EdgelPair& pair_msg = percept_msg.pairs(static_cast<int>(i));
    ScanLineEdgelPercept::EdgelPair& pair = representation.pairs[i];
    
    pair.begin = pair_msg.begin();
    pair.end = pair_msg.end();
    pair.id = pair_msg.id();
    pair.adaptive = pair_msg.adaptive();

    // TODO: is it ok here?
    // recalculate the values for the center edgel like in ScanLineEdgelDetector.h
    const Edgel& end = representation.edgels[static_cast<unsigned int>(pair.end)];
    const Edgel& begin = representation.edgels[static_cast<unsigned int>(pair.begin)];
    pair.point = Vector2d(begin.point + end.point)*0.5;
    pair.direction = (begin.direction - end.direction).normalize();
  }
}//end deserialize
