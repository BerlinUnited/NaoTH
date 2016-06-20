/* 
 * File:   Main.cpp
 * Author: thomas
 *
 * Created on 27. November 2008, 17:20
 */

#include <glib.h>
#include <glib-object.h>

#include <PlatformInterface/Platform.h>
//#include <Tools/Debug/Stopwatch.h>
#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/ModuleCreator.h>

#include <cstring>
#include "Simulator.h"

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
  std::cout << "LogSimulator compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

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

/*
 * 
 */
int main(int argc, char** argv)
{
  print_info();

  g_type_init();
  
  bool backendMode = false;
  bool realTime = false;
  unsigned short port = 5401;

  char* logpath = getenv("NAOTH_LOGFILE");
  if(logpath == NULL && argc > 1) {
    logpath = argv[argc-1];
  }

  // search for parameters
  for (int i = 1; i <= argc - 1; i++) {
    if (strcmp(argv[i], "-b") == 0) {
      backendMode = true;
    }
    if (strcmp(argv[i], "-r") == 0) {
      realTime = true;
    }
    if (strcmp(argv[i], "-h") == 0) {
        std::cout << "syntax: (-b)? (-r)? (-h)? (-p <port number>)? <logfile>" << std::endl;
        std::cout << "\"-b\" enable the backend mode which is only used by LogfilePlayer of RobotControl" << std::endl;
        std::cout << "\"-r\" play and loop the logfile according to the time recorded in the FrameInfo of the logfile" << std::endl;
        std::cout << "\"-p\" debug port number, range of valid values: [1,65535]" << std::endl;
        std::cout << "\"-h\" help" << std::endl;
        return (EXIT_SUCCESS);
    }
    if (strcmp(argv[i], "-p") == 0) {
      port = (unsigned short) strtol(argv[++i],0,10);
      if (port == 0) {
          cerr << "invalid port number" << endl;
          return (EXIT_FAILURE);
      }
    }
  }

  if(logpath == NULL)
  {
    cerr << "You need to give the path to the logfile as argument" << endl;
    cerr << "arguments: (-b)? (-r)? (-h)? (-p <port number>)? <logfile>" << endl;
    cerr << "\"-b\" enable the backend mode which is only used by LogfilePlayer of RobotControl" << endl;
    cerr << "\"-r\" play and loop the logfile according to the time recorded in the FrameInfo of the logfile" << endl;
    cerr << "\"-p\" debug port number, range of valid values: [1,65535]" << std::endl;
    cerr << "\"-h\" help" << endl;
    return (EXIT_FAILURE);
  }
  
  // create the simulator instance
  Simulator sim(logpath, backendMode, realTime, port);
  
  // init the platform
  Platform::getInstance().init(&sim);
  
  //init_agent(sim);
  Cognition* theCognition = createCognition();
  Motion* theMotion = createMotion();

  // ACHTUNG: C-Cast (!)
  ModuleManager* theCognitionManager = getModuleManager(theCognition);
  if(!theCognitionManager)
  {
    std::cerr << "ERROR: theCognition is not of type ModuleManager" << std::endl;
    return (EXIT_FAILURE);
  }

  ModuleManager* theMotionManager = getModuleManager(theMotion);
  if(!theMotionManager)
  {
    std::cerr << "ERROR: theMotion is not of type ModuleManager" << std::endl;
    return (EXIT_FAILURE);
  }

  // register a module to provide all the logged data
  ModuleCreator<LogProvider>* theLogProviderCognition = theCognitionManager->registerModule<LogProvider>(std::string("LogProvider"));
  ModuleCreator<LogProvider>* theLogProviderMotion = theMotionManager->registerModule<LogProvider>(std::string("LogProvider"));

  // register processes
  sim.registerCognition((naoth::Callable*)(theCognition));
  sim.registerMotion((naoth::Callable*)(theMotion));
 
  theLogProviderCognition->setEnabled(true);
  theLogProviderCognition->getModuleT()->init(sim.logFileScanner, sim.getRepresentations(), sim.getIncludedRepresentations());
  
  theLogProviderMotion->setEnabled(true);
  theLogProviderMotion->getModuleT()->init(sim.logFileScanner, sim.getRepresentations(), sim.getIncludedRepresentations());

  // start the execution
  sim.main();
  
  // dump some debug information
  //StopwatchManager::getInstance().dump();

  deleteCognition(theCognition);
  deleteMotion(theMotion);

  return (EXIT_SUCCESS);
}//end main

