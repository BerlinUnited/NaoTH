/* 
 * File:   Main.cpp
 * Author: thomas
 *
 * Created on 27. November 2008, 17:20
 */

#include <glib.h>
#include <glib-object.h>

#include <PlatformInterface/Platform.h>
#include <Tools/Debug/Stopwatch.h>
#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/ModuleCreator.h>

#include <cstring>
#include "Simulator.h"

using namespace std;
using namespace naoth;

// kind of a HACK, needed by the logsimulator
extern ModuleManager* getModuleManager(Cognition* c);
//extern int main(int argc, char** argv);

/*
 * 
 */
int main(int argc, char** argv)
{
  g_type_init();
  
  bool compatibleMode = false;
  bool backendMode = false;

  char* logpath = getenv("NAOTH_LOGFILE");

  if(logpath == NULL)
  {
    if(argc > 1)
    {
      logpath = argv[argc-1];

      if(argc > 2)
      {
        for (int i = 1; i < argc - 1; i++)
        {
          // check if compatible mode
          if (strcmp(argv[i], "-c") == 0)
          {
            compatibleMode = true;
          } else if (strcmp(argv[i], "-b") == 0)
          {
            backendMode = true;
          }
        }//end for
      }//end if > 2
    }//end if > 1
  }
  else
  {
    compatibleMode = getenv("NAOTH_LOGCOMPAT") != NULL;
  }


  if(logpath == NULL)
  {
    cerr << "You need to give the path to the logfile as argument" << endl;
    cerr << "arguments: (-b)? (-c)? <logfile>" << endl;
    cerr << "\"-b\" enable the backend mode which is only used by LogfilePlayer of RobotControl" << endl;
    cerr << "when using \"-c\" you are forcing a compatible mode" << endl;
    return (EXIT_FAILURE);
  }
  
  // create the simulator instance
  Simulator sim(logpath, compatibleMode, backendMode);
  
  // init the platform
  Platform::getInstance().init(&sim);
  
  //init_agent(sim);
  Cognition& theCognition = *createCognition();
  Motion& theMotion = *createMotion();

  // ACHTUNG: C-Cast (!)
  ModuleManager* theCognitionManager = getModuleManager(&theCognition);
  if(!theCognitionManager)
  {
    std::cerr << "ERROR: theCognition is not of type ModuleManager" << std::endl;
    return (EXIT_FAILURE);
  }

  // register a module to provide all the logged data
  ModuleCreator<LogProvider>* theLogProvider = theCognitionManager->registerModule<LogProvider>(std::string("LogProvider"));

  // register processes
  sim.registerCognition((naoth::Callable*)(&theCognition));
  sim.registerMotion((naoth::Callable*)(&theMotion));
 
  theLogProvider->setEnabled(true);
  theLogProvider->getModuleT()->init(sim.getRepresentations(), sim.getIncludedRepresentations());
  

  // start the execution
  sim.main();
  
  // dump some debug information
  StopwatchManager::getInstance().dump();

  return (EXIT_SUCCESS);
}//end main

