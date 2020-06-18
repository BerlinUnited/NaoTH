#include "Representations/Modeling/RobotPose.h"
#include <Messages/RobotPose.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void RobotPose::print(std::ostream& stream) const
{
  stream << "x = " << translation.x << std::endl;
  stream << "y = " << translation.y << std::endl;
  stream << "rotation = " << rotation << std::endl;
  stream << "valid = " << isValid << std::endl;
}

void RobotPose::draw(DrawingCanvas2D& canvas, bool drawDeviation) const
{
  //FIELD_DRAWING_CONTEXT;
  if (isValid)
  {
    /*
    switch( getPlayerInfo().gameData.teamColor )
    {
    case GameData::red:    canvas.pan("FF0000", 20); break;
    case GameData::blue:   canvas.pan("0000FF", 20); break;
    case GameData::yellow: canvas.pan("FFFF00", 20); break;
    case GameData::black:  canvas.pan("000000", 20); break;
    default: canvas.pan("AAAAAA", 20); break;
    }*/
    canvas.pen("000000", 20);
  }
  else
  {
    canvas.pen("AAAAAA", 20);
  }

  canvas.drawRobot(translation.x, translation.y, rotation);


  if (drawDeviation)
  {
    canvas.pen("000000", 20);

    canvas.drawLine(translation.x - principleAxisMajor.x,
      translation.y - principleAxisMajor.y,
      translation.x + principleAxisMajor.x,
      translation.y + principleAxisMajor.y);

    canvas.drawLine(translation.x - principleAxisMinor.x,
      translation.y - principleAxisMinor.y,
      translation.x + principleAxisMinor.x,
      translation.y + principleAxisMinor.y);

    canvas.drawOvalRotated(translation.x,
      translation.y,
      principleAxisMinor.abs() * 2.0,
      principleAxisMajor.abs() * 2.0,
      principleAxisMinor.angle());
  }
}

void Serializer<RobotPose>::serialize(const RobotPose& representation, std::ostream& stream)
{
  naothmessages::RobotPose message;

  message.mutable_pose()->set_rotation(representation.rotation);
  message.mutable_pose()->mutable_translation()->set_x(representation.translation.x);
  message.mutable_pose()->mutable_translation()->set_y(representation.translation.y);
  message.set_isvalid(representation.isValid);
  message.mutable_principleaxismajor()->set_x(representation.principleAxisMajor.x);
  message.mutable_principleaxismajor()->set_y(representation.principleAxisMajor.y);
  message.mutable_principleaxisminor()->set_x(representation.principleAxisMinor.x);
  message.mutable_principleaxisminor()->set_y(representation.principleAxisMinor.y);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializePartialToZeroCopyStream(&buf);
}

void Serializer<RobotPose>::deserialize(std::istream& stream, RobotPose& representation)
{
  naothmessages::RobotPose message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.rotation = message.pose().rotation();
  representation.translation.x = message.pose().translation().x();
  representation.translation.y = message.pose().translation().y();

  
  if (message.has_principleaxismajor()){
    representation.principleAxisMajor.x = message.principleaxismajor().x();
    representation.principleAxisMajor.x = message.principleaxismajor().y();
  }
  if (message.has_principleaxisminor()) {
    representation.principleAxisMinor.x = message.principleaxisminor().x();
    representation.principleAxisMinor.x = message.principleaxisminor().y();
  }
  
  if (message.has_isvalid()) {
    representation.isValid = message.isvalid();
  }
  
}

