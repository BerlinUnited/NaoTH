/*
 * @file DebugMessage.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef _DebugMessage_h_
#define _DebugMessage_h_

#include <sstream>
#include <map>
#include <list>

namespace naoth
{

class DebugMessageIn
{
public:
  class Message
  {
  public:
    std::string command;
    std::map<std::string, std::string> arguments;
  };

  std::list<Message> messages;
};

class DebugMessageOut
{
public:
  std::list<std::string> answers;
};

} // namespace naoth

#endif // _DebugMessage_h_
