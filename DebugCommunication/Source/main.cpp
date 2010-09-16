/* 
 * File:   main.cpp
 * Author: thomas
 *
 * Created on 10. September 2010, 15:50
 */

#include <cstdlib>
#include <iostream>

#include "DebugServer.h"

using namespace std;

class TestDebugCommandExecutor : public DebugCommandExecutor
{
  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream)
  {
    outstream << "pong\n";
  }
};

/*
 * 
 */
int main(int argc, char** argv)
{
  g_type_init();
  g_thread_init(NULL);

  DebugServer server(5401);
  TestDebugCommandExecutor* test = new TestDebugCommandExecutor();

  server.registerCommand("ping", "Gives you a \"pong\"", test);

  while(true)
  {
    server.execute();
    sleep(1);
  }

  return 0;
}

