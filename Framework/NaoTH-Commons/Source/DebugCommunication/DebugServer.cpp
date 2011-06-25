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

#include <Messages/Messages.pb.h>
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
        std::stringstream* answer = (std::stringstream*) g_async_queue_pop(answers);

        if(answer != NULL)
        {
          std::string answerAsString = answer->str();
          bool success = comm.sendMessage(answerAsString.c_str(), answerAsString.size());
          if(!success)
          {
            g_warning("could not send message");
            disconnect();
          }

          delete answer;
        } // end if answer != NULL
      }
    }

    if(comm.isConnected())
    {
      // 2. get new commands (maximal 50)
      try
      {
        GString* msg = NULL;
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
    GString* cmdRaw = (GString*) g_async_queue_pop(commands);

    std::stringstream* answer = new std::stringstream();
    handleCommand(cmdRaw, *answer);

    g_async_queue_push(answers, answer);

    g_free(cmdRaw);

  }//end while

  g_mutex_unlock(m_executing);

}//end execute


void DebugServer::handleCommand(GString* cmdRaw, std::stringstream& answer)
{
  // parse command

  naothmessages::Command cmd;
  if(cmd.ParseFromArray(cmdRaw->str, cmdRaw->len))
  {
    std::map<std::string, std::string> arguments;
    std::string commandName = cmd.name();

    for(int i=0; i < cmd.args().size(); i++)
    {
      const naothmessages::CMDArg& arg = cmd.args().Get(i);
      if(arg.has_bytes())
      {
        arguments[arg.name()] = arg.bytes();
      }
      else
      {
        arguments[arg.name()] = arg.name();
      }
    }

    handleCommand(commandName, arguments, answer);

  }
}//end handleCommand


void DebugServer::handleCommand(std::string& command, std::map<std::string,
  std::string>& arguments, std::stringstream& answer)
{
  if (executorMap.find(command) != executorMap.end())
  {
    executorMap[command]->executeDebugCommand(command, arguments, answer);
  } else
  {
    answer << "Unknown command \"" << command
      << "\", use \"help\" for a list of available commands" << std::endl;
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
    GString* tmp = (GString*) g_async_queue_pop(commands);
    g_string_free(tmp, true);
  }

  while (g_async_queue_length(answers) > 0)
  {
    std::stringstream* tmp = (std::stringstream*) g_async_queue_pop(answers);
    delete tmp;
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

