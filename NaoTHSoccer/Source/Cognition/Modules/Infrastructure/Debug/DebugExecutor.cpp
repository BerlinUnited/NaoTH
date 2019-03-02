/*
 * File:   DebugExecutor.cpp
 * Author: Heinrich Mellmann
 *
 */

#include "DebugExecutor.h"
#include <PlatformInterface/PlatformInterface.h>


DebugExecutor::DebugExecutor()
{
}

void DebugExecutor::execute()
{
  getDebugMessageOut().reset();

  for(const DebugMessageIn::Message& msg: getDebugMessageInCognition().messages)
  {
    answer_stream.clear();
    answer_stream.str("");

    // execure basic commands
    // NOTE: this is the first step of removing command callbacks
    if(msg.command == "representation:get") {
      for (const auto& p: msg.arguments) {
        getRepresentations().serializeRepresentation(answer_stream, p.first);
      }
    } else if(msg.command == "representation:set") {
      for (const auto& p: msg.arguments) {
        getRepresentations().setRepresentation(answer_stream, p.first, p.second);
      }
    } else if(msg.command == "representation:print") {
      for (const auto& p: msg.arguments) {
        getRepresentations().printRepresentation(answer_stream, p.first);
      }
    } else {
      getDebugCommandManager().handleCommand(msg.command, msg.arguments, answer_stream);
    }

    getDebugMessageOut().addResponse(msg.id, answer_stream);
  }
}//end execute




