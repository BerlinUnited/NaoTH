#include "AudioConfigSetter.h"

AudioConfigSetter::AudioConfigSetter(){
	getDebugParameterList().add(&params);
}

AudioConfigSetter::~AudioConfigSetter()
{
	getDebugParameterList().remove(&params);
}

void AudioConfigSetter::execute()
{
  //FIXME
  /*
  if(params.whistleListFile != getAudioControl().whistleListFile)
  {
    getAudioControl().whistleListFile = params.whistleListFile;
  }
  */
  if(params.activeChannels != getAudioControl().activeChannels)
  {
	  getAudioControl().activeChannels = params.activeChannels;
  }
  if (params.onOffSwitch != getAudioControl().onOffSwitch)
  {
	  getAudioControl().onOffSwitch = params.onOffSwitch;
  }

  /*
  if(params.threshold != getAudioControl().threshold)
  {
    getAudioControl().threshold = params.threshold;
  }
  
  if(params.checkAllWhistles != getAudioControl().checkAllWhistles)
  {
    getAudioControl().checkAllWhistles = params.checkAllWhistles;
  }
  
  if(params.saveRawAudio != getAudioControl().saveRawAudio)
  {
    getAudioControl().saveRawAudio = params.saveRawAudio;
  }
  */
}

