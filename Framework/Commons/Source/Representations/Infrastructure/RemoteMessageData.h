/*
 * @file RobotMessage.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef _RemoteMessageData_H_
#define _RemoteMessageData_H_

#include <string>
#include <vector>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{

// incomming messages
class RemoteMessageDataIn: public Printable
{
public:
  std::vector<std::string> data;

  virtual void print(std::ostream& stream) const
  {
    stream << "size = " << data.size() << std::endl;
  }
};

}

#endif // _RemoteMessageData_H_
