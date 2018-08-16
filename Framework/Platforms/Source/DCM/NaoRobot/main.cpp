/**
 * @file main.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#include "NaoController.h"

#include <glib.h>
#include <glib-object.h>
#include <signal.h>
//#include <rttools/rtthread.h>
#include <atomic>

#include <errno.h>

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace naoth;

std::atomic_int framesSinceCognitionLastSeen;

void got_signal(int t)
{

  if(t == SIGTERM || t == SIGINT) {
    std::cout << "shutdown requested by kill signal" << t << std::endl;
  } 
  else if(t == SIGSEGV) 
  {
    std::cerr << "SEGMENTATION FAULT" << std::endl;
    
    std::cout << "dumping traces" << std::endl;
    Trace::getInstance().dump();
    //StopwatchManager::getInstance().dump("cognition");

    std::cout << "syncing file system..." ;
    sync();
    std::cout << " finished." << std::endl;
  } else {
    std::cerr << "caught unknown signal " << t << std::endl;
  }

  exit(0);
}//end got_signal


// a semaphore for sychronization with the DCM
sem_t* dcm_sem = SEM_FAILED;


/* 
// Just some experiments with the RT-Threads
// not used yet
class TestThread : public RtThread
{
	public:
    TestThread(){}
    ~TestThread(){}
    NaoController* theController;

    virtual void *execute()
    {
      Stopwatch stopwatch;
      while(true)
      {
        theController->callMotion();
    
        if(sem_wait(dcm_sem) == -1)
        {
          cerr << "lock errno: " << errno << endl;
        }

        StopwatchManager::getInstance().notifyStop(stopwatch);
        StopwatchManager::getInstance().notifyStart(stopwatch);
        PLOT("_MotionCycle", stopwatch.lastValue);
      }//end while

      return NULL;
    }
    virtual void postExecute(){}
    virtual void preExecute(){}
} motionRtThread;
*/

void* motionThreadCallback(void* ref)
{
  framesSinceCognitionLastSeen = 0;

  NaoController* theController = static_cast<NaoController*> (ref);

  //Stopwatch stopwatch;
  while(true)
  {
    if(framesSinceCognitionLastSeen > 4000)
    {
      std::cerr << std::endl;
      std::cerr << "+==================================+" << std::endl;
      std::cerr << "| NO MORE MESSAGES FROM COGNITION! |" << std::endl;
      std::cerr << "+==================================+" << std::endl;
      std::cerr << "dumping traces" << std::endl;
      Trace::getInstance().dump();
      //StopwatchManager::getInstance().dump("cognition");

      #ifndef WIN32
      std::cerr << "syncing file system..." ;
      sync();
      std::cerr << " finished." << std::endl;
      #endif

      ASSERT(false && "Cognition seems to be dead");
    }//end if
    framesSinceCognitionLastSeen++;

    theController->runMotion();
    
    if(sem_wait(dcm_sem) == -1) {
      cerr << "lock errno: " << errno << endl;
    }

    //stopwatch.stop();
    //stopwatch.start();
  }//end while

  return NULL;
}//end motionThreadCallback

#define TO_STRING_INT(x) #x
#define TO_STRING(x) TO_STRING_INT(x)

int main(int /*argc*/, char **/*argv[]*/)
{
  std::cout << "=========================================="  << std::endl;
  std::cout << "NaoTH compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

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

  // init glib
  g_type_init();

  // react on "kill" and segmentation fault
  struct sigaction saKill;
  memset( &saKill, 0, sizeof(saKill) );
  saKill.sa_handler = got_signal;
  sigfillset(&saKill.sa_mask);
  sigaction(SIGTERM,&saKill,NULL);
  
  struct sigaction saInt;
  memset( &saInt, 0, sizeof(saInt) );
  saInt.sa_handler = got_signal;
  sigfillset(&saInt.sa_mask);
  sigaction(SIGINT,&saInt,NULL);
  
  struct sigaction saSeg;
  memset( &saSeg, 0, sizeof(saSeg) );
  saSeg.sa_handler = got_signal;
  sigfillset(&saSeg.sa_mask);
  sigaction(SIGSEGV,&saSeg,NULL);

  int retChDir = chdir("/home/nao");
  if(retChDir != 0)
  {
    std::cerr << "Could not change working directory" << std::endl;
  }

  // O_CREAT - create a new semaphore if not existing
  // open semaphore
  if((dcm_sem = sem_open("motion_trigger", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
  {
    perror("NaoController: sem_open");
    exit(-1);
  }

  // create the controller
  NaoController theController;
  naoth::init_agent(theController);


  // waiting for the first rise of the semaphore
  // bevore starting the threads
  sem_wait(dcm_sem);


  // create the motion thread
  // !!we use here a pthread directly, because std::thread doesn't support priorities
  pthread_t motionThread;
  int err = pthread_create(&motionThread, NULL, motionThreadCallback, (void*)&theController);
  if (err != 0) {
    handle_error_en(err, "create motionThread");
  }
  
  // set the pririty of the motion thread to 50
  sched_param param;
  param.sched_priority = 50;
  err = pthread_setschedparam(motionThread, SCHED_FIFO, &param);
  if (err != 0) {
    handle_error_en(err, "set priority motionThread");
  }
  
  std::thread cognitionThread = std::thread([&theController]
  {
    while(true) {
      theController.runCognition();
      framesSinceCognitionLastSeen = 0;
      std::this_thread::yield();
    }
  });


  //if(motionThread != NULL)
  {
    pthread_join(motionThread, NULL);
  }

  if(cognitionThread.joinable())
  {
    cognitionThread.join();
  }

  return 0;
}//end main
