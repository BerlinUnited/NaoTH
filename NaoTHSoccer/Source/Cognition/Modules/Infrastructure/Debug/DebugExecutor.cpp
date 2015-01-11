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

  for(std::list<DebugMessageIn::Message>::const_iterator iter = getDebugMessageInCognition().messages.begin();
      iter != getDebugMessageInCognition().messages.end(); ++iter)
  {
    answer_stream.clear();
    answer_stream.str("");

    getDebugCommandManager().handleCommand(iter->command, iter->arguments, answer_stream);
    getDebugMessageOut().addResponse(iter->id, answer_stream);
  }
}//end execute




