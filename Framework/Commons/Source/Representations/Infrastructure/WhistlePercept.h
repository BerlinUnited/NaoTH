
#ifndef _WHISTLE_PERCEPT_H
#define _WHISTLE_PERCEPT_H

#include <string>
#include <vector>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class WhistlePercept: public Printable
  {
  public:
    class Whistle
    {
      std::string name;
      size_t positionInCaputre;
      double responseValue;
    };

    std::vector<Whistle> recognizedWhistles;
    int counter;

    WhistlePercept();
    virtual void print(std::ostream& stream) const;

    virtual ~WhistlePercept();
  };
}

#endif  /* _WHISTLE_PERCEPT_H */

