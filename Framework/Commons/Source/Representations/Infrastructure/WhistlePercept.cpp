#include "WhistlePercept.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

WhistlePercept::WhistlePercept():
  counter(0)
{
}

void WhistlePercept::print(std::ostream& stream) const
{
  stream << "counter: " << counter << std::endl;
  stream << "capture to file: " << captureFile << std::endl;
  stream << "recognized whistles: " << recognizedWhistles.size() << std::endl;
  for (size_t wIdx = 0; wIdx < recognizedWhistles.size(); wIdx++)
  {
    stream << " " << recognizedWhistles[wIdx].name << " @" << recognizedWhistles[wIdx].positionInCapture << " response=" << recognizedWhistles[wIdx] .responseValue << std::endl; 
  }

}

WhistlePercept::~WhistlePercept()
{ 
}

void Serializer<WhistlePercept>::deserialize(std::istream& stream, WhistlePercept& representation)
{
  naothmessages::WhistlePerceptData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.counter = message.counter();
  representation.captureFile = message.capturefile();

  representation.recognizedWhistles.clear();
  for (int i = 0; i < message.recognizedwhistles_size(); i++)
  {
    const naothmessages::WhistleData& msgWhistle = message.recognizedwhistles(i);
    WhistlePercept::Whistle whistle(msgWhistle.name(), msgWhistle.positionincapture(), msgWhistle.responsevalue());
    representation.recognizedWhistles.push_back(whistle);
  }
}//end deserialize


void Serializer<WhistlePercept>::serialize(const WhistlePercept& representation, std::ostream& stream)
{
  //if (representation.lastCounterAtSerialization < representation.counter)
  {
    naothmessages::WhistlePerceptData message;

    message.set_counter(representation.counter);
    message.set_capturefile(representation.captureFile);

    for (size_t i = 0; i < representation.recognizedWhistles.size(); i++)
    {
      naothmessages::WhistleData* msgWhistle = message.add_recognizedwhistles();
      msgWhistle->set_name(representation.recognizedWhistles[i].name);
      msgWhistle->set_positionincapture(representation.recognizedWhistles[i].positionInCapture);
      msgWhistle->set_responsevalue(representation.recognizedWhistles[i].responseValue);
    }

    google::protobuf::io::OstreamOutputStream buf(&stream);
    message.SerializeToZeroCopyStream(&buf);
  }
  //representation.lastCounterAtSerialization = representation.counter;
}//end serialize

