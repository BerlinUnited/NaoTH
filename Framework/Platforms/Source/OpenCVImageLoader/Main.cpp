/*
* File:   Main.cpp
* Author: Kirill Yasinovskiy
*
* Created on 24. Juni 2011
*/


//glib
#include <glib.h>
#include <glib-object.h>

#include <PlatformInterface/Platform.h>
#include <Tools/Debug/Stopwatch.h>

#include "Cognition.h"
#include "Motion.h"

#include <cstring>
#include "OpenCVImageLoader.h"

using namespace std;

int main(int argc, char** argv)
{
  g_type_init();
  char* logpath = getenv("NAOTH_IMAGEDIR");

  //we don't have any environment variable with this name
  if (logpath == NULL)
  {
    if (argc >= 1)
    {
      logpath = argv[argc - 1];
    }
  }
  if (logpath == NULL)
  {
    cerr << "It was impossible to find a environment variable with the name of directory with images, so..." << endl;
    cerr << "...you need to give the path to the directory with images as argument" << endl;
    cerr << "argument <directory>" << endl;
    return (EXIT_FAILURE);
  }

  OpenCVImageLoader loader(logpath);
  
  // init the platform
  Platform::getInstance().init(&loader);

  Cognition theCognition;
  Motion theMotion;
  loader.registerCallbacks(&theMotion, &theCognition);

  // start the execution
  loader.main();

  // dump some debug information
  Stopwatch::getInstance().dump();

  return (EXIT_SUCCESS);
}//end main

