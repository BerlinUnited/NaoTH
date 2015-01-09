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
    Message() : id(-1) {}

    int id;
    std::string command;
    std::map<std::string, std::string> arguments;
  };

  std::list<Message> messages;
};

class DebugMessageInCognition : public DebugMessageIn {};
class DebugMessageInMotion : public DebugMessageIn {};

class DebugMessageOut
{
public:
  class Message
  {
  public:
    Message(int id, long length) : id(id), data(length) {}

    int id;
    std::vector<char> data;
  };

  std::list<Message*> answers;

  void addResponse(int id, std::stringstream& str) {
    long length = (long)str.tellp(); length = length < 0 ? 0 : length;
  
    // NOTE: the objects are deleted later by the DebugServer
    Message* msg = new Message(id, length);

    if(length > 0) {
      str.read(msg->data.data(), msg->data.size());
    }

    answers.push_back(msg);
  }

  void reset() {
    answers.clear();
  }
};

} // namespace naoth

#endif // _DebugMessage_h_
