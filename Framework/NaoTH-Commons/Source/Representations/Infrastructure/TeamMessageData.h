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
#include "Tools/DataStructures/Printable.h"
#include "Tools/Communication/RCTC/TeamMessage.h"

namespace naoth
{

class RCTCTeamMessageDataOut: public Printable
{
public:
  RCTCTeamMessageDataOut(): valid(false) {}
  rctc::Message data;
  bool valid;

  virtual void print(std::ostream& stream) const
  {
    stream << data;
  }
};

class RCTCTeamMessageDataIn: public Printable
{
public:
  std::vector<rctc::Message> data;

  virtual void print(std::ostream& stream) const
  {
    stream<<"size = "<<data.size()<<"\n";
    for(unsigned int i=0; i < data.size(); i++)
    {
      stream << "=================" << std::endl;
      stream << data[i] << std::endl;
    }
  }
};

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

} // namespace naoth

#endif // ROBOT_MESSAGE_DATA_H
