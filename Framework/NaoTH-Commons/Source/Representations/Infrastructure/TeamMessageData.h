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
#include "Tools/Communication/RCTC/TeamMessage.h"

namespace naoth
{

class RCTCTeamMessageDataOut: public Printable
{
public:
  RCTCTeamMessageDataOut(): valid(false) {}
  rctc::Message data;
  bool valid;

  virtual void print(ostream& stream) const
  {
    stream << data;
  }
};

class RCTCTeamMessageDataIn: public Printable
{
public:
  std::vector<rctc::Message> data;

  virtual void print(ostream& stream) const
  {
    stream<<"size = "<<data.size()<<"\n";
  }
};

// message to other robots
class TeamMessageDataOut: public PlatformInterchangeable, public Printable
{
public:
  std::string data;

  virtual void print(ostream& stream) const
  {
    stream<<"size = "<<data.size()<<"\n";
  }
};

// message from other robots
class TeamMessageDataIn: public PlatformInterchangeable, public Printable
{
public:
  std::vector<std::string> data;

  virtual void print(ostream& stream) const
  {
    stream<<"size = "<<data.size()<<"\n";
  }
};

} // namespace naoth

#endif // ROBOT_MESSAGE_DATA_H
