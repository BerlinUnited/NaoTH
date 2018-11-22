/**
* @file MotionStatus.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* 
*/

#include "Representations/Motion/Walk2018/StepBuffer.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

std::ostream& operator<<(std::ostream& os, const Step& s){
  InverseKinematic::FeetPose begin = s.footStep.begin();
  InverseKinematic::FeetPose end = s.footStep.end();

  Pose3D end_relative_to_begin;
  Pose3D begin_foot,end_foot;
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

//using namespace naoth;

//void Serializer<StepBuffer>::StepBuffer(const StepBuffer& /*representation*/, std::ostream& /*stream*/)
//{
//  naothmessages::MotionStatus message;
  
//  message.set_time(representation.time);
//  message.set_lastmotion(representation.lastMotion);
//  message.set_currentmotion(representation.currentMotion);
//  message.set_headmotion(representation.headMotion);
//  message.set_currentmotionstate(representation.currentMotionState);
//  DataConversion::toMessage(representation.plannedMotion.lFoot, *message.mutable_plannedmotionleftfoot());
//  DataConversion::toMessage(representation.plannedMotion.rFoot, *message.mutable_plannedmotionrightfoot());
//  DataConversion::toMessage(representation.plannedMotion.hip, *message.mutable_plannedmotionhip());

//  // step control
//  naothmessages::StepControlStatus* stepControlStatus =  message.mutable_stepcontrolstatus();
//  stepControlStatus->set_stepid(representation.stepControl.stepID);
//  stepControlStatus->set_steprequestid(representation.stepControl.stepRequestID);
//  stepControlStatus->set_moveablefoot(representation.stepControl.moveableFoot);

//  google::protobuf::io::OstreamOutputStream buf(&stream);
//  message.SerializeToZeroCopyStream(&buf);
//}

//void Serializer<StepBuffer>::StepBuffer(std::istream& /*stream*/, StepBuffer& /*representation*/)
//{
//  naothmessages::MotionStatus message;
//  google::protobuf::io::IstreamInputStream buf(&stream);
//  if(message.ParseFromZeroCopyStream(&buf))
//  {
//    representation.time = message.time();
//    representation.lastMotion = static_cast<motion::MotionID>(message.lastmotion());
//    representation.currentMotion = static_cast<motion::MotionID>(message.currentmotion());
//    representation.headMotion = static_cast<HeadMotionRequest::HeadMotionID>(message.headmotion());
//    representation.currentMotionState = static_cast<motion::State>(message.currentmotionstate());
//    DataConversion::fromMessage(message.plannedmotionleftfoot(), representation.plannedMotion.lFoot);
//    DataConversion::fromMessage(message.plannedmotionrightfoot(), representation.plannedMotion.rFoot);
//    DataConversion::fromMessage(message.plannedmotionhip(), representation.plannedMotion.hip);

//    // step control
//    const naothmessages::StepControlStatus& stepControlStatus =  message.stepcontrolstatus();
//    representation.stepControl.stepID = stepControlStatus.stepid();
//    representation.stepControl.stepRequestID = stepControlStatus.steprequestid();
//    representation.stepControl.moveableFoot = static_cast<MotionStatus::StepControlStatus::MoveableFoot>(stepControlStatus.moveablefoot());
//  }
//  else
//  {
//    THROW("Serializer<StepBuffer>::deserialize failed");
//  }
//}
