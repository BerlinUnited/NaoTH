/*
 * @file RobotMessage.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef ROBOT_MESSAGE_DATA_H
#define ROBOT_MESSAGE_DATA_H

#include <string>
#include <vector>
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"

namespace naoth
{

// message to other robots
class TeamMessageDataOut: public PlatformInterchangeable, public Printable
{
public:
  std::string data;

  virtual void print(std::ostream& stream) const
  {
    stream<<"size = "<<data.size()<<"\n";
  }
};

// message from other robots
class TeamMessageDataIn: public PlatformInterchangeable, public Printable
{
public:
  std::vector<std::string> data;

  virtual void print(std::ostream& stream) const
  {
    stream<<"size = "<<data.size()<<"\n";
  }
};

} // namespace naoth

#endif // ROBOT_MESSAGE_DATA_H
