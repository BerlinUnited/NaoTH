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
    Message(int id, size_t length) : id(id), data(length) {}

    int id;
    std::vector<char> data;
  };

  std::list<Message*> answers;

  void addResponse(int id, std::stringstream& str) {
    // get the length of the message in the stream
    size_t length = static_cast<size_t>(std::max(static_cast<int>(str.tellp()), 0));

    // NOTE: the objects are deleted later by the DebugServer
    Message* msg = new Message(id, length);

    if(length > 0) {
      str.read(msg->data.data(), static_cast<int>(msg->data.size()));
    }

    answers.push_back(msg);
  }

  void reset() {
    answers.clear();
  }
};

} // namespace naoth

#endif // _DebugMessage_h_
