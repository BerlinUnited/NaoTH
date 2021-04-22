#include "WhistlePercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

WhistlePercept::WhistlePercept():
  whistleDetected(false)
{
}

void WhistlePercept::print(std::ostream& stream) const
{ 
  //stream << "whistleDetected: " << whistleDetected << std::endl;
  stream << "Frame number of last detected whistle: " << frameWhenWhistleDetected << std::endl;
  /*
  stream << "capture to file: " << captureFile << std::endl;
  stream << "recognized whistles: " << recognizedWhistles.size() << std::endl;
  for (size_t wIdx = 0; wIdx < recognizedWhistles.size(); wIdx++) 
  {
    stream << " " << recognizedWhistles[wIdx].name << " @" << recognizedWhistles[wIdx].positionInCapture << " response=" << recognizedWhistles[wIdx] .responseValue << std::endl; 
  }
  */
}

WhistlePercept::~WhistlePercept()
{ 
}

void Serializer<WhistlePercept>::deserialize(std::istream& stream, WhistlePercept& representation)
{
  naothmessages::WhistlePercept message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  // NOTE: obsolete
  if(message.has_framewhenwhistledetected()) {
    representation.frameWhenWhistleDetected = message.framewhenwhistledetected();
  }

  if(message.has_capturefile()) {
    representation.captureFile = message.capturefile();
  }

  if(message.has_whistledetected()) {
    representation.whistleDetected = message.whistledetected();
  }

  representation.recognizedWhistles.clear();
  for (int i = 0; i < message.recognizedwhistles_size(); i++)
  {
    const naothmessages::WhistlePercept::Whistle& msgWhistle = message.recognizedwhistles(i);
    WhistlePercept::Whistle whistle(msgWhistle.name(), msgWhistle.positionincapture(), msgWhistle.responsevalue());
    representation.recognizedWhistles.push_back(whistle);
  }
}//end deserialize


void Serializer<WhistlePercept>::serialize(const WhistlePercept& representation, std::ostream& stream)
{
  naothmessages::WhistlePercept message;

  // NOTE: obsolete
  message.set_capturefile(representation.captureFile);
  message.set_whistledetected(representation.whistleDetected);

  for (size_t i = 0; i < representation.recognizedWhistles.size(); i++)
  {
    naothmessages::WhistlePercept::Whistle* msgWhistle = message.add_recognizedwhistles();
    msgWhistle->set_name(representation.recognizedWhistles[i].name);
    msgWhistle->set_positionincapture(representation.recognizedWhistles[i].positionInCapture);
    msgWhistle->set_responsevalue(representation.recognizedWhistles[i].responseValue);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}//end serialize

