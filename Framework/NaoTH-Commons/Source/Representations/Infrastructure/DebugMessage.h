/*
 * @file DebugMessage.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef __DebugMessage_h__
#define __DebugMessage_h__

#include <sstream>
#include <map>
#include <vector>

#include "PlatformInterface/PlatformInterchangeable.h"

namespace naoth
{

class DebugMessageIn: public PlatformInterchangeable
{
public:
  class Message
  {
  public:
    std::string command;
    std::map<std::string, std::string> arguments;
  };

  std::vector<Message> messages;
};

class DebugMessageOut: public PlatformInterchangeable
{
public:
  std::stringstream answer;
};

} // namespace naoth

#endif // __DebugMessage_h__
