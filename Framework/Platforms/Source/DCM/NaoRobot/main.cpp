/**
 * @file main.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#include "NaoController.h"
#include <Tools/ThreadUtil.h>

#include <glib.h>
#include <glib-object.h>
#include <csignal>
//#include <rttools/rtthread.h>
#include <atomic>

#include <errno.h>

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace naoth;
using namespace std;

// a semaphore for sychronization with the DCM
sem_t* dcm_sem = SEM_FAILED;

std::atomic_int framesSinceCognitionLastSeen(0);
// control variable for the motion and cognition threads
std::atomic_bool running(true); 
std::atomic_bool already_got_signal(false);

// handle signals to stop the binary
void got_signal(int sigid)
{
  // notify all threads to stop
  running = false;
  
  // raise the semaphore, so that the motion thread can stop gracefully
  if (sem_post(dcm_sem) == -1) {
    std::cerr << "dcm_sem lock errno: " << errno << std::endl;
  }

  system("/usr/bin/paplay Media/naoth_stop.wav");

  if(sigid == SIGTERM || sigid == SIGINT) // graceful stop
  {
    std::cout << "shutdown requested by kill signal " << sigid << std::endl;
    
    if (already_got_signal) {
      std::cout << "WARNING: received repeated kill signals. Graceful stop was not possible. Will kill." << std::endl;
    } else {
      // remember that we got a signal in case we don't manage to stop the binary gracefully
      already_got_signal = true;
      // stop signal handling for now and give the binary time to stop gracefully
      return;
    }
  } 
  else if(sigid == SIGSEGV) // segmentation fault
  {
    std::cerr << "SEGMENTATION FAULT" << std::endl;
    
    std::cout << "dumping traces" << std::endl;
    Trace::getInstance().dump();
    //StopwatchManager::getInstance().dump("cognition");

    std::cout << "syncing file system..." ;
    sync();
    std::cout << " finished." << std::endl;
  } 
  else
  {
    std::cerr << "caught unknown signal " << sigid << std::endl;
  }

  // set the default handler for the signal and forward the signal
  std::signal(sigid, SIG_DFL);
  std::raise(sigid);

}//end got_signal

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
          std::cerr << "lock errno: " << errno << endl;
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
  std::cout << "[NaoRobot] start motion thread" << std::endl;
  
  framesSinceCognitionLastSeen = 0;

  NaoController* theController = static_cast<NaoController*> (ref);

  //Stopwatch stopwatch;
  while(running)
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
      std::cerr << "dcm_sem lock errno: " << errno << std::endl;
    }

    //stopwatch.stop();
    //stopwatch.start();
  }//end while

  std::cout << "[NaoRobot] stop motion thread" << std::endl;
  
  return NULL;
}//end motionThreadCallback


// used to determine the NAO version
bool fileExists(const std::string& filename) {
  struct stat buffer;
  return (stat(filename.c_str(), &buffer) == 0);
}

// determine if it's NAO 6
bool isNAO6() {
    return fileExists("/usr/bin/lola") ||
           fileExists("/opt/aldebaran/bin/lola");
}

#define TO_STRING_INT(x) #x
#define TO_STRING(x) TO_STRING_INT(x)

int main(int /*argc*/, char **/*argv[]*/)
{
  // determine the NAO version
  static const bool NAO6 = isNAO6();

  std::cout << "=========================================="  << std::endl;
  std::cout << "NaoTH compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

  std::cout << "Detected NAO version " << (NAO6 ? "V6" : "<= V5") << std::endl;

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

  //
  // react on "kill" and segmentation fault:
  // Signal     Value     Action   Comment
  // --------------------------------------------------------
  // SIGSEGV      11       Core    Invalid memory reference
  // SIGINT        2       Term    Interrupt from keyboard
  // SIGQUIT       3       Core    Quit from keyboard
  // SIGKILL       9       Term    Kill signal
  //
  std::signal(SIGTERM, got_signal);
  std::signal(SIGTERM, got_signal);
  std::signal(SIGINT,  got_signal);
  std::signal(SIGSEGV, got_signal);
  
  // TODO: why do we need that?
  if(chdir("/home/nao") != 0) {
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
  NaoController theController(NAO6);
  naoth::init_agent(theController);


  // waiting for the first rise of the semaphore
  // bevore starting the threads
  std::cerr << "[naoth] wait for DCM" << std::endl;
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
  
  pthread_setname_np(motionThread, "Motion");
  
  
  std::thread cognitionThread = std::thread([&theController]
  {
    while(running) {
      theController.runCognition();
      framesSinceCognitionLastSeen = 0;
      std::this_thread::yield();
    }
  });
  
  ThreadUtil::setName(cognitionThread, "Cognition");

  //if(motionThread != NULL)
  {
    pthread_join(motionThread, NULL);
  }
  std::cout << "[NaoRobot] Motion thread joined. " << std::endl;

  if(cognitionThread.joinable()) {
    cognitionThread.join();
  }
  std::cout << "[NaoRobot] Cognition thread joined. " << std::endl;
  std::cout << "[NaoRobot] Main stopped. " << std::endl;
  
  return 0;
}//end main
