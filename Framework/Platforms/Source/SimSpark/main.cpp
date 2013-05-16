/**
 * @file main.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#include <glib.h>
#include <glib-object.h>

#include <SimSpark/SimSparkController.h>
#include <sstream>

using namespace std;


// allows for changing the platform name during the compilation
#ifndef PLATFORM_NAME
#define PLATFORM_NAME SimSpark
#endif

#define MAKE_STRING(name) #name
#define MAKE_NAME(name) MAKE_STRING(name)


int main(int argc, char** argv)
{
  g_type_init();

  string teamName = "NaoTH";
  gchar* optTeamName = NULL;
  unsigned int num = 0; // zero means get a number from server
  string server = "localhost";
  gchar* optServer = NULL;
  unsigned int port = 3100;
  gboolean sync = false;
  
  GOptionEntry entries[] = {
    {"num",'n', 0, G_OPTION_ARG_INT, &num, "player number", "0"},
    {"team", 't', 0, G_OPTION_ARG_STRING, &optTeamName, "team name", "NaoTH"},
    {"server", 's', 0, G_OPTION_ARG_STRING, &optServer, "server host", "localhost"},
    {"port", 'p', 0, G_OPTION_ARG_INT, &port, "server port", "3100"},
    {"sync", 0, 0,  G_OPTION_ARG_NONE, &sync, "sync mode", NULL},
    {NULL} // This NULL is very important!!!
  };
  
  std::stringstream info;
  info << "=========================================="  << std::endl;
  info << " http://www.naoth.de " << std::endl;
  info << "NaoTH compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;
  #ifdef REVISION
  info << "Revision number: " << MAKE_STRING(REVISION) << std::endl;
  #endif
  #ifdef USER_NAME
  info << "Owner: " << MAKE_STRING(USER_NAME) << std::endl;
  #endif
  #ifdef BRANCH_PATH
  info << "Branch path: " << MAKE_STRING(BRANCH_PATH) << std::endl;
  #endif
  info << "==========================================\n"  << std::endl;


  GError *error = NULL;
  GOptionContext *context = g_option_context_new(info.str().c_str());
  g_option_context_add_main_entries (context, entries, "NaoTH Simspark (SPL) controller");
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
  g_option_context_free(context);
  
  SimSparkController theController(MAKE_NAME(PLATFORM_NAME));

  if (!theController.init(teamName, num, server, port, sync > 0))
  {
      cerr << "NaoTH SimSpark (SPL) initialization failed!" << endl;
    return EXIT_FAILURE;
  }

  naoth::init_agent(theController);

  theController.main();

  return 0;
}//end main

