/**
 * @file naoth.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "PlatformInterface/Platform.h"
#include "NaoController.h"
#include "NaoMotionController.h"
#include "DebugCommunication/DebugServer.h"
#include "Tools/Debug/DebugBufferedOutput.h"

#include "Cognition.h"
#include "Motion.h"
#include "Tools/DummpyCallable.h"

#include <glib.h>
#include <glib-object.h>
#include <rttools/rtthread.h>

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

/*
class TestThread : public RtThread
{
	public:
    TestThread(){}
    ~TestThread(){}
    virtual void *execute(){}
    virtual void postExecute(){}
    virtual void preExecute(){}
};
*/

DebugServer* theDebugServer;

void* cognitionThreadCallback(void* ref)
{
  NaoController theController;
  Cognition theCognition;
  theController.registerCallbacks((DummyCallable*)NULL, &theCognition);
  theController.setDebugServer(theDebugServer);

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
  theController.setDebugServer(theDebugServer);
  
  // open semaphore
  sem_t* sem = SEM_FAILED;
  if((sem = sem_open("motion_trigger", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
  {
    perror("libnaoth: sem_open");
  }
  StopwatchItem stopwatch;

  while(true)
  {
    theController.callMotion();
    
    sem_wait(sem);
    Stopwatch::getInstance().notifyStop(stopwatch);
    Stopwatch::getInstance().notifyStart(stopwatch);
    PLOT("_MotionCycle", stopwatch.lastValue);
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
  

  theDebugServer = new DebugServer();
  theDebugServer->start(5401, true);

  GError* err = NULL;

  pthread_t handle;
  pthread_create(&handle, 0, motionThreadCallback, NULL);

  sched_param param;
  param.sched_priority = 50;
  pthread_setschedparam(handle, SCHED_FIFO, &param);

  /*
  GThread* motionThread = g_thread_create(motionThreadCallback, NULL, true, &err);
  if(err)
  {
    g_warning("Could not create motion thread: %s", err->message);
  }
  g_thread_set_priority(motionThread, G_THREAD_PRIORITY_HIGH);
  */
  


  GThread* cognitionThread = g_thread_create(cognitionThreadCallback, NULL, true, &err);
  if(err)
  {
    g_warning("Could not create cognition thread: %s", err->message);
  }

  /*
  if(motionThread != NULL)
  {
    g_thread_join(motionThread);
  }*/

  if(cognitionThread != NULL)
  {
    g_thread_join(cognitionThread);
  }

  return 0;
}//end main
