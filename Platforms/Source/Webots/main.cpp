/**
 * @file main.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include <Webots/WebotsController.h>

#include "Cognition.h"
#include "Motion.h"

using namespace std;

void printUsage()
{
  cout << "NaoTH Webots controller" << endl;
  cout << "-------------------------" << endl;
  cout << "http://www.naoteamhumboldt.de" << endl << endl;
}//end printUsage

int main(int argc, char** argv)
{
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

