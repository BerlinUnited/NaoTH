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
  // some infos, mainly for debugging
  FrameInfo lastSend;
  unsigned int interval = 0;
  // the address where the data is send
  std::string host;
  unsigned int port;
  // the acutal data to send
  std::string data;

  virtual void print(std::ostream& stream) const
  {
    stream << "host = " << host << ":" << port << "\n";
    stream << "size = " << data.size() << "\n";
    stream << "lastSend = " << lastSend.getFrameNumber() << " @ " << lastSend.getTime() << "\n";
    stream << "interval = " << interval << std::endl;
  }
};

}

#endif // _TeamMessageDebug_H_
