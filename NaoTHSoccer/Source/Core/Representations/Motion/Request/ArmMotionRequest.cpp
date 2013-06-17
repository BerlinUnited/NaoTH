/**
* @file ArmMotionRequest.cpp
*
* @author <a href="mailto:guido.schillaci@informatik.hu-berlin.de">Guido Schillaci</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Definition of class ArmMotionRequest
*/


#include "ArmMotionRequest.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

std::string ArmMotionRequest::getName(ArmMotionID id)
{
  switch (id)
  {
    case hold: return "hold";
    case set_left_shoulder_position: return "set_left_shoulder_position";
    case set_left_shoulder_stiffness: return "set_left_shoulder_stiffness";
    case set_left_elbow_position: return "set_left_elbow_position";
    case set_left_elbow_stiffness: return "set_left_elbow_stiffness";
    case set_right_shoulder_position: return "set_right_shoulder_position";
    case set_right_shoulder_stiffness: return "set_right_shoulder_stiffness";
    case set_right_elbow_position: return "set_right_elbow_position";
    case set_right_elbow_stiffness: return "set_right_elbow_stiffness";
    case set_left_arm_joint_position: return "set_left_arm_joint_position";
    case set_left_arm_joint_stiffness: return "set_left_arm_joint_stiffness";
    case set_right_arm_joint_position: return "set_right_arm_joint_position";
    case set_right_arm_joint_stiffness: return "set_right_arm_joint_stiffness";
    case set_both_arms_joint_position: return "set_both_arms_joint_position";
    case set_both_arms_joint_stiffness: return "set_both_arms_joint_stiffness";

    case arms_none: return "arms_none";
    case arms_back: return "arms_back";
    case arms_down: return "arms_down";

    default: return "unknown";
  }///end switch
}///end getName

void ArmMotionRequest::print(std::ostream &stream) const
{
    stream << "Current ArmMotionRequest = " << getName(id) << std::endl;
    stream << "Left elbow yaw " << lElbowPosition.x << " roll "<< lElbowPosition.y<<std::endl;
    stream << "Left shoulder pitch " << lShoulderPosition.x << " roll "<< lShoulderPosition.y<<std::endl;
    stream << "Right elbow yaw " << rElbowPosition.x << " roll "<< rElbowPosition.y<<std::endl;
    stream << "Right shoulder pitch " << rShoulderPosition.x << " roll "<< rShoulderPosition.y<<std::endl;
}

ArmMotionRequest::ArmMotionID ArmMotionRequest::getId(const std::string& name)
{
  for(int i = 0; i < numOfArmMotion; i++)
  {
    if(name == getName((ArmMotionID)i)) return (ArmMotionID)i;
  }//end for
  
  return numOfArmMotion;
}//end getId


void Serializer<ArmMotionRequest>::serialize(const ArmMotionRequest& representation, std::ostream& stream)
{
  naothmessages::ArmMotionRequest message;
  message.set_id(representation.id);

  DataConversion::toMessage(representation.lShoulderPosition, *(message.mutable_lshoulderposition()));
  DataConversion::toMessage(representation.lShoulderStiffness, *(message.mutable_lshoulderstiffness()));
  DataConversion::toMessage(representation.lElbowPosition, *(message.mutable_lelbowposition()));
  DataConversion::toMessage(representation.lElbowStiffness, *(message.mutable_lelbowstiffness()));

  DataConversion::toMessage(representation.rShoulderPosition, *(message.mutable_rshoulderposition()));
  DataConversion::toMessage(representation.rShoulderStiffness, *(message.mutable_rshoulderstiffness()));
  DataConversion::toMessage(representation.rElbowPosition, *(message.mutable_relbowposition()));
  DataConversion::toMessage(representation.rElbowStiffness, *(message.mutable_relbowstiffness()));

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializePartialToZeroCopyStream(&buf);
}//end serialize

void Serializer<ArmMotionRequest>::serialize(const ArmMotionRequest& representation, naothmessages::ArmMotionRequest* msg)
{

    msg->set_id(representation.id);

    DataConversion::toMessage(representation.lShoulderPosition, *(msg->mutable_lshoulderposition()));
    DataConversion::toMessage(representation.lShoulderStiffness, *(msg->mutable_lshoulderstiffness()));
    DataConversion::toMessage(representation.lElbowPosition, *(msg->mutable_lelbowposition()));
    DataConversion::toMessage(representation.lElbowStiffness, *(msg->mutable_lelbowstiffness()));

    DataConversion::toMessage(representation.rShoulderPosition, *(msg->mutable_rshoulderposition()));
    DataConversion::toMessage(representation.rShoulderStiffness, *(msg->mutable_rshoulderstiffness()));
    DataConversion::toMessage(representation.rElbowPosition, *(msg->mutable_relbowposition()));
    DataConversion::toMessage(representation.rElbowStiffness, *(msg->mutable_relbowstiffness()));


}//end serialize


void Serializer<ArmMotionRequest>::deserialize(std::istream& stream, ArmMotionRequest& representation)
{
  naothmessages::ArmMotionRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.id = static_cast<ArmMotionRequest::ArmMotionID>(message.id());

  DataConversion::fromMessage(message.lshoulderposition(), representation.lShoulderPosition);
  DataConversion::fromMessage(message.lshoulderstiffness(), representation.lShoulderStiffness);
  DataConversion::fromMessage(message.lelbowposition(), representation.lElbowPosition);
  DataConversion::fromMessage(message.lelbowstiffness(), representation.lElbowStiffness);

  DataConversion::fromMessage(message.rshoulderposition(), representation.rShoulderPosition);
  DataConversion::fromMessage(message.rshoulderstiffness(), representation.rShoulderStiffness);
  DataConversion::fromMessage(message.relbowposition(), representation.rElbowPosition);
  DataConversion::fromMessage(message.relbowstiffness(), representation.rElbowStiffness);

}//end deserialize


void Serializer<ArmMotionRequest>::deserialize(const naothmessages::ArmMotionRequest* msg, ArmMotionRequest& representation)
{

  representation.id = static_cast<ArmMotionRequest::ArmMotionID>(msg->id());

  DataConversion::fromMessage(msg->lshoulderposition(), representation.lShoulderPosition);
  DataConversion::fromMessage(msg->lshoulderstiffness(), representation.lShoulderStiffness);
  DataConversion::fromMessage(msg->lelbowposition(), representation.lElbowPosition);
  DataConversion::fromMessage(msg->lelbowstiffness(), representation.lElbowStiffness);

  DataConversion::fromMessage(msg->rshoulderposition(), representation.rShoulderPosition);
  DataConversion::fromMessage(msg->rshoulderstiffness(), representation.rShoulderStiffness);
  DataConversion::fromMessage(msg->relbowposition(), representation.rElbowPosition);
  DataConversion::fromMessage(msg->relbowstiffness(), representation.rElbowStiffness);

}//end deserialize

