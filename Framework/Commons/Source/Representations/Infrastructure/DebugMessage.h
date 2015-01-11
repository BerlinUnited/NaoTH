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
#include <vector>

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
  typedef std::vector<char> Data;

  std::list<Data*> answers;

  void addResponse(std::stringstream& str) {
    long length = (long)str.tellp(); length = length < 0 ? 0 : length;
  
    // NOTE: the objects are deleted later by the DebugServer
    Data* buffer = new Data(length);

    if(length > 0) {
      str.read(buffer->data(), buffer->size());
    }

    answers.push_back(buffer);
  }

  void reset() {
    answers.clear();
  }
};

} // namespace naoth

#endif // _DebugMessage_h_
