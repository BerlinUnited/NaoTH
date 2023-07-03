/* 
 * File:   ButtonData.cpp
 * Author: thomas
 * 
 * Created on 6. march 2009, 10:40
 */

#include "ButtonData.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <string>

using namespace naoth;
using namespace std;

ButtonData::ButtonData() 
{
  for(int i=0; i < numOfButtons; i++)
  {
    isPressed[i] = false;
    numOfFramesPressed[i] = 0;
    eventCounter[i] = 0;
  }
}

string ButtonData::getButtonName(ButtonID id)
{
  switch(id)
  {
    case Chest: return "Chest";
    case LeftFootLeft: return "LeftFootLeft";
    case LeftFootRight: return "LeftFootRight";
    case RightFootLeft: return "RightFootLeft";
    case RightFootRight: return "RightFootRight";

    case HeadFront: return "HeadFront";
    case HeadMiddle: return "HeadMiddle";
    case HeadRear: return "HeadRear";

    case LeftHandBack: return "LeftHandBack";
    case LeftHandLeft: return "LeftHandLeft";
    case LeftHandRight: return "LeftHandRight";

    case RightHandBack: return "RightHandBack";
    case RightHandLeft: return "RightHandLeft";
    case RightHandRight: return "RightHandRight";

    default: return "Unknown Button";
  }//end switch
}//end getBumperName

void ButtonData::print(ostream& stream) const
{
  stream << "Button: Pressed, #Pressed, #Events" << std::endl;
  for(int i = 0; i < numOfButtons; i++)
  {
    stream  << getButtonName((ButtonID)i) << ": " 
            << isPressed[i] << ", "
            << numOfFramesPressed[i] << ", "
            << eventCounter[i]
            << std::endl;
  }
}//end print

void Serializer<ButtonData>::serialize(const ButtonData& representation, std::ostream& stream)
{
  naothmessages::ButtonData msg;

  // NOTE: deprecated to support logs before 12.01.2017
  for(size_t i=0; i < ButtonData::numOfButtons; i++) {
    msg.add_ispressed(representation.isPressed[i]);
    msg.add_numofframespressed(static_cast<unsigned int>(representation.numOfFramesPressed[i]));
    msg.add_eventcounter(static_cast<unsigned int>(representation.eventCounter[i]));
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<ButtonData>::deserialize(std::istream& stream, ButtonData& representation)
{
  google::protobuf::io::IstreamInputStream buf(&stream);
  naothmessages::ButtonData msg;
  msg.ParseFromZeroCopyStream(&buf);

  ASSERT(ButtonData::numOfButtons == msg.ispressed_size());
  ASSERT(ButtonData::numOfButtons == msg.numofframespressed_size());
  ASSERT(ButtonData::numOfButtons == msg.eventcounter_size());

  for(int i=0; i < ButtonData::numOfButtons; i++) {
    representation.isPressed[i] = msg.ispressed(i);
    representation.numOfFramesPressed[i] = static_cast<int>(msg.numofframespressed(i));
    representation.eventCounter[i] = static_cast<int>(msg.eventcounter(i));
  }
}



