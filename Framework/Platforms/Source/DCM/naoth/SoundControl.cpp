#include "SoundControl.h"

using namespace naoth;

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

/*
 * Initialize sound-device
 * needs member-variables:
 * header
 *  
 * fills member-variables:
 * playback-handle
 * hw_params
 * periodSize
 *
 */
int SoundControl::init()
{
  int dir;
  snd_pcm_format_t format;

  switch (header.bitsPerSample)
  {
    case 8:
      format = SND_PCM_FORMAT_S8;
      break;
    case 16:
      format = SND_PCM_FORMAT_S16_LE;
      break;
    case 24:
      format = SND_PCM_FORMAT_S24_LE;
      break;
    case 32:
      format = SND_PCM_FORMAT_S32_LE;
      break;
    default:
      format = SND_PCM_FORMAT_UNKNOWN;
  }
  
  string device = "default";
  if((err = snd_pcm_open(&playback_handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0)
  {
    std::cerr << "Cannot open audio device " << device.c_str() << ": " << snd_strerror(err) << endl;
    return -1;
	}

  if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
  {
    std::cerr <<  "Cannot allocate hardware parameter structure (" << snd_strerror(err) << ")" << endl;
    return -1;
	}

  if((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0)
  {
    std::cerr << "Cannot initialize hardware parameter structure (" << snd_strerror (err) << ")" << endl;
    return -1;
  }

  if((err = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
  {
    std::cerr << "Cannot set access type (" << snd_strerror(err) << ")" << endl;
    return -1;
	}

  if ((err = snd_pcm_hw_params_set_format(playback_handle, hw_params, format)) < 0)
  {
    std::cerr << "Cannot set sample format (" << snd_strerror(err) << ")" << endl;
    return -1;
	}

	if((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &header.samplesPerSec, 0)) < 0)
  {
    std::cerr << "Cannot set sample rate (" << snd_strerror(err) << ")" << endl;
    return -1;
	}

	if((err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, header.channels)) < 0)
  {
    std::cerr << "Cannot set channel count (" << snd_strerror (err) << ")" << endl;
    return -1;
	}

	if((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0) 
  {
    std::cerr << "Cannot set parameters (" << snd_strerror(err) << ")" << endl;
    return -1;
	}

  if((err = snd_pcm_hw_params_get_period_size(hw_params, &periodSize, &dir)) < 0)
  {
    std::cerr << "Cannot get Period-Size" << endl;
    return -1;
  }

	snd_pcm_hw_params_free(hw_params);

	if((err = snd_pcm_prepare(playback_handle)) < 0) 
  {
    std::cerr << "Cannot prepare audio interface for use (" << snd_strerror(err) << ")" << endl;
    return -1;
	}
  
  return 1;
}

void SoundControl::deinit()
{
  snd_pcm_drain(playback_handle);
  snd_pcm_close(playback_handle);
}

void SoundControl::setSoundData(const SoundPlayData& theSoundData)
{
  //create new thread for playing filename
  if((theSoundData.soundFile != "") && (!is_running))
  {
    string filename = media_path + theSoundData.soundFile;
    this->filename = new char[filename.size()];
    strcpy(this->filename,filename.c_str());

    //create thread-handler
    if(pthread_create(&playThreadID,NULL,play_thread_handler,this)!=0)
    {
      std::cerr << "Error: Not able to create MotionDebug-ReceiveThread" << endl;
      return;
    }
  }

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

  theSoundDataCopy = SoundPlayData(theSoundData);
}

/* Don't use directly, it will be called by play_thread_handler
 * use setSoundData!
*/
void SoundControl::play()
{
  is_running=true;
  int dataBufferSize;
  char *dataBuffer;
  char buffer[44];
  int err;

  ifstream is;

  is.open(filename,ios::binary);
  if(is.fail())
  {
    std::cerr << "Unable to open file " << filename << endl;
    return;
  }
  is.seekg(ios::beg);
  is.read(buffer,44);

  //Chunk-Header

  if(strncmp(buffer,"RIFF",4)!=0)
  {
    std::cerr << "No RIFF-Header found" << endl;
    return;
  }

  if(strncmp(buffer + 8,"WAVE",4)!=0)
  {
    std::cerr << "No Wave-Header found" << endl;
    return;
  }

  memcpy((void *) &header.fileLength,(void *) (buffer+4),4);
  header.fileLength-=8;

  if(strncmp(buffer + 12, "fmt ",4)!=0)
  {
    std::cerr << "No Format-Header found" << endl;
    return;
  }

  memcpy((void *) &header.formatLength,(void *) (buffer+16),4);
  memcpy((void *) &header.formatTag,(void *) (buffer+20),2);
  memcpy((void *) &header.channels,(void *) (buffer+22),2);
  memcpy((void *) &header.samplesPerSec,(void *) (buffer+24),4);
  memcpy((void *) &header.avgBytesPerSec,(void *) (buffer+28),4);
  memcpy((void *) &header.blockAlign,(void *) (buffer+32),2);
  memcpy((void *) &header.bitsPerSample,(void *) (buffer+34),2);

  if(strncmp(buffer + 36,"data",4)!=0)
  {
    std::cerr << "No Data-Header found" << endl;
    return;
  }

  memcpy((void *) &header.dataLength,(void *) (buffer+40),4);

  //Check if soundcard could be addressed, if not return
  if(init()<0)
  {
    return;
  }

  dataBufferSize = periodSize * header.channels * header.bitsPerSample / 8;

  /*
  std::cerr << "PeriodSize: " << periodSize << endl;
  std::cerr << "Channels: " << header.channels << endl;
  std::cerr << "BitsPerSample: " << header.bitsPerSample << endl;
  std::cerr << "DataLength: " << header.dataLength << endl;
  std::cerr << "DataBufferSize: " << dataBufferSize << endl;
  */

  dataBuffer = new char[dataBufferSize];

  if (!dataBuffer)
  {
    std::cerr << "Unable to allocate memory for data" << endl;
  }

  for(int i=0;i<=(int)(header.dataLength / dataBufferSize)+1; i++)
  {
    if(is.eof())
    {
      break;
    }
    is.read(dataBuffer,dataBufferSize);

    err = snd_pcm_writei(playback_handle, dataBuffer, periodSize);
    if (err == -EPIPE)
    {
      std::cerr << "underrun" << endl;
      snd_pcm_prepare(playback_handle);
      break;
    }
    else if (err < 0) {
      std::cerr << "write error: " << snd_strerror(err) << endl;
      break;
    }
    else if (err != (int)periodSize)
    {
      std::cerr << "short write" << endl;
      break;
    }
  }

  deinit();
  is.close();
  delete[] dataBuffer;
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
