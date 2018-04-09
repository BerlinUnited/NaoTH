/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef __TARGET_HIP_FEET_POSE_h_
#define __TARGET_HIP_FEET_POSE_h_

#include "Tools/DataStructures/Printable.h"
#include "Motion/Engine/InverseKinematicsMotion/Motions/IKPose.h"

class TargetHipFeetPose : public naoth::Printable
{
public:
    TargetHipFeetPose(){}

    InverseKinematic::HipFeetPose pose;

    virtual void print(std::ostream& /*stream*/) const
    {
    }
};

//namespace naoth
//{
//  template<>
//  class Serializer<TargetHipFeetPose>
//  {
//  public:
//    static void serialize(const TargetHipFeetPose& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, TargetHipFeetPose& representation);
//  };
//}

#endif // __TARGET_HIP_FEET_POSE_h_
