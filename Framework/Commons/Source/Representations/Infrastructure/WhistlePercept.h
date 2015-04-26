
#ifndef _WHISTLE_PERCEPT_H
#define _WHISTLE_PERCEPT_H

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class WhistlePercept: public Printable
  {
  public:
    int counter;

    WhistlePercept();
    virtual void print(std::ostream& stream) const;

    virtual ~WhistlePercept();
  };
}

#endif  /* _WHISTLE_PERCEPT_H */

