/**
 * @file naoth.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "NaoController.h"

#include "Tools/Debug/DebugBufferedOutput.h"

#include "Cognition.h"
#include "Motion.h"
#include "Tools/DummpyCallable.h"

#include <glib.h>
#include <glib-object.h>
//#include <rttools/rtthread.h>

using namespace naoth;

void got_signal(int)
{
  // do something
  std::cout << "catched signal" << std::endl;

  Trace::getInstance().dump();
  Stopwatch::getInstance().dump("cognition");

  sync();

  exit(0);
}//end got_signal

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

// a semaphore for sychronization with the DCM
sem_t* dcm_sem = SEM_FAILED;


void* cognitionThreadCallback(void* ref)
{
  NaoController* theController = static_cast<NaoController*> (ref);

  while(true)
  {
    theController->callCognition();
  }

  return NULL;
}//end cognitionThreadCallback


void* motionThreadCallback(void* ref)
{
  NaoController* theController = static_cast<NaoController*> (ref);

  StopwatchItem stopwatch;
  while(true)
  {
    theController->callMotion();
    
    if(sem_wait(dcm_sem) == -1)
    {
      cerr << "lock errno: " << errno << endl;
    }

    Stopwatch::getInstance().notifyStop(stopwatch);
    Stopwatch::getInstance().notifyStart(stopwatch);
    PLOT("_MotionCycle", stopwatch.lastValue);
  }//end while

  return NULL;
}//end motionThreadCallback



int main(int argc, char *argv[])
{

    std::cout << "=========================================="  << std::endl;
    std::cout << "NaoTH compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

    #ifdef BZR_REVISION
    std::cout << "Revision number: " << BZR_REVISION << std::endl;
    #endif
    #ifdef BZR_BRANCHINFO
    std::cout << "Branch info: " << BZR_BRANCHINFO << std::endl;
    #endif
    std::cout << "==========================================\n"  << std::endl;

  // init glib
  g_type_init();
  if (!g_thread_supported())
    g_thread_init(NULL);

  // react on "kill"
  struct sigaction sa;
  memset( &sa, 0, sizeof(sa) );
  sa.sa_handler = got_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGTERM,&sa,NULL);
  

  // O_CREAT - create a new semaphore if not existing
  // open semaphore
  if((dcm_sem = sem_open("motion_trigger", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
  {
    perror("NaoController: sem_open");
    exit(-1);
  }


  // create the controller
  NaoController theController;
  Cognition theCognition;
  Motion theMotion;
  theController.registerCallbacks(&theMotion, &theCognition);


  // waiting for the first rise of the semaphore
  // bevore starting the threads
  sem_wait(dcm_sem);


  // create the motion thread
  // !!we use here a pthread directly, because glib doustn't support priorities
  pthread_t motionThread;
  pthread_create(&motionThread, 0, motionThreadCallback, (void*)&theController);

  // set the pririty of the motion thread to 50
  sched_param param;
  param.sched_priority = 50;
  pthread_setschedparam(motionThread, SCHED_FIFO, &param);

  /*
  GThread* motionThread = g_thread_create(motionThreadCallback, &theController, true, &err);
  if(err)
  {
    g_warning("Could not create motion thread: %s", err->message);
  }
  g_thread_set_priority(motionThread, G_THREAD_PRIORITY_HIGH);
  */
  

  GError* err = NULL;
  GThread* cognitionThread = g_thread_create(cognitionThreadCallback, (void*)&theController, true, &err);
  if(err)
  {
    g_warning("Could not create cognition thread: %s", err->message);
  }


  //if(motionThread != NULL)
  {
    pthread_join(motionThread, NULL);
    //g_thread_join(motionThread);
  }

  if(cognitionThread != NULL)
  {
    g_thread_join(cognitionThread);
  }

  return 0;
}//end main
