#ifndef _TeamMessageDebug_H_
#define _TeamMessageDebug_H_

#include <string>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{

// debug message to a host
class TeamMessageDebug: public Printable
{
public:
  std::string data;

  virtual void print(std::ostream& stream) const
  {
    stream << "size = " << data.size() << std::endl;
  }
};

}

#endif // _TeamMessageDebug_H_
