/**
 * @file main.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#include <glib.h>
#include <glib-object.h>

#include <SimSpark/SimSparkController.h>

#include "Cognition.h"
#include "Motion.h"

using namespace std;

void printUsage(char* name)
{
  cout << "NaoTH Simspark controller" << endl;
  cout << "-------------------------" << endl;
  cout << "http://www.naoteamhumboldt.de" << endl << endl;
  cout << "Usage: "<<name<<" [teamname [playernumber [server [port]]]]" << endl << endl;
}

int main(int argc, char** argv)
{
  g_type_init();
  
  string teamName = "NaoTH";
  unsigned int num = 0; // zero means get a number from server
  string server = "localhost";
  unsigned int port = 3100;

  if (argc == 2 &&
    (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
  {
    printUsage(argv[0]);
    exit(0);
  }

  // read options
  if (argc > 1)
  {
    teamName = string(argv[1]);
  }
  if (argc > 2)
  {
    num = atoi(argv[2]);
  }
  if (argc > 3)
  {
    server = string(argv[3]);
  }
  if (argc > 4)
  {
    port = atoi(argv[4]);
  }

  SimSparkController theController;

  if (!theController.init(teamName, num, server, port))
  {
    cerr << "NaoTH SimSpark initialization failed!" << endl;
    return EXIT_FAILURE;
  }

  Cognition theCognition;
  Motion theMotion;
  theController.registerCallbacks(&theMotion, &theCognition);

  //theController.main();
  theController.multiThreadsMain();

  return 0;
}//end main

