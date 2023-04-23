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
      "-p|--port <n>:       Debug port, default = 5401\n"
      "-h|--help:           Show help\n";
  exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char** argv, bool& backendMode, unsigned short& port, bool& useGameController)
{
  const char* const short_opts = "bgp:h";
  const option long_opts[] = {
      {"backend",        no_argument,       nullptr, 'b'},
      {"gamecontroller", no_argument,       nullptr, 'g'},
      {"port",           required_argument, nullptr, 'p'},
      {"help",           no_argument,       nullptr, 'h'},
      {nullptr,          0,                 nullptr,  0 }
  };

  int opt;
  while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
      switch (opt) {
          case 'b': backendMode = true; break;
          case 'g': useGameController = true; break;
          case 'p': port = atoi(optarg); break;
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

  parse_arguments(argc, argv, backendMode, port, useGameController);

  // create the simulator instance
  DummySimulator sim(backendMode, port, useGameController);
  
  // init the platform
  Platform::getInstance().init(&sim);

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

