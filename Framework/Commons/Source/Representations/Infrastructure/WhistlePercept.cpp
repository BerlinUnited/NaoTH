#include "WhistlePercept.h"

using namespace naoth;

WhistlePercept::WhistlePercept():
  counter(0)
{
}

void WhistlePercept::print(std::ostream& stream) const
{
  stream << "counter: " << counter << std::endl;
}

WhistlePercept::~WhistlePercept()
{ 
}

