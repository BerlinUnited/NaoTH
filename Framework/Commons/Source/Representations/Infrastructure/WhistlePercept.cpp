#include "WhistlePercept.h"

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

