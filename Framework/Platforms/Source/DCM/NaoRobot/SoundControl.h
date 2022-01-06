/*
 * File:   SoundControl.h
 * Author: Oliver Welter
 *
 * Created on 16. Juni 2009, 20:44
 */

#ifndef _SOUNDCONTROL_H
#define	_SOUNDCONTROL_H

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "Representations/Infrastructure/SoundData.h"

namespace naoth
{

class SoundControl
{
public:
  SoundControl();
  ~SoundControl();
  void setSoundData(const SoundPlayData& theSoundData);
  
private:
  void play();

  std::atomic<bool> stopping;
  std::thread playThread;

  std::string media_path;
  std::string filename;
  
  std::condition_variable new_data_avaliable;
  std::mutex dataMutex;
  
};

} // end namespace naoth

#endif	/* _SOUNDCONTROL_H */

