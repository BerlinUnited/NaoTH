/* 
 * File:   SoundControl.h
 * Author: Oliver Welter
 *
 * Created on 16. Juni 2009, 20:44
 */

#ifndef _SOUNDCONTROL_H
#define	_SOUNDCONTROL_H


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>

#include <pthread.h>
#include <alsa/asoundlib.h>
#include <sys/types.h>

#include "Representations/Infrastructure/SoundData.h"

using namespace std;

namespace naoth
{

class WaveHeader
{
public:
  //RIFF-Chunk
  unsigned int fileLength;

  //Format-Chunk
  unsigned int formatLength;
  unsigned short formatTag;
  unsigned short channels;
  unsigned int samplesPerSec;
  unsigned int avgBytesPerSec;
  unsigned short blockAlign;
  unsigned short bitsPerSample;

  //Data-Chunk
  unsigned int dataLength;
};


class SoundControl
{
public:
  SoundControl();
  ~SoundControl();
  void setSoundData(const SoundData& theSoundData);
  void play();

private:
  string media_path;
  int err;
  snd_pcm_t *playback_handle;
  snd_pcm_hw_params_t *hw_params;
  WaveHeader header;
  snd_pcm_uframes_t periodSize;
  SoundData theSoundDataCopy;
  pthread_t playThreadID;
  char *filename;
  bool is_running;

  int init();
  void deinit();
  void change_volume(const SoundData::SpeakerID id,const int volume);
  void change_mute(const bool mute);

};

} // end namespace naoth

#endif	/* _SOUNDCONTROL_H */

