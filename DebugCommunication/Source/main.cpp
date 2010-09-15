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

/*
 * 
 */
int main(int argc, char** argv)
{
  g_type_init();
  g_thread_init(NULL);

  DebugServer server(5401);

  while(true)
  {
    server.execute();
    sleep(1);
  }

  return 0;
}

