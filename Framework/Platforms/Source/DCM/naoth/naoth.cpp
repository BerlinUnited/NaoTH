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

int main(int argc, char *argv[])
{
  // init glib
  g_type_init();
  if (!g_thread_supported())
    g_thread_init(NULL);
  
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
