/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef __TARGET_COM_FEET_POSE_h_
#define __TARGET_COM_FEET_POSE_h_

#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Motion/Engine/InverseKinematicsMotion/Motions/IKPose.h"

class TargetCoMFeetPose : public naoth::Printable
{
public:
    TargetCoMFeetPose(){}

    InverseKinematic::CoMFeetPose pose;

    virtual void print(std::ostream& /*stream*/) const
    {
    }
};

//namespace naoth
//{
//  template<>
//  class Serializer<CoMErrors>
//  {
//  public:
//    static void serialize(const CoMErrors& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, CoMErrors& representation);
//  };
//}

#endif // __TARGET_COM_FEET_POSE_h_
