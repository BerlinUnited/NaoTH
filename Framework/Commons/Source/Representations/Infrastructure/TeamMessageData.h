/*
 * @file RobotMessage.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef _TeamMessageData_H_
#define _TeamMessageData_H_

#include <string>
#include <vector>
#include "Tools/DataStructures/Printable.h"

namespace naoth
{

// message to other robots
class TeamMessageDataOut: public Printable
{
public:
  std::string data;

  virtual void print(std::ostream& stream) const
  {
    stream << "size = " << data.size() << std::endl;
  }
};

// message from other robots
class TeamMessageDataIn: public Printable
{
public:
  std::vector<std::string> data;

  virtual void print(std::ostream& stream) const
  {
    stream << "size = " << data.size() << std::endl;
  }
};

}

#endif // _TeamMessageData_H_
