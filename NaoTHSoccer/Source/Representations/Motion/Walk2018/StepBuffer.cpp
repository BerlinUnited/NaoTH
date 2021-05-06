/**
* @file StepBuffer.cpp
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

std::ostream& operator<<(std::ostream& os, const Step& s)
{
  InverseKinematic::FeetPose begin = s.footStep.begin();
  InverseKinematic::FeetPose end = s.footStep.end();

  Pose3D end_relative_to_begin;
  Pose3D begin_foot;
  Pose3D end_foot;
  if(s.footStep.liftingFoot() == FootStep::LEFT) {
      begin.localInLeftFoot();
      end.localInRightFoot();
      end_relative_to_begin = begin.right * end.left;
      begin.localInRightFoot();
      begin_foot = begin.left;
      end_foot   = end.left;
  } else {
      begin.localInRightFoot();
      end.localInLeftFoot();
      end_relative_to_begin = begin.left * end.right;
      begin.localInLeftFoot();
      begin_foot = begin.right;
      end_foot   = end.right;
  }

  os << "begin" << std::endl;
  os << begin_foot/*s.footStep.footBegin()*/ << std::endl;
  os << "end" << std::endl;
  os << end_foot/*s.footStep.footEnd()*/   << std::endl;
  os << "support" << std::endl;
  os << s.footStep.supFoot()   << std::endl;
  os << "end relative to begin" << std::endl;
  os << end_relative_to_begin   << std::endl;
  os << ((s.type == Step::STEP_WALK) ? "step_walk" : "step_control") << std::endl;
  return os;
}

using namespace naoth;
#include <Tools/DataConversion.h>

void Serializer<StepBuffer>::serialize(const StepBuffer& representation, std::ostream& stream)
{
    static unsigned int id = 0;
    naothmessages::StepBuffer sb;

    if(representation.first().id() != id){
        id = representation.first().id();
        DataConversion::toMessage(representation.first().footStep.supFoot(), *sb.mutable_support_foot());
    }

    google::protobuf::io::OstreamOutputStream buf(&stream);
    sb.SerializeToZeroCopyStream(&buf);
}

void Serializer<StepBuffer>::deserialize(std::istream& stream, StepBuffer& representation)
{
  naothmessages::StepBuffer message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(message.ParseFromZeroCopyStream(&buf))
  {
      if(message.has_support_foot()){
         Step s = representation.add();
         DataConversion::fromMessage(message.support_foot(), s.footStep.supFoot());
      }
  }
  else
  {
    THROW("Serializer<StepBuffer>::deserialize failed");
  }
}
