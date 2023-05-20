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

void parse_arguments(int argc, char** argv,
                     gboolean& backendMode,
                     gint& port,
                     gboolean& useGameController,
                     gboolean& useTeamComms,
                     gchar*& teamcommInterface,
                     gint& playerNumber)
{
  GOptionEntry entries[] = {
    {"backend",       'b', 0, G_OPTION_ARG_NONE,   &backendMode,       "Use DummySimulator with RobotControl", NULL},
    {"gamecontroller",'g', 0, G_OPTION_ARG_NONE,   &useGameController, "Listen to the GameController", NULL},
    {"teamcomm",      't', 0, G_OPTION_ARG_NONE,   &useTeamComms,      "Use TeamComm", NULL},
    {"interface",     'i', 0, G_OPTION_ARG_STRING, &teamcommInterface, "The TeamComm interface, default = wlan0", "wlan0"},
    {"debug",         'd', 0, G_OPTION_ARG_INT,    &port,              "Debug port, default = 5401", "5401"},
    {"num",           'n', 0, G_OPTION_ARG_INT,    &playerNumber,      "Player number", "0"},
    {NULL,0,0,G_OPTION_ARG_NONE, NULL, NULL, NULL} // This NULL is very important!!!
  };

  GError *error = NULL;
  GOptionContext *context = g_option_context_new(NULL);
  g_option_context_add_main_entries (context, entries, NULL);
  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
    g_print ("option parsing failed: %s\n", error->message);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv)
{
  print_info();

  g_type_init();

  gboolean backendMode = false;
  gint port = 5401;
  gboolean useGameController = false;
  gboolean useTeamComms = false;
  gchar* teamcommInterface = "wlan0";
  gint playerNumber = 0; // zero means read from config

  parse_arguments(argc, argv, backendMode, port, useGameController, useTeamComms, teamcommInterface, playerNumber);

  // create the simulator instance
  // TODO: why is it unsigned short?
  DummySimulator sim(backendMode, static_cast<unsigned short>(port));
  
  // init the platform
  Platform::getInstance().init(&sim);

  if (useGameController) { sim.enableGameController(); }
  if (useTeamComms) { sim.enableTeamComm(teamcommInterface); }
  if (playerNumber != 0) { Platform::getInstance().theConfiguration.setInt("player", "PlayerNumber", (int)playerNumber); }

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

