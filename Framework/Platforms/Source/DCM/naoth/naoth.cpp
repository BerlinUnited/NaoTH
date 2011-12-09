/**
 * @file naoth.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "PlatformInterface/Platform.h"
#include "NaoController.h"
#include "NaoMotionController.h"

#include "Cognition.h"
#include "Motion.h"
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


void* cognitionThreadCallback(void* ref)
{
  NaoController theController;
  Cognition theCognition;
  theController.registerCallbacks((DummyCallable*)NULL, &theCognition);
  
  while(true)
  {
    theController.callCognition();
    //usleep(100000);
  }

  return NULL;
}//end cognitionThreadCallback


void* motionThreadCallback(void* ref)
{
  NaoMotionController theController;
  Motion theMotion;
  theController.registerCallbacks(&theMotion, (DummyCallable*)NULL);
  
  while(true)
  {
    theController.callMotion();
    //usleep(100000);
  }

  return NULL;
}//end motionThreadCallback


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
  

  GError* err = NULL;

  GThread* motionThread = g_thread_create(motionThreadCallback, NULL, true, &err);
  if(err)
  {
    g_warning("Could not create motion thread: %s", err->message);
  }

  GThread* cognitionThread = g_thread_create(cognitionThreadCallback, NULL, true, &err);
  if(err)
  {
    g_warning("Could not create cognition thread: %s", err->message);
  }



  if(motionThread != NULL)
  {
    g_thread_join(motionThread);
  }

  if(cognitionThread != NULL)
  {
    g_thread_join(cognitionThread);
  }

  return 0;
}
