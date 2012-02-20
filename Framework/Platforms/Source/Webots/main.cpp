/**
 * @file main.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include <Webots/WebotsController.h>

#include "Cognition.h"
#include "Motion.h"

#include <glib.h>
#include <glib-object.h>

using namespace std;

void printUsage()
{
  cout << "NaoTH Webots controller" << endl;
  cout << "-------------------------" << endl;
  cout << "Compiled: " << __DATE__ << " at " << __TIME__ << endl;
  #ifdef BZR_REVISION
  cout << "Revision number: " << BZR_REVISION << endl;
  #endif
  cout << "-------------------------" << endl;
  cout << "http://www.naoth.de" << endl << endl;
}//end printUsage

int main(int argc, char** argv)
{
  g_type_init();

  printUsage();

  wb_robot_init();
  naoth::WebotsController theController;

  theController.init();

  Cognition theCognition;
  Motion theMotion;
  theController.registerCallbacks(&theMotion, &theCognition);

  theController.main();

  return 0;
}//end main

