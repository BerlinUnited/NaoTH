
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

      Whistle(const std::string& name, const long long& positionInCapture, const double& responseValue)
      :
        name(name),
        positionInCapture(positionInCapture),
        responseValue(responseValue)
      {}

      std::string name;
      long long positionInCapture;
      double responseValue;
    };

    // NOTE: this was used by template based whistle detector
    std::vector<Whistle> recognizedWhistles;
    std::string captureFile;

    // NOTE: obsolete, this is here for documentation purposes
    int counter;

    bool whistleDetected;

    WhistlePercept();
    virtual ~WhistlePercept();
    virtual void print(std::ostream& stream) const;

    void reset()
    {
      recognizedWhistles.clear();
      counter = 0; // NOTE: obsolete?
      whistleDetected = false;
    }

    void addWhistle(const std::string& name, const long long& positionInCapture, const double& responseValue)
    {
      recognizedWhistles.emplace_back(name, positionInCapture, responseValue);
    }
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

