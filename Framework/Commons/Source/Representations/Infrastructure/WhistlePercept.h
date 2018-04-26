
#ifndef _WHISTLE_PERCEPT_H
#define _WHISTLE_PERCEPT_H

#include <string>
#include <vector>
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{
  class WhistlePercept: public Printable
  {
  public:
    class Whistle
    {
      public:

      Whistle(const std::string& name_, const long long& positionInCapture_, const double& responseValue_)
      :
        name(name_),
        positionInCapture(positionInCapture_),
        responseValue(responseValue_)
      {}

      std::string name;
      long long positionInCapture;
      double responseValue;
    };

    std::vector<Whistle> recognizedWhistles;
    std::string captureFile;
    int counter;
    int lastCounterAtSerialization;

    WhistlePercept();
    virtual void print(std::ostream& stream) const;

    virtual ~WhistlePercept();
  };

  template<>
  class Serializer<WhistlePercept>
  {
  public:
    static void serialize(const WhistlePercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, WhistlePercept& representation);
  };
}

#endif  /* _WHISTLE_PERCEPT_H */

