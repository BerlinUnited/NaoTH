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
  void setSoundData(const SoundPlayData& theSoundData);
  void play();

private:
  std::string media_path;
  int err;
  SoundPlayData theSoundDataCopy;
  pthread_t playThreadID;
  std::string filename;
  bool is_running;

  void change_volume(const SoundPlayData::SpeakerID id,const int volume);
  void change_mute(const bool mute);

};

} // end namespace naoth

#endif	/* _SOUNDCONTROL_H */

