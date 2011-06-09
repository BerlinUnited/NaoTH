/**
 * @file naoth.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "naoth.h"
#include "NaoController.h"
#include "PlatformInterface/Platform.h"
#include "Cognition.h"
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
  DummyMotion theMotion;
  theController.registerCallbacks(&theMotion, &theCognition);
  
  while(true)
  {
    theController.callCognition();
  }
  
  return 0;
}
