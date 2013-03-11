/* 
 * File:   Main.cpp
 * Author: Kirill Yasinovskiy
 *
 *  Created on 1. February 2012
 */

#include <glib.h>
#include <glib-object.h>

#include <OpenCVWebCam/OpenCVWebCamController.h>

#include <PlatformInterface/Platform.h>
#include <Tools/Debug/Stopwatch.h>


void printUsage()
{
  cout << "Berlin United OpenCV WebCam controller" << endl;
  cout << "-------------------------" << endl;
  cout << "http://www.naoth.de" << endl << endl;
}//end printUsage

int main(int argc, char** argv)
{
  g_type_init();

  printUsage();

  OpenCVWebCamController theController;

  theController.init();

  // init the platform
  Platform::getInstance().init(&theController);

  naoth::init_agent(theController);

  theController.main();

  return 0;
}//end main

