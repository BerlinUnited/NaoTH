/**
 * @file LinePercept.cpp
 * 
 * Definition of class LinePercept
 */ 

#include "LinePercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;

void LinePercept::print(ostream& stream) const
{
  for(unsigned int i = 0; i < lines.size(); i++)
  {
    const FieldLineSegment& line = lines[i];
    stream 
      << "== Line " << i << " == " << endl
      << "lineInImage = (" << line.lineInImage.segment.begin() << ") -- (" << line.lineInImage.segment.end() << ")" << endl
      << "lineOnField = (" << line.lineOnField.begin() << ") -- (" << line.lineOnField.end() << ")" << endl
      << "thickness = (" << line.lineInImage.thickness << ")" << endl
      << "angle = (" << line.lineInImage.angle << ")" << endl
    ;
  }//end for

  cout<<"\n";
  for(unsigned int i=0; i<flags.size(); i++)
  {
    stream 
      << "== Flag " << i << "==" << endl
      << "seenPosOnField = " << flags[i].seenPosOnField << endl
      << "absolutePosOnField = " << flags[i].absolutePosOnField << endl
      ;
  }//end for
}//end print

void Serializer<LinePercept>::serialize(const LinePercept& representation, std::ostream& stream)
{
  
  naothmessages::LinePercept p;

  // lines
  for(size_t i=0; i < representation.lines.size(); i++)
  {
    naothmessages::FieldLineSegment* segment = p.add_lines();
    const LinePercept::FieldLineSegment& line = representation.lines[i];

    //lines[i].fillProtobuf(newLineSegment);
    // TODO: put it somewhere else
    // lineInImage
    segment->mutable_lineinimage()->mutable_base()->set_x(line.lineInImage.segment.getBase().x);
    segment->mutable_lineinimage()->mutable_base()->set_y(line.lineInImage.segment.getBase().y);
    segment->mutable_lineinimage()->mutable_direction()->set_x(line.lineInImage.segment.getDirection().x);
    segment->mutable_lineinimage()->mutable_direction()->set_y(line.lineInImage.segment.getDirection().y);
    segment->mutable_lineinimage()->set_length(line.lineInImage.segment.getLength());

    // lineOnField
    segment->mutable_lineonfield()->mutable_base()->set_x(line.lineOnField.getBase().x);
    segment->mutable_lineonfield()->mutable_base()->set_y(line.lineOnField.getBase().y);
    segment->mutable_lineonfield()->mutable_direction()->set_x(line.lineOnField.getDirection().x);
    segment->mutable_lineonfield()->mutable_direction()->set_y(line.lineOnField.getDirection().y);
    segment->mutable_lineonfield()->set_length(line.lineOnField.getLength());
  }//end for

  // intersections
  for(size_t i=0; i < representation.intersections.size(); i++)
  {
    naothmessages::Intersection* intersection = p.add_intersections();
    const LinePercept::Intersection& percept = representation.intersections[i];
    //intersections[i].fillProtobuf(newIntersection);

    intersection->set_type((naothmessages::Intersection_IntersectionType) percept.getType());
    intersection->mutable_posinimage()->set_x(percept.getPos().x);
    intersection->mutable_posinimage()->set_y(percept.getPos().y);
    intersection->mutable_posonfield()->set_x(percept.getPosOnField().x);
    intersection->mutable_posonfield()->set_y(percept.getPosOnField().y);

    intersection->set_segmentoneindex(percept.getSegmentIndices()[0]);
    intersection->set_segmenttwoindex(percept.getSegmentIndices()[1]);
    intersection->set_segmentonedistance(percept.getSegmentsDistancesToIntersection()[0]);
    intersection->set_segmenttwodistance(percept.getSegmentsDistancesToIntersection()[1]);
  }//end for

  // middle circle
  if(representation.middleCircleWasSeen)
  {
    p.mutable_middlecirclecenter()->set_x(representation.middleCircleCenter.x);
    p.mutable_middlecirclecenter()->set_y(representation.middleCircleCenter.y);
  }
  else
  {
    p.clear_middlecirclecenter();
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize



void Serializer<LinePercept>::deserialize(std::istream& stream, LinePercept& representation)
{
  // reset the percept befor copy from stream
  representation.reset();

  // deserialize
  naothmessages::LinePercept p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParsePartialFromZeroCopyStream(&buf);

  // line segments
  for(int i=0; i < p.lines_size(); i++)
  {
    LinePercept::FieldLineSegment line;
    const naothmessages::FieldLineSegment& segment = p.lines(i);
    //line.readFromProtobuf(&p.lines(i));

    // lineInImage
    if(segment.has_lineinimage())
    {
      Vector2d base;
      base.x = segment.lineinimage().base().x();
      base.y = segment.lineinimage().base().y();

      Vector2d direction;
      direction.x = segment.lineinimage().direction().x();
      direction.y = segment.lineinimage().direction().y();

      double length = segment.lineinimage().length();

      line.lineInImage.segment = Math::LineSegment(base, base+direction*length);
    }//end if

    // lineOnField
    if(segment.has_lineonfield())
    {
      Vector2d base;
      base.x = segment.lineonfield().base().x();
      base.y = segment.lineonfield().base().y();

      Vector2d direction;
      direction.x = segment.lineonfield().direction().x();
      direction.y = segment.lineonfield().direction().y();

      double length = segment.lineonfield().length();
      line.lineOnField = Math::LineSegment(base,base+direction*length);
    }//end if

    representation.lines.push_back(line);
  }//end for


  // intersections
  for(int i=0; i < p.intersections_size(); i++)
  {
    LinePercept::Intersection percept;
    const naothmessages::Intersection& msg = p.intersections(i);
    //intersection.readFromProtobuf(&p.intersections(i));

    if(msg.has_type())
      percept.setType((Math::Intersection::IntersectionType) msg.type());

    if(msg.has_posinimage())
    {
      Vector2d pos;
      pos.x = msg.posinimage().x();
      pos.y = msg.posinimage().y();
      percept.setPosInImage(pos);
    }//end if

    if(msg.has_posonfield())
    {
      Vector2d pos;
      pos.x = msg.posonfield().x();
      pos.y = msg.posonfield().y();
      percept.setPosOnField(pos);
    }//end if

    if(msg.has_segmentoneindex() && msg.has_segmenttwoindex() && 
       msg.has_segmentonedistance() && msg.has_segmenttwodistance())
    {
      percept.setSegments(
        msg.segmentoneindex(), 
        msg.segmenttwoindex(),
        msg.segmentonedistance(),
        msg.segmenttwodistance());
    }//end if

    representation.intersections.push_back(percept);
  }//end for


  // middle circle
  representation.middleCircleWasSeen = false;
  if(p.has_middlecirclecenter())
  {
    representation.middleCircleWasSeen = true;
    representation.middleCircleCenter.x = p.middlecirclecenter().x();
    representation.middleCircleCenter.y = p.middlecirclecenter().y();
  }

  // 
  representation.lineWasSeen = !representation.lines.empty();
}//end deserialize