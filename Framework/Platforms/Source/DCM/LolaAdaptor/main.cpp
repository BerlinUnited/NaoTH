/**
 * @file main.cpp
 *
 * @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
 */

#include "LolaAdaptor.h"

#include <csignal>
#include <sys/stat.h>

using namespace naoth;
using namespace std;

// std::atomic_int framesSinceCognitionLastSeen;
std::atomic_bool running;
std::atomic_bool already_got_signal;

#define TO_STRING_INT(x) #x
#define TO_STRING(x) TO_STRING_INT(x)

// handle signals to stop the binary
void got_signal(int sigid)
{
  // notify all threads to stop
  running = false;

  system("/usr/bin/paplay Media/naoth_stop.wav");

  if(sigid == SIGTERM || sigid == SIGINT) // graceful stop
  {
    std::cout << "shutdown requested by kill signal" << sigid << std::endl;
    
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

bool fileExists (const std::string& filename) {
  struct stat buffer;   
  return (stat (filename.c_str(), &buffer) == 0); 
}

int main(int /*argc*/, char **/*argv[]*/)
{
  std::cout << "=========================================="  << std::endl;
  std::cout << "LoalAdaptor compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

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
  // g_type_init();

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


  // create the controller
  if(fileExists("/usr/bin/lola") || fileExists("/opt/aldebaran/bin/lola"))
  {
    LolaAdaptor theLolaAdaptor;

    // try to start lola
    theLolaAdaptor.start();
  }

  return 0;
}//end main
