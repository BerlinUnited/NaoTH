#include "SoundControl.h"

using namespace naoth;
using namespace std;

inline static void *play_thread_handler(void *data)
{
  SoundControl *sc_thd = (SoundControl *)data;
  sc_thd->play();
  pthread_exit(NULL);
  void *ret = NULL;
  return ret;
}

SoundControl::SoundControl():
media_path("Media/"),
is_running(false)
{
}

SoundControl::~SoundControl()
{
}

void SoundControl::setSoundData(const SoundPlayData& theSoundData)
{
  //create new thread for playing filename
  if((theSoundData.soundFile != "") && (!is_running))
  {
    this->filename = media_path + theSoundData.soundFile;

    //create thread-handler
    if(pthread_create(&playThreadID,NULL,play_thread_handler,this)!=0)
    {
      std::cerr << "Error: Not able to create MotionDebug-ReceiveThread" << endl;
      return;
    }
    
    pthread_setname_np(playThreadID, "SoundControl");
  }

/*
  for(int i=0;i<SoundPlayData::numOfSpeaker;i++)
  {
    if(theSoundData.volume[i] != theSoundDataCopy.volume[i])
    {
      change_volume((SoundPlayData::SpeakerID) i,theSoundData.volume[i]);
    }
  }

  if(theSoundData.mute != theSoundDataCopy.mute)
  {
    change_mute(theSoundData.mute);
  }
*/

  theSoundDataCopy = SoundPlayData(theSoundData);
}//end setSoundData

/* Don't use directly, it will be called by play_thread_handler
 * use setSoundData!
*/
void SoundControl::play()
{
  is_running=true;

  // play a sound that the user knows we recognized his shutdown request
  std::string cmd = "/usr/bin/paplay " + filename;
  system(cmd.c_str());

  is_running=false;
}

void SoundControl::change_volume(const SoundPlayData::SpeakerID /*id*/,const int /*volume*/)
{
  //if((err = snd_mixer_selem_set_playback_volume(,,volume))<0)
  //{
    //std::cerr << "Unable to set volume(" << volume << ")" << endl;
    return;
  //}
}

void SoundControl::change_mute(const bool /*mute*/)
{
  if(false)
  {
    std::cerr << "Unable to mute" << endl;
    return;
  }
}
