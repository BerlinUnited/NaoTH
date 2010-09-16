/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include <string>
#include <stdlib.h>

#include <iostream>
#include <string.h>

#include "DebugServer.h"

DebugServer::DebugServer(unsigned int port)
: comm(port)
{
  commands = g_async_queue_new();
  answers = g_async_queue_new();

  if (g_thread_supported())
  {
    GError* err = NULL;
    g_message("Starting debug server as seperate thread");
    dispatcherThread = g_thread_create(dispatcher_static, this, true, &err);
  } else
  {
    g_warning("No threading support: DebugServer not available");
  }
}

void DebugServer::dispatcher()
{
  g_message("Dispatcher init");
  g_async_queue_ref(commands);
  g_async_queue_ref(answers);
  comm.init();

  g_message("Starting DebugServer dispatcher loop");
  while (true)
  {
    char* msg = comm.readMessage();
    if (msg != NULL)
    {
      g_async_queue_push(commands, msg);
    }

    g_thread_yield();

    while (g_async_queue_length(answers) > 0)
    {
      g_debug("there is something in the *answer* queue");
      char* answer = (char*) g_async_queue_pop(answers);

      g_debug("found %s in the *answer* queue", answer);

      comm.sendMessage(answer, strlen(answer) + 1);

      delete answer;
      g_thread_yield();
    }

    g_thread_yield();
  }

}

void DebugServer::execute()
{
  g_debug("DebugServer::execute");
  while (g_async_queue_length(commands) > 0)
  {
    g_debug("there is something in the *commands* queue");
    char* cmdRaw = (char*) g_async_queue_pop(commands);

    g_debug("found %s in the *commands* queue", cmdRaw);


    GString* answer = g_string_new("");
    handleCommand(cmdRaw, answer);

    g_string_append(answer, "\n\0");

    g_async_queue_push(answers, g_string_free(answer, false));
    g_debug("pushed new answer");

    delete cmdRaw;
  }
}

void DebugServer::handleCommand(char* cmdRaw, GString* answer)
{
  // parse command
  GError* parseError = NULL;
  int argc;
  char** argv;
  g_shell_parse_argv(cmdRaw, &argc, &argv, &parseError);

  if (parseError)
  {
    g_string_append(answer, "parsing error: ");
    g_string_append(answer, parseError->message);
  } else
  {
    std::map<std::string, std::string> arguments;
    std::string commandName = "";
    // iterate over the command parts and build up the arguments as map
    bool answerAsBase64 = false;
    // command name
    if (argc > 0)
    {

      if (g_str_has_prefix(argv[0], "+"))
      {
        answerAsBase64 = true;
        commandName.assign(argv[0] + 1);
      } else
      {
        answerAsBase64 = false;
        commandName.assign(argv[0]);
      }
    }
    // argument names and if existings their values
    std::string lastKey;
    bool nextIsValue = false;
    bool valueIsBase64 = false;
    for (int i = 1; i < argc; i++)
    {
      if (nextIsValue)
      {
        if (lastKey != "")
        {
          if (valueIsBase64)
          {
            size_t len;
            char* decoded = (char*) g_base64_decode(argv[i], &len);
            arguments[lastKey].assign(decoded);
            g_free(decoded);
          } else
          {
            arguments[lastKey].assign(argv[i]);
          }
        }

        lastKey.assign("");
        nextIsValue = false;
      } else
      {
        if (g_str_has_prefix(argv[i], "-"))
        {
          nextIsValue = true;
          valueIsBase64 = false;

          lastKey.assign(argv[i] + 1);
        } else if (g_str_has_prefix(argv[i], "+"))
        {
          nextIsValue = true;
          valueIsBase64 = true;

          lastKey.assign(argv[i] + 1);
        } else
        {
          nextIsValue = false;
          lastKey.assign(argv[i]);
        }
        arguments[lastKey] = "";

      }
    }

    g_strfreev(argv);

    handleCommand(commandName, arguments, answer, answerAsBase64);

  }

}

void DebugServer::handleCommand(std::string command, std::map<std::string,
  std::string> arguments, GString* answer, bool encodeBase64)
{

  std::stringstream answerFromHandler;
  if(executorMap.find(command) != executorMap.end())
  {
    executorMap[command]->executeDebugCommand(command, arguments, answerFromHandler);
  }
  else
  {
    answerFromHandler << "received command \"" << command << "\", params[";
    for (std::map<std::string, std::string>::iterator it = arguments.begin();
      it != arguments.end(); it++)
    {
      answerFromHandler << it->first <<  ":" << it->second;
      if (it != arguments.end())
      {
        answerFromHandler <<  "; ";
      }
    }
    answerFromHandler << "]";
  }

  if(encodeBase64)
  {
    char* encoded = g_base64_encode((guchar*) answerFromHandler.str().c_str(),
      answerFromHandler.str().length());
    g_string_append(answer, encoded);
    g_free(encoded);
  }
  else
  {
    g_string_append(answer, answerFromHandler.str().c_str());
  }

}

bool DebugServer::registerCommand(std::string command, std::string description,
  DebugCommandExecutor* executor)
{
  if (executorMap.find(command) == executorMap.end())
  {
    // new command
    executorMap[command] = executor;
    descriptionMap[command] = description;
  }
}

void* DebugServer::dispatcher_static(void* ref)
{
  ((DebugServer*) ref)->dispatcher();
  return NULL;
}

DebugServer::~DebugServer()
{
  g_free(dispatcherThread);
  g_async_queue_unref(commands);
  g_async_queue_unref(answers);
}

