#include "MultiBallPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void MultiBallPercept::print(std::ostream &stream) const {
    stream << "wasSeen = " << wasSeen() << std::endl;
    stream << "frameInfoWhenBallWasSeen:" << frameInfoWhenBallWasSeen << std::endl;
    stream << "----------------------" << std::endl;
    for(size_t i = 0; i < percepts.size(); i++) {
        stream << "BallPercept Number " << i+1 << ":" << std::endl;
        percepts[i].print(stream);
        stream << "----------------------" << std::endl;
    }
}

using namespace naoth;

void Serializer<MultiBallPercept>::serialize(const MultiBallPercept& representation, std::ostream& stream)
{
  naothmessages::MultiBallPercept p;

  p.mutable_frameinfowhenballwasseen()->set_framenumber(representation.frameInfoWhenBallWasSeen.getFrameNumber());
  p.mutable_frameinfowhenballwasseen()->set_time(representation.frameInfoWhenBallWasSeen.getTime());

  for(MultiBallPercept::ConstABPIterator iter = representation.begin(); iter != representation.end(); iter++) {
    naothmessages::MultiBallPercept_BallPercept* bp = p.add_percepts();

    bp->mutable_centerinimage()->set_x((*iter).centerInImage.x);
    bp->mutable_centerinimage()->set_y((*iter).centerInImage.y);

    bp->set_radiusinimage((*iter).radiusInImage);

    bp->mutable_positiononfield()->set_x((*iter).positionOnField.x);
    bp->mutable_positiononfield()->set_y((*iter).positionOnField.y);

    bp->set_cameraid((naothmessages::CameraID)iter->cameraId);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<MultiBallPercept>::deserialize(std::istream& stream, MultiBallPercept& representation)
{
  naothmessages::MultiBallPercept p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  representation.reset();

  if(p.has_frameinfowhenballwasseen()) {
      representation.frameInfoWhenBallWasSeen.setFrameNumber(p.frameinfowhenballwasseen().framenumber());
      representation.frameInfoWhenBallWasSeen.setTime(p.frameinfowhenballwasseen().time());
  }

  for(int i = 0; i < p.percepts_size(); i++){
    MultiBallPercept::BallPercept ballPercept;
    const naothmessages::MultiBallPercept::BallPercept& percept = p.percepts(i);

    if(percept.has_centerinimage()){
        ballPercept.centerInImage.x = percept.centerinimage().x();
        ballPercept.centerInImage.y = percept.centerinimage().y();
    }

    if(percept.has_positiononfield()){
        ballPercept.positionOnField.x = percept.positiononfield().x();
        ballPercept.positionOnField.y = percept.positiononfield().y();
    }

    if(percept.has_cameraid()){
        ballPercept.cameraId = (CameraInfo::CameraID) percept.cameraid();
    }

    if(percept.has_radiusinimage()){
        ballPercept.radiusInImage = percept.radiusinimage();
    }

    representation.add(ballPercept);
  }
}//end deserialize
