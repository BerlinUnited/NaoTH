/* 
 * File:   DebugServer.cpp
 * Author: thomas
 * 
 * Created on 10. September 2010, 15:38
 */

#include <string.h>

#include <string>
#include <cstdlib>

#include <iostream>
#include <sstream>

#include "DebugServer.h"

DebugServer::DebugServer()
  : frameEnded(false), abort(false)
{
  m_executing = g_mutex_new();
  m_abort = g_mutex_new();

  commands = g_async_queue_new();
  answers = g_async_queue_new();

  g_async_queue_ref(commands);
  g_async_queue_ref(answers);
}//end DebugServer


void DebugServer::start(unsigned short port)
{
  if (g_thread_supported())
  {
    comm.init(port);

    GError* err = NULL;
    g_debug("Starting debug server thread");
    connectionThread = g_thread_create(connection_thread_static, this, true, &err);
    if(err)
    {
      g_warning("Could not start debug server thread: %s", err->message);
    }

    registerCommand("help", "list available commands or get the description of a specific command", this);
    registerCommand("endFrame",
      "marking a frame as ended, thus the processing of rest of the messages will be done in the next cycle",
       this);
  }
  else
  {
    g_warning("No threading support: DebugServer not available");
  }
}//end start

void DebugServer::mainConnection()
{
  g_async_queue_ref(commands);
  g_async_queue_ref(answers);

  while(true)
  {
    g_mutex_lock(m_abort);
    if(abort)
    {
      g_mutex_unlock(m_abort);
      break; // end loop;
    }
    g_mutex_unlock(m_abort);

    if(comm.isConnected())
    {
      // 1. send out already answered messages
      while(g_async_queue_length(answers) > 0)
      {
        GString* answer = (GString*) g_async_queue_pop(answers);

        if(answer != NULL)
        {
          bool success = comm.sendMessage(answer->str, answer->len+1);
          if(!success)
          {
            g_warning("could not send message");
            disconnect();
          }

          g_string_free(answer, true);
        } // end if answer != NULL
      }
    }

    if(comm.isConnected())
    {
      // 2. get new commands (maximal 50)
      try
      {
        char* msg = NULL;
        unsigned int counter = 0;
        do
        {
          msg = comm.readMessage();
          if(msg != NULL)
          {
            g_async_queue_push(commands, msg);
          }
          counter++;
        } while(msg != NULL && counter < 50);
      }
      catch(...)
      {
        // error occured, clear all queues and disconnect
        disconnect();
      } // end catch
    }

    if(!comm.isConnected())
    {
      // connect again, wait max 1 second until connection is etablished
      comm.connect(1);
    } // end if connected

    // always give other thread the possibility to gain control before entering
    // the loop again
    g_usleep(1000);
    g_thread_yield();

  } // end while true

  g_async_queue_unref(commands);
  g_async_queue_unref(answers);
}

void DebugServer::disconnect()
{
  // stop executing (so it's not messing up with our queues)
  g_mutex_lock(m_executing);
  clearBothQueues();
  g_mutex_unlock(m_executing);

  // all commands are "answered", disconnect
  comm.disconnect();
}


void DebugServer::execute()
{
  g_mutex_lock(m_executing);

  frameEnded = false;

  // handle all commands
  while (!frameEnded && g_async_queue_length(commands) > 0)
  {
    char* cmdRaw = (char*) g_async_queue_pop(commands);

    GString* answer = g_string_new("");
    handleCommand(cmdRaw, answer);

    g_async_queue_push(answers, answer);

    g_free(cmdRaw);

  }//end while

  g_mutex_unlock(m_executing);

}//end execute


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
  }
  else
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
            std::string arg(argv[i]);
            char* decoded = (char*) malloc(arg.size());
            int decodedSize = base64Decoder.decode(arg, decoded, arg.size());
            //char* decoded = (char*) g_base64_decode(argv[i], (gsize*) &len);
            arguments[lastKey].assign(decoded, decodedSize);
            free(decoded);
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
}//end handleCommand


void DebugServer::handleCommand(std::string command, std::map<std::string,
  std::string> arguments, GString* answer, bool encodeBase64)
{
  std::stringstream answerFromHandler;
  
  if (executorMap.find(command) != executorMap.end())
  {
    executorMap[command]->executeDebugCommand(command, arguments, answerFromHandler);
  } else
  {
    answerFromHandler << "Unknown command \"" << command
      << "\", use \"help\" for a list of available commands" << std::endl;
  }
  
  const std::string& str = answerFromHandler.str();

  if (encodeBase64 && str.length() > 0)
  {
    std::string encoded = base64Encoder.encode((const char*) str.c_str(), str.length());
    //char* encoded = g_base64_encode((guchar*) str.c_str(), str.length());

    g_string_append(answer, encoded.c_str());
//    g_free(encoded);
  } else
  {
    g_string_append(answer, str.c_str());
  }
}//end handleCommand


bool DebugServer::registerCommand(std::string command, std::string description,
  DebugCommandExecutor* executor)
{
  if (executorMap.find(command) == executorMap.end())
  {
    // new command
    executorMap[command] = executor;
    descriptionMap[command] = description;
    executor->registerDestructionListener(*this);
    return true;
  }
  return false;
}//end registerCommand


void DebugServer::objectDestructed(DebugCommandExecutor* object)
{
  std::list<std::string> registeredKeys;

  // search all registered keys of the object
  std::map<std::string, DebugCommandExecutor*>::const_iterator iter;
  for (iter = executorMap.begin(); iter != executorMap.end(); iter++)
  {
    if ((*iter).second == object)
    {
      registeredKeys.push_back((*iter).first);
    }//end if
  }//end for

  // unregister all found commands
  std::list<std::string>::const_iterator iter_key;
  for (iter_key = registeredKeys.begin(); iter_key != registeredKeys.end(); iter_key++)
  {
    executorMap.erase(*iter_key);

    g_debug("unregistering command %s", (*iter_key).c_str());
  }//end for
}

void DebugServer::executeDebugCommand(const std::string& command, const std::map<std::string, std::string>& arguments,
  std::ostream& out)
{
  if (command == "help")
  {
    if (arguments.empty())
    {
      // list all available commands
      out << "Available commands, use \"help <command>\" for a description:\n";
      std::map<std::string, std::string>::const_iterator iter = descriptionMap.begin();
      while (iter != descriptionMap.end())
      {
        out << iter->first;
        iter++;
        if (iter != descriptionMap.end())
        {
          out << ": " << iter->second << "\n";
        }
      }
    }
    else
    {
      std::string firstArg = arguments.begin()->first;
      if (descriptionMap.find(firstArg) != descriptionMap.end())
      {
        out << firstArg << "\n";
        out << "------------------\n";
        out << descriptionMap[firstArg];
        out << "\n";
      } else
      {
        out << "Unknown command \"" << firstArg
          << "\", use \"help\" for a list of available commands";
      }

    }
    out << "\n";
  }
  else if( command == "endFrame")
  {
    frameEnded = true;
  }
}

void DebugServer::clearBothQueues()
{
  while (g_async_queue_length(commands) > 0)
  {
    char* tmp = (char*) g_async_queue_pop(commands);
    g_free(tmp);
  }

  while (g_async_queue_length(answers) > 0)
  {
    GString* tmp = (GString*) g_async_queue_pop(answers);
    g_string_free(tmp, true);
  }

}

void* DebugServer::connection_thread_static(void* ref)
{
  ((DebugServer*) ref)->mainConnection();
  return NULL;
}


DebugServer::~DebugServer()
{
  g_mutex_lock(m_abort);
  abort = true;
  g_mutex_unlock(m_abort);

  g_thread_join(connectionThread);

  clearBothQueues();
  comm.disconnect();

  g_mutex_free(m_executing);
  g_mutex_free(m_abort);

  g_async_queue_unref(commands);
  g_async_queue_unref(answers);

}

