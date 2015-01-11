/**
 * @file BallPercept.cpp
 * 
 * Definition of class BallPercept
 */ 

#include "BallPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<BallPercept>::serialize(const BallPercept& representation, std::ostream& stream)
{
  naothmessages::BallPercept p;

  p.set_ballwasseen(representation.ballWasSeen);
  
  p.mutable_centerinimage()->set_x(representation.centerInImage.x);
  p.mutable_centerinimage()->set_y(representation.centerInImage.y);

  p.set_radiusinimage(representation.radiusInImage);
  
  p.set_ballcolor((naothmessages::Color) representation.ballColor);

  p.mutable_bearingbasedoffsetonfield()->set_x(representation.bearingBasedOffsetOnField.x);
  p.mutable_bearingbasedoffsetonfield()->set_y(representation.bearingBasedOffsetOnField.y);

  p.mutable_frameinfowhenballwasseen()->set_framenumber(representation.frameInfoWhenBallWasSeen.getFrameNumber());
  p.mutable_frameinfowhenballwasseen()->set_time(representation.frameInfoWhenBallWasSeen.getTime());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<BallPercept>::deserialize(std::istream& stream, BallPercept& representation)
{
  naothmessages::BallPercept p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  if(p.has_ballwasseen())
  {
    representation.ballWasSeen = p.ballwasseen();
  }
  if(p.has_centerinimage())
  {
    representation.centerInImage.x = p.centerinimage().x();
    representation.centerInImage.y = p.centerinimage().y();
  }
  if(p.has_radiusinimage())
  {
    representation.radiusInImage = p.radiusinimage();
  }
  if(p.has_ballcolor())
  {
    representation.ballColor = (ColorClasses::Color) p.ballcolor();
  }
  if(p.has_bearingbasedoffsetonfield())
  {
    representation.bearingBasedOffsetOnField.x = p.bearingbasedoffsetonfield().x();
    representation.bearingBasedOffsetOnField.y = p.bearingbasedoffsetonfield().y();
  }
  if(p.has_frameinfowhenballwasseen())
  {
    representation.frameInfoWhenBallWasSeen = 
      naoth::FrameInfo(p.frameinfowhenballwasseen().time(), p.frameinfowhenballwasseen().framenumber());
  }
}//end deserialize