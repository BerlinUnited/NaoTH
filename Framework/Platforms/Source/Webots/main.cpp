/**
 * @file main.cpp
 *
 * @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Benjamin</a>
 *
 */

#include <glib.h>
#include <glib-object.h>

#include <Webots/WebotsController.h>
#include <sstream>

using namespace std;


// allows for changing the platform name during the compilation
#ifndef PLATFORM_NAME
#define PLATFORM_NAME Webots
#endif

#define MAKE_STRING(name) #name
#define MAKE_NAME(name) MAKE_STRING(name)


// TODO: this can be generalized
#ifndef REVISION
#define REVISION "unknown"
#endif

#ifndef USER_NAME
#define USER_NAME "unknown"
#endif

#ifndef BRANCH_PATH
#define BRANCH_PATH "unknown"
#endif

#define NAOTH_BUILD_INFO                         \
  "=========================================="   \
  " http://www.naoth.de                      "   \
  "NaoTH compiled on: " __DATE__" at " __TIME__ \
  "Revision number: " REVISION                   \
  "Owner: " USER_NAME                            \
  "Branch path: " BRANCH_PATH                    \
  "==========================================\n"


int main(int argc, char** argv)
{
  g_type_init();

  // player configuration
  unsigned int teamNumber = 0; // zero means read from config
  unsigned int playerNumber = 0; // zero means get a number from server
  string teamName = "";
  gchar* optTeamName = NULL;
  
  // server configuration
  string server = "localhost";
  unsigned int port = 1412;
  gchar* optServer = NULL;

  // simulation config
  string modelPath = "rsg/agent/naov4/nao.rsg";
  gchar* optModelPath = NULL;
  gboolean sync = false;
  
  GOptionEntry entries[] = {
    {"team_number", 'm', 0, G_OPTION_ARG_INT, &teamNumber, "team number", "0"},
    {"num",'n', 0, G_OPTION_ARG_INT, &playerNumber, "player number", "0"},
    {"team", 't', 0, G_OPTION_ARG_STRING, &optTeamName, "team name", "NaoTH"},
    {"server", 's', 0, G_OPTION_ARG_STRING, &optServer, "server host", "localhost"},
    {"port", 'p', 0, G_OPTION_ARG_INT, &port, "server port", "3100"},
    {"model_path", 0, 0, G_OPTION_ARG_STRING, &optModelPath, "path for the model of the robot", "rsg/agent/naov4/nao.rsg"},
    {"sync", 0, 0,  G_OPTION_ARG_NONE, &sync, "sync mode", NULL},
    {NULL,0,0,G_OPTION_ARG_NONE, NULL, NULL, NULL} // This NULL is very important!!!
  };
  

  GError *error = NULL;
  GOptionContext *context = g_option_context_new(NAOTH_BUILD_INFO);
  g_option_context_add_main_entries (context, entries, "NaoTH Webots (SPL) controller");
  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
    g_print ("option parsing failed: %s\n", error->message);
    return EXIT_FAILURE;
  }

  if ( optTeamName != NULL )
  {
    teamName = optTeamName;
    g_free(optTeamName);
  }
  if ( optServer != NULL )
  {
    server = optServer;
    g_free(optServer);
  }
  if ( optModelPath != NULL )
  {
    modelPath = optModelPath;
    g_free(optModelPath);
  }
  g_option_context_free(context);
  
  WebotsController theController(MAKE_NAME(PLATFORM_NAME));

  if (!theController.init(modelPath, teamName, teamNumber, playerNumber, server, port, sync > 0))
  {
    cerr << "NaoTH Webots (SPL) initialization failed!" << endl;
    return EXIT_FAILURE;
  }

  naoth::init_agent(theController);

  theController.main();

  return 0;
}//end main

