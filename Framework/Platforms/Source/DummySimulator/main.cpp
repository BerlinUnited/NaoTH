/* 
 * File:   main.cpp
 * Author: schlottb[at]informatik.hu-berlin.de
 *
 * Created on 2017.05.21
 */

#include <getopt.h>
#include <glib.h>
#include <glib-object.h>

#include <PlatformInterface/Platform.h>
//#include <Tools/Debug/Stopwatch.h>
#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/ModuleCreator.h>

#include <cstring>
#include "DummySimulator.h"

using namespace std;
using namespace naoth;

// kind of a HACK, needed by the logsimulator
extern ModuleManager* getModuleManager(Cognition* c);
extern ModuleManager* getModuleManager(Motion* m);

#define TO_STRING_INT(x) #x
#define TO_STRING(x) TO_STRING_INT(x)


void print_info() 
{
  std::cout << "=========================================="  << std::endl;
  std::cout << "DummySimulator compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

  #ifdef REVISION
  std::cout << "Revision number: " << TO_STRING(REVISION) << std::endl;
  #endif
  #ifdef USER_NAME
  std::cout << "Owner: " << TO_STRING(USER_NAME) << std::endl;
  #endif
  #ifdef BRANCH_PATH
  std::cout << "Branch path: " << TO_STRING(BRANCH_PATH) << std::endl;
  #endif
  std::cout << "==========================================\n"  << std::endl;
}

void print_help()
{
  std::cout <<
      "USAGE: ./dummysimulator [options]\n\n"
      "OPTIONS:\n"
      "-b|--backend:        Use DummySimulator with RobotControl\n"
      "-g|--gamecontroller: Listen to the GameController\n"
      "-t|--teamcomm:       Use TeamComm\n"
      "-i|--interface <n>:  The TeamComm interface, default = wifi0\n"
      "-d|--debug <n>:      Debug port, default = 5401\n"
      "-h|--help:           Show help\n";
  exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char** argv,
                     bool& backendMode,
                     unsigned short& port,
                     bool& useGameController,
                     bool& useTeamComms,
                     string& teamcommInterface)
{
  const char* const short_opts = "bgti:d:h";
  const option long_opts[] = {
      {"backend",        no_argument,       nullptr, 'b'},
      {"gamecontroller", no_argument,       nullptr, 'g'},
      {"teamcomm",       no_argument,       nullptr, 't'},
      {"interface",      required_argument, nullptr, 'i'},
      {"debug",          required_argument, nullptr, 'd'},
      {"help",           no_argument,       nullptr, 'h'},
      {nullptr,          0,                 nullptr,  0 }
  };

  int opt;
  while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
      switch (opt) {
          case 'b': backendMode = true; break;
          case 'g': useGameController = true; break;
          case 't': useTeamComms = true; break;
          case 'i': teamcommInterface.assign(optarg); break;
          case 'd': port = atoi(optarg); break; // debug port
          case 'h': // -h or --help
          case '?': // Unrecognized option
          default:
              print_help();
              break;
      }
  }
}

int main(int argc, char** argv)
{
  print_info();

  g_type_init();

  bool backendMode = false;
  unsigned short port = 5401;
  bool useGameController = false;
  bool useTeamComms = false;
  string teamcommInterface = "wlan0";

  parse_arguments(argc, argv, backendMode, port, useGameController, useTeamComms, teamcommInterface);

  // create the simulator instance
  DummySimulator sim(backendMode, port);
  
  // init the platform
  Platform::getInstance().init(&sim);

  if (useGameController) { sim.enableGameController(); }
  if (useTeamComms) { sim.enableTeamComm(teamcommInterface); }

  //init_agent(sim);
  Cognition* theCognition = createCognition();
  Motion* theMotion = createMotion(); // crashes at inversekinematics

  // ACHTUNG: C-Cast (!)
  ModuleManager* theCognitionManager = getModuleManager(theCognition);
  if (!theCognitionManager)
  {
    std::cerr << "ERROR: theCognition is not of type ModuleManager" << std::endl;
    return (EXIT_FAILURE);
  }

  // register processes
  sim.registerCognition((naoth::Callable*)(theCognition));
  sim.registerMotion((naoth::Callable*)(theMotion));

  // start the execution
  sim.main();

  deleteCognition(theCognition);
  deleteMotion(theMotion);

  return (EXIT_SUCCESS);
}

