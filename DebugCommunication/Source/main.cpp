/* 
 * File:   main.cpp
 * Author: thomas
 *
 * Created on 10. September 2010, 15:50
 */

#include <cstdlib>

#include "DebugServer.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
  g_type_init();
  g_thread_init(NULL);

  DebugServer server;

  return 0;
}

