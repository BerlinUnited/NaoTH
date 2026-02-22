/**
 * @file main.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#include "BoosterController.h"

#include "Tools/ThreadUtil.h"
#include "Tools/FileUtils.h"

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


std::atomic_int framesSinceCognitionLastSeen(0);

// control variable for the motion and cognition threads
std::atomic_bool running(true); 
std::atomic_bool already_got_signal(false);

// handle signals to stop the binary
void got_signal(int sigid)
{
  // notify all threads to stop
  running = false;
  
  //system("/usr/bin/paplay Media/naoth_stop.wav");

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



#define TO_STRING_INT(x) #x
#define TO_STRING(x) TO_STRING_INT(x)

int main(int /*argc*/, char **/*argv[]*/)
{
  std::cout << "=========================================="  << std::endl;
  std::cout << "NaoTH compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

  std::cout << "Robot: Booster K1" << std::endl;

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
  if(chdir("~") != 0) {
    std::cerr << "Could not change working directory" << std::endl;
  }

  // create the controller
  BoosterController theController;
  naoth::init_agent(theController);


  std::thread motionThread = std::thread([&theController]
  {
    while(running) {
      theController.runMotion();
      std::this_thread::yield();
    }
  });
  ThreadUtil::setName(motionThread, "Motion");

  
  std::thread cognitionThread = std::thread([&theController]
  {
    while(running) {
      theController.runCognition();
      framesSinceCognitionLastSeen = 0;
      std::this_thread::yield();
    }
  });
  
  ThreadUtil::setName(cognitionThread, "Cognition");

  if(motionThread.joinable()) {
    motionThread.join();
  }
  std::cout << "[BoosterRobot] Motion thread joined. " << std::endl;

  if(cognitionThread.joinable()) {
    cognitionThread.join();
  }
  std::cout << "[BoosterRobot] Cognition thread joined. " << std::endl;
  std::cout << "[BoosterRobot] Main stopped. " << std::endl;
  
  return 0;
}//end main
