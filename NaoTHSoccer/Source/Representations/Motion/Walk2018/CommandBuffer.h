/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef __CommandBuffer_h_
#define __CommandBuffer_h_

#include "Motion/Engine/InverseKinematicsMotion/Motions/IKPose.h"
#include "Motion/Engine/InverseKinematicsMotion/Motions/Walk2018/FootStep.h"

#include "Tools/DataStructures/Printable.h"
#include "Representations/Motion/Request/WalkRequest.h"

#include "Tools/DataStructures/RingBufferWithSum.h"

#include <string>

class CommandBuffer : public naoth::Printable
{
  public:
    CommandBuffer(){}

    RingBuffer<InverseKinematic::CoMFeetPose, 10> poses;
    RingBuffer<FootStep::Foot, 10> footIds;

    void reset(){
        poses.clear();
        footIds.clear();
    }

    virtual void print(std::ostream& /*stream*/) const
    {
    }
};

//namespace naoth
//{
//  template<>
//  class Serializer<CommandBuffer>
//  {
//  public:
//    static void serialize(const CommandBuffer& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, CommandBuffer& representation);
//  };
//}

#endif // __CommandBuffer_h_
