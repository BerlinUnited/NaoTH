/**
 * @file naoth.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "NaoController.h"
#include "PlatformInterface/Platform.h"
#include "Cognition.h"
#include "Tools/DummpyCallable.h"
#include <glib.h>
#include <glib-object.h>

using namespace naoth;

void got_signal(int)
{
  // do something
  std::cout << "catched signal" << std::endl;
  Trace::getInstance().dump();
  Stopwatch::getInstance().dump("cognition");

  sync();

  exit(0);
}

int main(int argc, char *argv[])
{
  // init glib
  g_type_init();
  if (!g_thread_supported())
    g_thread_init(NULL);


  // react on "kill"
  struct sigaction sa;
  memset( &sa, 0, sizeof(sa) );
  sa.sa_handler = got_signal;
  sigfillset(&sa.sa_mask);
  //sigaction(SIGINT,&sa,NULL);
  sigaction(SIGTERM,&sa,NULL);
  
  NaoController theController;
  Cognition theCognition;
  theController.registerCallbacks((DummyCallable*)NULL, &theCognition);
  
  while(true)
  {
    theController.callCognition();
    //usleep(100000);
  }
  
  return 0;
}
