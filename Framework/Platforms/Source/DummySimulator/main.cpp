/* 
 * File:   main.cpp
 * Author: schlottb[at]informatik.hu-berlin.de
 *
 * Created on 2017.05.21
 */

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

int main(int argc, char** argv)
{
  print_info();

  g_type_init();

  bool backendMode = false;
  bool realTime = false;
  unsigned short port = 5401;

  // create the simulator instance
  DummySimulator sim(backendMode, realTime, port);
  
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

