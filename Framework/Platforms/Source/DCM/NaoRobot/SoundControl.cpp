
#include "SoundControl.h"

#include <Tools/ThreadUtil.h>
#include <iostream>
#include <cstdlib>

using namespace naoth;
using namespace std;

SoundControl::SoundControl() :
  stopping(false),
  media_path("Media/")
{
  playThread = std::thread(&SoundControl::play, this);

  ThreadUtil::setPriority(playThread, ThreadUtil::Priority::lowest);
  ThreadUtil::setName(playThread, "SoundControl");
}

SoundControl::~SoundControl()
{
  stopping = true;
  std::cout << "[SoundControl] stop wait" << std::endl;

  // NOTE: wake up the thread in case it was waiting for the new data
  new_data_avaliable.notify_one();
  // wait for the thread to stop if necessary
  if(playThread.joinable()) {
    playThread.join();
  }
  std::cout << "[SoundControl] stop done" << std::endl;
}

void SoundControl::setSoundData(const SoundPlayData& theSoundData)
{
  // try to get the lock on the data
  // if we cannot get the lock, the the thread is still working
  std::unique_lock<std::mutex> lock(dataMutex, std::try_to_lock);

  if(theSoundData.soundFile == "" || !lock.owns_lock()) {
    return;
  }

  filename = media_path + theSoundData.soundFile;

  // release the data lock and notify the waiting thread
  lock.unlock();
  new_data_avaliable.notify_one();  
}

void SoundControl::play()
{
  while(!stopping) 
  {
    //std::cout << "[SoundControl] wait for new data to play" << std::endl;
    std::unique_lock<std::mutex> lock(dataMutex);
    new_data_avaliable.wait(lock);

    std::cout << "[SoundControl] play " << filename << std::endl;
    std::string cmd = "/usr/bin/paplay " + filename;
    std::system(cmd.c_str());

    //TODO: handle the case if the file does not exist:
    //      if the file does not exist, then we end up in 
    //      a loop printing error messages
    
    filename = "";
    // NOTE: should we unlock here?
    // lock.unlock();
  }
}
